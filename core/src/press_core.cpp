//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2018 Renc. All rights reserved.
//

//#include "io/binary.hpp"

//#include "topology/auxiliary.hpp"
//#include "topology/ac_automaton.hpp"
//#include "topology/huffman.hpp"
//#include "util/timer.hpp"
//#include "util/helper.hpp"
//#include "service/spatial_compressor.hpp"
//#include "service/temporal_compressor.hpp"

//#include "service/query_processor.hpp"
//#include "service/trajectory_reformattor.hpp"

#include <csignal>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "service/map_matcher.hpp"
#include "third_party/log.h"
#include "third_party/picojson.h"
#include "topology/graph.hpp"
#include "topology/grid_index.hpp"
#include "topology/sp_table.hpp"
#include "topology/trajectory.hpp"
#include "user_defined/factory.hpp"
#include "util/utility.hpp"

// Global states and storage.
bool roadnetReady = false;
bool gridIndexReady = false;
bool spTableReady = false;
std::string roadnetName;
Graph roadnet;
GridIndex gridIndex;
SPTable spTable;
std::vector<GPSTrajectory> gpsTrajectories;
std::vector<MapMatchedTrajectory> mapMatchedTrajectories;

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
  MAP_MATCHER,
};
// Clear a component.
void clearComponent(Component comp) {
  switch (comp) {
    case Component::ROADNET: {
      roadnetReady = false;
      gridIndexReady = false;
      spTableReady = false;
      gpsTrajectories.clear();
      mapMatchedTrajectories.clear();
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
    case Component::MAP_MATCHER: {
      gpsTrajectories.clear();
      mapMatchedTrajectories.clear();
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

// Handle dump roadnet to ${TMP_FOLDER}/[roadnetName]/road_network.bin
void handleDumpRoadnetToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/";
  if (!fileExists(folderName.c_str()) && !createFolder(folderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folderName + "road_network.bin";
  FileWriter graphWriter(fileName.c_str(), true);
  roadnet.store(graphWriter);
  response = successResponse("Roadnet is dumped to " + fileName + ".");
}

// Handle load roadnet from ${TMP_FOLDER}/[folder]/road_network.bin
void handleLoadRoadnetFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::ROADNET);
  auto folder = requestJson.get("Folder").get<std::string>();
  auto fileName = config.tmpFolder + folder + "/road_network.bin";
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

// Handle dump grid index to ${TMP_FOLDER}/[roadnetName]/grid_index.bin
void handleDumpGridIndexToBinary(picojson::value& requestJson, std::string& response) {
  if (!gridIndexReady) {
    response = errorResponse("Grid index is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/";
  if (!fileExists(folderName.c_str()) && !createFolder(folderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folderName + "grid_index.bin";
  FileWriter gridIndexWriter(fileName.c_str(), true);
  gridIndex.store(gridIndexWriter);
  response = successResponse("Grid index is dumped to " + fileName + ".");
}

// Handle load grid index from ${TMP_FOLDER}/[folder]/grid_index.bin
void handleLoadGridIndexFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::GRID_INDEX);
  auto folder = requestJson.get("Folder").get<std::string>();
  if (!roadnetReady || folder != roadnetName) {
    response = errorResponse("Roadnet is not ready or roadnet mismatch with grid index.");
    return;
  }
  auto fileName = config.tmpFolder + folder + "/grid_index.bin";
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

// Handle dump SP table to ${TMP_FOLDER}/[roadnetName]/sp_table.bin
void handleDumpSPTableToBinary(picojson::value& requestJson, std::string& response) {
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/";
  if (!fileExists(folderName.c_str()) && !createFolder(folderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto fileName = folderName + "sp_table.bin";
  FileWriter spTableWriter(fileName.c_str(), true);
  spTable.store(spTableWriter);
  response = successResponse("SP table is dumped to " + fileName + ".");
}

// Handle load SP table from ${TMP_FOLDER}/[folder]/sp_table.bin
void handleLoadSPTableFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::SP_TABLE);
  auto folder = requestJson.get("Folder").get<std::string>();
  if (!roadnetReady || folder != roadnetName) {
    response = errorResponse("Roadnet is not ready or roadnet mismatch with SP table.");
    return;
  }
  auto fileName = config.tmpFolder + folder + "/sp_table.bin";
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

// Handle add and map match GPS trajectory.
void handleAddGPSTrajectoryAndMapMatch(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!gridIndexReady) {
    response = errorResponse("Grid index is not ready.");
    return;
  }
  auto sigmaZ = requestJson.get("SigmaZ").get<double>();
  auto maxGPSBias = requestJson.get("MaxGPSBias").get<double>();
  auto maxDistDifference = requestJson.get("MaxDistDifference").get<double>();
  auto fileName = config.dataFolder + requestJson.get("FileName").get<std::string>();
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "GPS data source file doesn't exist: " << fileName;
    response = errorResponse("GPS data source file doesn't exist.");
    return;
  }
  auto gpsTrajectoryReaderType = getGPSTrajectoryReaderType(
    requestJson.get("GPSTrajectoryReaderType").get<std::string>()
  );
  auto gpsTrajectoryReader = Factory::getGPSTrajectoryReader(gpsTrajectoryReaderType);
  GPSTrajectory gpsTrajectory;
  gpsTrajectoryReader->readGPSTrajectory(fileName, gpsTrajectory);
  MapMatcher mapMatcher;
  mapMatcher.mapMatch(
    spTable,
    roadnet,
    gridIndex,
    sigmaZ,
    maxGPSBias,
    maxDistDifference,
    gpsTrajectory,
    gpsTrajectories,
    mapMatchedTrajectories
  );
  response = successResponse("Added original and map matched GPS trajectory.");
}

// Handle clear original and map matched GPS trajectories.
void handleClearGPSAndMapMatchedTrajectories(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::MAP_MATCHER);
  response = successResponse("Added original and map matched GPS trajectory.");
}

// Handle dump GPS trajectories to ${TMP_FOLDER}/[roadnetName]/gps_trajectories/[0 .. (n - 1)].bin
void handleDumpGPSTrajectoriesToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto gpsFolderName = config.tmpFolder + roadnetName + "/gps_trajectories/";
  if (!fileExists(gpsFolderName.c_str()) && !createFolder(gpsFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << gpsFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(gpsFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << gpsFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < gpsTrajectories.size(); ++i) {
    auto fileName = gpsFolderName + std::to_string(i) + ".bin";
    FileWriter gpsWriter(fileName.c_str(), true);
    gpsTrajectories.at(i).store(gpsWriter);
  }
  response = successResponse("GPS trajectories are dumped to " + gpsFolderName + ".");
}

// Handle dump map matched trajectories to
// ${TMP_FOLDER}/[roadnetName]/map_matched_trajectories/[0 .. (n - 1)].bin
void handleDumpMapMatchedTrajectoriesToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto mmFolderName = config.tmpFolder + roadnetName + "/map_matched_trajectories/";
  if (!fileExists(mmFolderName.c_str()) && !createFolder(mmFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << mmFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(mmFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << mmFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < mapMatchedTrajectories.size(); ++i) {
    auto fileName = mmFolderName + std::to_string(i) + ".bin";
    FileWriter mmWriter(fileName.c_str(), true);
    mapMatchedTrajectories.at(i).store(mmWriter);
  }
  response = successResponse("Map matched trajectories are dumped to " + mmFolderName + ".");
}

// Handle load GPS trajectories from ${TMP_FOLDER}/[folder]/gps_trajectories/[0 .. (n - 1)].bin
void handleLoadGPSTrajectoriesFromBinary(picojson::value& requestJson, std::string& response) {
  auto folder = requestJson.get("Folder").get<std::string>();
  if (!roadnetReady || folder != roadnetName) {
    response = errorResponse("Roadnet is not ready or roadnet mismatch with GPS trajectories.");
    return;
  }
  auto folderName = config.tmpFolder + folder + "/gps_trajectories/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list GPS trajectory folder: " << folderName;
    response = errorResponse("Fail to list GPS trajectory folder.");
    return;
  }
  gpsTrajectories.clear();
  for (auto& file: files) {
    FileReader gpsTrajectoryReader((folderName + file).c_str(), true);
    gpsTrajectories.emplace_back(GPSTrajectory(gpsTrajectoryReader));
  }
  response = successResponse("GPS trajectories are loaded from " + folderName + ".");
}

// Handle load map matched trajectories from
// ${TMP_FOLDER}/[folder]/map_matched_trajectories/[0 .. (n - 1)].bin
void handleLoadMapMatchedTrajectoriesFromBinary(
  picojson::value& requestJson,
  std::string& response
) {
  auto folder = requestJson.get("Folder").get<std::string>();
  if (!roadnetReady || folder != roadnetName) {
    response = errorResponse(
      "Roadnet is not ready or roadnet mismatch with map matched trajectories."
    );
    return;
  }
  auto folderName = config.tmpFolder + folder + "/map_matched_trajectories/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list map matched trajectory folder: " << folderName;
    response = errorResponse("Fail to list map matched trajectory folder.");
    return;
  }
  mapMatchedTrajectories.clear();
  for (auto& file: files) {
    FileReader mmTrajectoryReader((folderName + file).c_str(), true);
    mapMatchedTrajectories.emplace_back(MapMatchedTrajectory(mmTrajectoryReader));
  }
  response = successResponse("Map matched trajectories are loaded from " + folderName + ".");
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
      } else if (cmd == "AddGPSTrajectoryAndMapMatch") {
        handleAddGPSTrajectoryAndMapMatch(requestJson, response);
      } else if (cmd == "ClearGPSAndMapMatchedTrajectories") {
        handleClearGPSAndMapMatchedTrajectories(requestJson, response);
      } else if (cmd == "DumpGPSTrajectoriesToBinary") {
        handleDumpGPSTrajectoriesToBinary(requestJson, response);
      } else if (cmd == "DumpMapMatchedTrajectoriesToBinary") {
        handleDumpMapMatchedTrajectoriesToBinary(requestJson, response);
      } else if (cmd == "LoadGPSTrajectoriesFromBinary") {
        handleLoadGPSTrajectoriesFromBinary(requestJson, response);
      } else if (cmd == "LoadMapMatchedTrajectoriesFromBinary") {
        handleLoadMapMatchedTrajectoriesFromBinary(requestJson, response);
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
