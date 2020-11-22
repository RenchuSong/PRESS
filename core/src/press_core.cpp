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
//#include "topology/graph.hpp"
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
//#include "user_defined/factory.hpp"

#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>

#include "util/utility.hpp"
#include "third_party/picojson.h"
#include "third_party/log.h"

// Config for PRESS core.
struct CoreConfig {
  std::string tmpFolder;
  std::string dataFolder;
  std::string logsFolder;
  std::string logLevel;
  
  CoreConfig(char* configPath) {
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
void configLogger(CoreConfig& config) {
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

int main(int argc, char** argv) {
  // Read config file.
  CoreConfig config(argv[1]);
  // Config logger.
  configLogger(config);
  // Daemonize.
  daemonize();
  // Register signal handlers.
  registerSignalHandler();

  FILE_LOG(TLogLevel::linfo) << "PRESS core started.";
  // Handle requests.
  while (true) {
    FILE_LOG(TLogLevel::linfo) << "I'm alive.";
    std::cout << "test" << std::endl;
    sleep(1);
  }

  return EXIT_SUCCESS;
}
