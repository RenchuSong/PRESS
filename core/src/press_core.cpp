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

// Handle system signals
void signalHandler(int signum) {
  FILE_LOG(TLogLevel::linfo) << "Interrupt signal (" << signum << ") received. Exit PRESS core.";
  exit(signum);
}

// Get log level.
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

int main(int argc, char** argv) {
  // Read config file.
  std::string configRaw = readFile(argv[1]);
  picojson::value configJson;
  std::string err = picojson::parse(configJson, configRaw);
  if (!err.empty()) {
    std::cerr << "Failed to parse config file (" << argv[1] << "): " << err << std::endl;
    exit(EXIT_FAILURE);
  }
  // Get config entries.
  auto& coreConfig = configJson.get("core");
  std::string& tmpFolder = coreConfig.get("tmp").get<std::string>();
  auto& dataFolder = coreConfig.get("data").get<std::string>();
  auto& logsFolder = coreConfig.get("logs").get<std::string>();
  auto& logLevel = coreConfig.get("log_level").get<std::string>();
  // Config logger.
  FILELog::ReportingLevel() = getLogLevel(logLevel);
  FILE* log_fd = fopen((logsFolder + "press_core_admin.log").c_str(), "a");
  Output2FILE::Stream() = log_fd;
  // Daemonize.
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
  // Register signal handler
  signal(SIGABRT, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGSEGV, signalHandler);
  signal(SIGTERM, signalHandler);
  FILE_LOG(TLogLevel::linfo) << "PRESS core started.";
  // Handle requests.
  while (true) {
    FILE_LOG(TLogLevel::linfo) << "I'm alive.";
    std::cout << "test" << std::endl;
    sleep(1);
  }

  return EXIT_SUCCESS;
}
