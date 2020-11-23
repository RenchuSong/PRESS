//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2018 Renc. All rights reserved.
//

//#include "io/file_reader.hpp"
//#include "io/file_writer.hpp"
//#include "io/binary.hpp"
#include "topology/graph.hpp"
//#include "topology/trajectory.hpp"
//#include "topology/geospatial_structures.hpp"
//#include "topology/sp_table.hpp"
//#include "topology/grid_index.hpp"
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
bool isRoadnetReady = false;
std::string roadnetName;
Graph roadnet;

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

// Clear everything.
void clearSystem() {
  isRoadnetReady = false;
}

// Error response.
std::string errorResponse(std::string errMsg) {
  return std::string("{\"success\": false, \"message\": \"") + errMsg + "\"}";
}

// Handle read roadnet from ${DATA}/[folder]/road_network.txt
void handleReadRoadnetFromFile(picojson::value& requestJson, std::string& response) {
  clearSystem();
  auto& folder = requestJson.get("folder").get<std::string>();
  auto graphReaderType = getGraphReaderType(requestJson.get("graphReaderType").get<std::string>());
  auto graphReader = Factory::getGraphReader(graphReaderType);
  roadnetName = folder;
  graphReader->readGraph(config.dataFolder + folder + "/road_network.txt", roadnet);
  isRoadnetReady = true;
  response = std::string("{\"success\": true, \"message\": \"Roadnet of dataset ")
    + folder + " is loaded.\"}";
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
      auto& cmd = requestJson.get("cmd").get<std::string>();
      // Handles each type of requests.
      if (cmd == "read_roadnet_from_file") {
        handleReadRoadnetFromFile(requestJson, response);
      } else {
        FILE_LOG(TLogLevel::lerror) << "Unknown request: " << request;
        response = errorResponse("Unknown request.");
      }
      writeNext(response);
      FILE_LOG(TLogLevel::ldebug) << "Responded packet << " << response;
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
