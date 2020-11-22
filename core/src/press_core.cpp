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

#include "util/utility.hpp"
#include "third_party/picojson.h"
#include "third_party/log.h"

int main(int argc, char** argv) {
  // Read config file.
  std::string configRaw = readFile(argv[1]);
  picojson::value configJson;
  std::string err = picojson::parse(configJson, configRaw);
  if (!err.empty()) {
    std::cerr << "Failed to parse config file (" << argv[1] << "): " << err << std::endl;
    return 1;
  }
  // Get config entries.
  auto& coreConfig = configJson.get("core");
  std::string& tmpFolder = coreConfig.get("tmp").get<std::string>();
  auto& dataFolder = coreConfig.get("data").get<std::string>();
  auto& logsFolder = coreConfig.get("logs").get<std::string>();
  // Config logger.
  FILELog::ReportingLevel() = TLogLevel::ldebug;
  FILE* log_fd = fopen((logsFolder + "press_core_admin.log").c_str(), "a");
  Output2FILE::Stream() = log_fd;
  // Flag if roadnetwork and auxiliary trajectory structures are constructed.
  FILE_LOG(TLogLevel::linfo) << tmpFolder << " " << dataFolder << " " << logsFolder;

  return 0;
}
