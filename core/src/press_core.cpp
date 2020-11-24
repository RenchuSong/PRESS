//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2018 Renc. All rights reserved.
//

//#include "io/binary.hpp"
#include "topology/graph.hpp"
//#include "topology/trajectory.hpp"
//#include "topology/geospatial_structures.hpp"
#include "topology/sp_table.hpp"
#include "topology/grid_index.hpp"
//#include "topology/auxiliary.hpp"
//#include "topology/ac_automaton.hpp"
//#include "topology/huffman.hpp"
//#include "util/timer.hpp"
//#include "util/helper.hpp"
//#include "service/spatial_compressor.hpp"
//#include "service/temporal_compressor.hpp"
//#include "service/map_matcher.hpp"
//#include "service/query_processor.hpp"
//#include "service/trajectory_reformattor.hpp"
#include "user_defined/factory.hpp"

#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#include "util/utility.hpp"
#include "third_party/picojson.h"
#include "third_party/log.h"

// Global states and storage.
bool roadnetReady = false;
bool gridIndexReady = false;
bool spTableReady = false;
std::string roadnetName;
Graph roadnet;
GridIndex gridIndex;
SPTable spTable;

// Config for PRESS core.
struct CoreConfig {
  std::string tmpFolder;
  std::string dataFolder;
  std::string logsFolder;
  std::string logLevel;

  void setConfig(char* configPath) {
    std::string configRaw = readFile(configPath);
    picojson::value configJson;
    std::string err = picojson::parse(configJson, configRaw);
    if (!err.empty()) {
      std::cerr << "Failed to parse config file (" << configPath << "): " << err << std::endl;
      exit(EXIT_FAILURE);
    }
    // Get config entries.
    auto& coreConfig = configJson.get("core");
    tmpFolder = coreConfig.get("tmp").get<std::string>();
    dataFolder = coreConfig.get("data").get<std::string>();
    logsFolder = coreConfig.get("logs").get<std::string>();
    logLevel = coreConfig.get("log_level").get<std::string>();
  }
};
CoreConfig config;

// Config logger for PRESS core.
TLogLevel getLogLevel(std::string& level) {
  if (level == "ERROR") {
    return TLogLevel::lerror;
  }
  if (level == "INFO") {
    return TLogLevel::linfo;
  }
  if (level == "WARN") {
    return TLogLevel::lwarning;
  }
  if (level == "DEBUG") {
    return TLogLevel::ldebug;
  }
  return TLogLevel::linfo;
}
void configLogger() {
  FILELog::ReportingLevel() = getLogLevel(config.logLevel);
  FILE* log_fd = fopen((config.logsFolder + "press_core_admin.log").c_str(), "a");
  Output2FILE::Stream() = log_fd;
}

// Handle system signals
void signalHandler(int signum) {
  FILE_LOG(TLogLevel::linfo) << "Interrupt signal (" << signum << ") received. Exit PRESS core.";
  exit(signum);
}

// Daemonize.
void daemonize() {
  pid_t pid, sid;
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  umask(0);
  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

// Register signal handlers.
void registerSignalHandler() {
  signal(SIGABRT, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGSEGV, signalHandler);
  signal(SIGTERM, signalHandler);
}

enum Component {
  ROADNET,
  GRID_INDEX,
  SP_TABLE,
};
// Clear a component.
void clearComponent(Component comp) {
  switch (comp) {
    case Component::ROADNET: {
      roadnetReady = false;
      gridIndexReady = false;
      spTableReady = false;
      break;
    }
    case Component::GRID_INDEX: {
      gridIndexReady = false;
      break;
    }
    case Component::SP_TABLE: {
      spTableReady = false;
      break;
    }
  }
}

// Error response.
std::string errorResponse(std::string errMsg) {
  return std::string("{\"success\": false, \"message\": \"") + errMsg + "\"}";
}

// Success response with message.
std::string successResponse(std::string msg) {
  return std::string("{\"success\": true, \"message\": \"") + msg + "\"}";
}

// Handle read roadnet from ${DATA_FOLDER}/[folder]/road_network.txt
void handleReadRoadnetFromDataSource(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::ROADNET);
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto fileName = config.dataFolder + folder + "/road_network.txt";
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "Data source file doesn't exist: " << folder;
    response = errorResponse("Data source file doesn't exist.");
    return;
  }
  auto graphReaderType = getGraphReaderType(requestJson.get("GraphReaderType").get<std::string>());
  auto graphReader = Factory::getGraphReader(graphReaderType);
  graphReader->readGraph(fileName, roadnet);
  roadnetReady = true;
  roadnetName = folder;
  response = successResponse("Roadnet of dataset " + folder + " is loaded.");
}

// Handle dump roadnet to ${TMP_FOLDER}/[folder]/road_network.bin
void handleDumpRoadnetToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  if (!fileExists(folder.c_str()) && !createFolder(folder)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folder;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folder + "/road_network.bin";
  FileWriter graphWriter(fileName.c_str(), true);
  roadnet.store(graphWriter);
  response = successResponse("Roadnet is dumped to " + fileName + ".");
}

// Handle load roadnet from ${TMP_FOLDER}/[folder]/road_network.bin
void handleLoadRoadnetFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::ROADNET);
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  auto fileName = folder + "/road_network.bin";
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "Roadnet binary file does not exist: " << fileName;
    response = errorResponse("Failed to load roadnet.");
    return;
  }
  FileReader graphReader(fileName.c_str(), true);
  roadnet.load(graphReader);
  roadnetReady = true;
  roadnetName = folder;
  response = successResponse("Roadnet is loaded from " + fileName + ".");
}

// Handle build grid index from loaded roadnet.
void handleBuildGridIndex(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  clearComponent(Component::GRID_INDEX);
  auto gridWidth = requestJson.get("CellWidth").get<double>();
  auto gridHeight = requestJson.get("CellHeight").get<double>();
  gridIndex.build(roadnet, gridWidth, gridHeight);
  gridIndexReady = true;
  response = successResponse("Built grid index from roadnet " + roadnetName + ".");
}

// Handle dump grid index to ${TMP_FOLDER}/[folder]/grid_index.bin
void handleDumpGridIndexToBinary(picojson::value& requestJson, std::string& response) {
  if (!gridIndexReady) {
    response = errorResponse("Grid index is not ready.");
    return;
  }
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  if (!fileExists(folder.c_str()) && !createFolder(folder)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folder;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folder + "/grid_index.bin";
  FileWriter gridIndexWriter(fileName.c_str(), true);
  gridIndex.store(gridIndexWriter);
  response = successResponse("Grid index is dumped to " + fileName + ".");
}

// Handle load grid index from ${TMP_FOLDER}/[folder]/grid_index.bin
void handleLoadGridIndexFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::GRID_INDEX);
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  auto fileName = folder + "/grid_index.bin";
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "Grid index binary file does not exist: " << fileName;
    response = errorResponse("Failed to load grid index.");
    return;
  }
  FileReader gridIndexReader(fileName.c_str(), true);
  gridIndex.load(gridIndexReader);
  gridIndexReady = true;
  response = successResponse("Grid index is loaded from " + fileName + ".");
}

// Handle build SP table from loaded roadnet.
void handleBuildSPTable(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  clearComponent(Component::SP_TABLE);
  auto maxDist = requestJson.get("MaxDist").get<double>();
  spTable.build(roadnet, maxDist);
  spTableReady = true;
  response = successResponse("Built SP table from roadnet " + roadnetName + ".");
}

// Handle dump SP table to ${TMP_FOLDER}/[folder]/sp_table.bin
void handleDumpSPTableToBinary(picojson::value& requestJson, std::string& response) {
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  if (!fileExists(folder.c_str()) && !createFolder(folder)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folder;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folder + "/sp_table.bin";
  FileWriter spTableWriter(fileName.c_str(), true);
  spTable.store(spTableWriter);
  response = successResponse("SP table is dumped to " + fileName + ".");
}

// Handle load SP table from ${TMP_FOLDER}/[folder]/sp_table.bin
void handleLoadSPTableFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::SP_TABLE);
  auto folder = config.tmpFolder + requestJson.get("Folder").get<std::string>();
  auto fileName = folder + "/sp_table.bin";
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "SP table binary file does not exist: " << fileName;
    response = errorResponse("Failed to load SP table.");
    return;
  }
  FileReader spTableReader(fileName.c_str(), true);
  spTable.load(spTableReader);
  spTableReady = true;
  response = successResponse("SP table is loaded from " + fileName + ".");
}

struct ReqRespHelper {
  std::string inPath;
  std::string outPath;
  std::ifstream reqStream;
  std::ofstream respStream;
  std::string data;
  std::string response;

  ReqRespHelper(char* inPath, const char* outPath): inPath(inPath), outPath(outPath) { }

  std::string& readNext() {
    reqStream.open(inPath, std::ifstream::in);
    if (!reqStream.is_open()) {
      FILE_LOG(TLogLevel::lerror) << "Fail to open request stream.";
      exit(EXIT_FAILURE);
    }
    std::getline(reqStream, data);
    reqStream.close();
    return data;
  }

  void writeNext(std::string& data) {
    respStream.open(outPath, std::ofstream::out);
    if (!respStream.is_open()) {
      FILE_LOG(TLogLevel::lerror) << "Fail to open response stream.";
      exit(EXIT_FAILURE);
    }
    respStream << data;
    respStream.close();
  }

  void handleRequest(std::string& request) {
    // Parse request.
    picojson::value requestJson;
    std::string err = picojson::parse(requestJson, request);
    if (!err.empty()) {
      FILE_LOG(TLogLevel::lerror) << "Failed to parse request (" << request << ")";
      auto errResp = errorResponse("Failed to parse request.");
      writeNext(errResp);
      return;
    }
    try {
      auto& cmd = requestJson.get("Cmd").get<std::string>();
      // Handles each type of requests.
      if (cmd == "ReadRoadnetFromDataSource") {
        handleReadRoadnetFromDataSource(requestJson, response);
      } else if (cmd == "DumpRoadnetToBinary") {
        handleDumpRoadnetToBinary(requestJson, response);
      } else if (cmd == "LoadRoadnetFromBinary") {
        handleLoadRoadnetFromBinary(requestJson, response);
      } else if (cmd == "BuildGridIndex") {
        handleBuildGridIndex(requestJson, response);
      } else if (cmd == "DumpGridIndexToBinary") {
        handleDumpGridIndexToBinary(requestJson, response);
      } else if (cmd == "LoadGridIndexFromBinary") {
        handleLoadGridIndexFromBinary(requestJson, response);
      } else if (cmd == "BuildSPTable") {
        handleBuildSPTable(requestJson, response);
      } else if (cmd == "DumpSPTableToBinary") {
        handleDumpSPTableToBinary(requestJson, response);
      } else if (cmd == "LoadSPTableFromBinary") {
        handleLoadSPTableFromBinary(requestJson, response);
      } else {
        FILE_LOG(TLogLevel::lerror) << "Unknown request: " << request;
        response = errorResponse("Unknown request.");
      }
      writeNext(response);
      FILE_LOG(TLogLevel::ldebug) << "Responded packet << " << response;
    } catch (const std::exception& ex) {
      FILE_LOG(TLogLevel::lerror) << "Failed to handle request (" << request << "): " << ex.what();
      auto errResp = errorResponse("Failed to handle request.");
      writeNext(errResp);
    } catch (const std::string& ex) {
      FILE_LOG(TLogLevel::lerror) << "Failed to handle request (" << request << "): " << ex;
      auto errResp = errorResponse("Failed to handle request.");
      writeNext(errResp);
    } catch (...) {
      FILE_LOG(TLogLevel::lerror) << "Failed to handle request (" << request << ")";
      auto errResp = errorResponse("Failed to handle request.");
      writeNext(errResp);
    }
  }
};

int main(int argc, char** argv) {
  // Read config file.
  config.setConfig(argv[1]);
  // Config logger.
  configLogger();
  // Daemonize.
//  daemonize();
  // Register signal handlers.
  registerSignalHandler();
  // Open communication channel.
  ReqRespHelper reqRespHelper(argv[2], argv[3]);
  FILE_LOG(TLogLevel::linfo) << "PRESS core started.";
  // Handle requests.
  while (true) {
    auto& request = reqRespHelper.readNext();
    FILE_LOG(TLogLevel::linfo) << "Request received >> " << request;
    reqRespHelper.handleRequest(request);
    FILE_LOG(TLogLevel::linfo) << "Response sent";
  }

  return EXIT_SUCCESS;
}
