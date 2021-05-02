//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2018 Renc. All rights reserved.
//


#include "util/timer.hpp"
//#include "util/helper.hpp"

#include <csignal>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <vector>
#include <unistd.h>

#include "service/map_matcher.hpp"
#include "service/query_processor.hpp"
#include "service/spatial_compressor.hpp"
#include "service/temporal_compressor.hpp"
#include "service/trajectory_reformattor.hpp"
#include "third_party/log.h"
#include "third_party/picojson.h"
#include "topology/ac_automaton.hpp"
#include "topology/auxiliary.hpp"
#include "topology/huffman.hpp"
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
bool acAutomatonReady = false;
bool huffmanReady = false;
bool auxiliaryReady = false;
std::string roadnetName;
Graph roadnet;
GridIndex gridIndex;
SPTable spTable;
ACAutomaton acAutomaton;
Huffman huffman;
Auxiliary auxiliary;
std::vector<GPSTrajectory> gpsTrajectories;
std::vector<MapMatchedTrajectory> mapMatchedTrajectories;
std::vector<PRESSTrajectory> pressTrajectories;
std::vector<std::vector<int> > spCompressionResults;
std::vector<Binary> fstCompressionResults;
std::vector<std::vector<TemporalPair> > btcCompressionResults;
std::vector<PRESSCompressedTrajectory> pressCompressedTrajectories;
std::vector<PRESSTrajectory> pressDeCompressedTrajectories;

// Config for PRESS core.
struct CoreConfig {
  std::string expFolder;
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
    expFolder = configJson.get("experiments").get<std::string>();
    dataFolder = configJson.get("data").get<std::string>();
    auto& coreConfig = configJson.get("core");
    tmpFolder = coreConfig.get("tmp").get<std::string>();
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
  RE_FORMATTER,
  TRAINER,
};
// Clear a component.
void clearComponent(Component comp) {
  switch (comp) {
    case Component::ROADNET: {
      roadnetReady = false;
      gridIndexReady = false;
      spTableReady = false;
      acAutomatonReady = false;
      huffmanReady = false;
      auxiliaryReady = false;
      gpsTrajectories.clear();
      mapMatchedTrajectories.clear();
      pressTrajectories.clear();
      spCompressionResults.clear();
      fstCompressionResults.clear();
      btcCompressionResults.clear();
      pressCompressedTrajectories.clear();
      pressDeCompressedTrajectories.clear();
      break;
    }
    case Component::GRID_INDEX: {
      gridIndexReady = false;
      break;
    }
    case Component::SP_TABLE: {
      spTableReady = false;
      acAutomatonReady = false;
      huffmanReady = false;
      auxiliaryReady = false;
      gpsTrajectories.clear();
      mapMatchedTrajectories.clear();
      pressTrajectories.clear();
      spCompressionResults.clear();
      fstCompressionResults.clear();
      btcCompressionResults.clear();
      pressCompressedTrajectories.clear();
      pressDeCompressedTrajectories.clear();
      break;
    }
    case Component::MAP_MATCHER: {
      gpsTrajectories.clear();
      mapMatchedTrajectories.clear();
      break;
    }
    case Component::RE_FORMATTER: {
      pressTrajectories.clear();
      break;
    }
    case Component::TRAINER: {
      acAutomatonReady = false;
      huffmanReady = false;
      auxiliaryReady = false;
      spCompressionResults.clear();
      fstCompressionResults.clear();
      btcCompressionResults.clear();
      pressCompressedTrajectories.clear();
      pressDeCompressedTrajectories.clear();
      break;
    }
  }
}

// Error response.
std::string errorResponse(std::string errMsg) {
  return std::string("{\"Success\": false, \"Message\": \"") + errMsg + "\"}";
}

// Success response with message.
std::string successResponse(std::string msg) {
  return std::string("{\"Success\": true, \"Message\": \"") + msg + "\"}";
}
// Success response with data.
std::string successResponseWithData(std::string& data) {
  return std::string("{\"Success\": true, \"Message\": \"\", \"Data\": ") + data + "}";
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

// Handle get roadnet as a JSON.
void handleGetRoadnet(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  std::stringstream ss;
  roadnet.toJSON(ss);
  std::string json = ss.str();
  response = successResponseWithData(json);
}

// Handle dump roadnet to ${EXP_FOLDER}/[folder]/road_network.bin
void handleDumpRoadnetToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
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

// Handle load roadnet from ${EXP_FOLDER}/[folder]/road_network.bin
void handleLoadRoadnetFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::ROADNET);
  auto folder = requestJson.get("Folder").get<std::string>();
  auto fileName = config.expFolder + folder + "/road_network.bin";
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
  response = successResponse("Built grid index from roadnet.");
}

// Handle dump grid index to ${EXP_FOLDER}/[folder]/grid_index.bin
void handleDumpGridIndexToBinary(picojson::value& requestJson, std::string& response) {
  if (!gridIndexReady) {
    response = errorResponse("Grid index is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
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

// Handle load grid index from ${EXP_FOLDER}/[folder]/grid_index.bin
void handleLoadGridIndexFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::GRID_INDEX);
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto folder = requestJson.get("Folder").get<std::string>();
  auto fileName = config.expFolder + folder + "/grid_index.bin";
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

// Handle dump SP table to ${EXP_FOLDER}/[folder]/sp_table.bin
void handleDumpSPTableToBinary(picojson::value& requestJson, std::string& response) {
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
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

// Handle load SP table from ${EXP_FOLDER}/[folder]/sp_table.bin
void handleLoadSPTableFromBinary(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::SP_TABLE);
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto folder = requestJson.get("Folder").get<std::string>();
  auto fileName = config.expFolder + folder + "/sp_table.bin";
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
  response = successResponse("Cleared original and map matched GPS trajectories.");
}

// Handle dump GPS trajectories to ${EXP_FOLDER}/[folder:exp/gps_trajectories]/[0 .. (n - 1)].bin
void handleDumpGPSTrajectoriesToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto gpsFolderName = config.expFolder + folder + "/";
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

// Handle get GPS trajectories sample rates in an array.
void handleGetGPSTrajectoriesSampleRate(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  std::stringstream ss;
  ss << "[";
  for (int i = 0; i < gpsTrajectories.size(); ++i) {
    if (i > 0) {
      ss << ",";
    }
    ss << gpsTrajectories.at(i).getSampleRate();
  }
  ss << "]";
  std::string json = ss.str();
  response = successResponseWithData(json);
}

// Handle dump map matched trajectories to
// ${EXP_FOLDER}/[folder:exp/map_matched_trajectories]/[0 .. (n - 1)].bin
void handleDumpMapMatchedTrajectoriesToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto mmFolderName = config.expFolder + folder + "/";
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

// Handle load GPS trajectories from ${EXP_FOLDER}/[folder:exp/gps_trajectories]/[0 .. (n - 1)].bin
void handleLoadGPSTrajectoriesFromBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
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

// Handle show GPS trajectory from ${EXP_FOLDER}/[folder:exp/gps_trajectories]/[ID].bin
void handleShowGPSTrajectory(picojson::value& requestJson, std::string& response) {
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto& id = requestJson.get("ID").get<std::string>();
  auto fileName = config.expFolder + folder + "/" + id + ".bin";
  if (!fileExists(fileName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "GPS binary file doesn't exist: " << fileName;
    response = errorResponse("GPS binary file doesn't exist.");
    return;
  }
  FileReader gpsTrajectoryReader(fileName.c_str(), true);
  std::stringstream ss;
  GPSTrajectory(gpsTrajectoryReader).toJSON(ss);
  std::string json = ss.str();
  response = successResponseWithData(json);
}

// Handle load map matched trajectories from
// ${EXP_FOLDER}/[folder:exp/map_matched_trajectories]/[0 .. (n - 1)].bin
void handleLoadMapMatchedTrajectoriesFromBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready or roadnet mismatch with map matched trajectories."
    );
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
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

// Handle reformat trajectories.
void handleReformatTrajectories(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (gpsTrajectories.size() != mapMatchedTrajectories.size()) {
    response = errorResponse("GPS trajectories and map matched trajectories count are not same.");
    return;
  }
  pressTrajectories.clear();
  TrajectoryReformatter refomatter;
  for (int i = 0; i < gpsTrajectories.size(); ++i) {
    PRESSTrajectory pressTrajectory;
    refomatter.generateTrajectory(
      spTable,
      roadnet,
      gpsTrajectories.at(i),
      mapMatchedTrajectories.at(i),
      pressTrajectory
    );
    pressTrajectories.emplace_back(pressTrajectory);
  }
  response = successResponse(
    "Re-formatted GPS and map matched trajectories into PRESS trajectory."
  );
}

// Handle clear PRESS trajectories.
void handleClearPRESSTrajectories(picojson::value& requestJson, std::string& response) {
  clearComponent(Component::RE_FORMATTER);
  response = successResponse("Cleared PRESS trajectories.");
}

// Handle dump PRESS trajectories to
// ${EXP_FOLDER}/[folder:exp/press_trajectories]/[0 .. (n - 1)].bin
void handleDumpPRESSTrajectoriesToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto pressFolderName = config.expFolder + folder + "/";
  if (!fileExists(pressFolderName.c_str()) && !createFolder(pressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << pressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(pressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << pressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < pressTrajectories.size(); ++i) {
    auto pressTrajectoryFolder = pressFolderName + std::to_string(i) + "/";
    if (!createFolder(pressTrajectoryFolder)) {
      FILE_LOG(TLogLevel::lerror) << "Failed to create press folder: " << pressFolderName;
      response = errorResponse("Failed to create press folder.");
      return;
    }
    auto spatialName = pressTrajectoryFolder + "spatial.bin";
    FileWriter spatialWriter(spatialName.c_str(), true);
    auto temporalName = pressTrajectoryFolder + "temporal.bin";
    FileWriter temporalWriter(temporalName.c_str(), true);
    pressTrajectories.at(i).store(spatialWriter, temporalWriter);
  }
  response = successResponse("PRESS trajectories are dumped to " + pressFolderName + ".");
}

// Handle load PRESS trajectories from ${EXP_FOLDER}/[folder:exp/press_trajectories]/[0 .. (n - 1)]/[spatial|temporal].bin
void handleLoadPRESSTrajectoriesFromBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto folderName = config.expFolder + folder + "/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list PRESS trajectory folder: " << folderName;
    response = errorResponse("Fail to list PRESS trajectory folder.");
    return;
  }
  pressTrajectories.clear();
  for (auto& file: files) {
    FileReader spatialReader((folderName + file + "/spatial.bin").c_str(), true);
    FileReader temporalReader((folderName + file + "/temporal.bin").c_str(), true);
    pressTrajectories.emplace_back(PRESSTrajectory(spatialReader, temporalReader));
  }
  response = successResponse("PRESS trajectories are loaded from " + folderName + ".");
}

// Handle show PRESS trajectory from ${EXP_FOLDER}/[folder:exp/press_trajectories]/[ID]/[spatial|temporal].bin
void handleShowPRESSTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& folder = requestJson.get("Folder").get<std::string>();
  auto& id = requestJson.get("ID").get<std::string>();
  auto folderName = config.expFolder + folder + "/" + id;
  if (
    !fileExists((folderName + "/spatial.bin").c_str()) ||
    !fileExists((folderName + "/temporal.bin").c_str())
  ) {
    FILE_LOG(TLogLevel::lerror) << "PRESS binary file doesn't exist: " << folderName;
    response = errorResponse("PRESS binary file doesn't exist.");
    return;
  }
  FileReader spatialReader((folderName + "/spatial.bin").c_str(), true);
  FileReader temporalReader((folderName + "/temporal.bin").c_str(), true);
  std::stringstream ss;
  PRESSTrajectory(spatialReader, temporalReader).toJSON(roadnet, ss);
  std::string json = ss.str();
  response = successResponseWithData(json);
}

// Handle train AC automaton, huffman tree and build auxiliary.
void handleTrainACAutomatonHuffmanTreeAndBuildAuxiliary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (pressTrajectories.size() == 0) {
    response = errorResponse("There is no press trajectory that can be used for training FST.");
    return;
  }
  clearComponent(Component::TRAINER);
  SpatialCompressor spatialCompressor;
  int theta = (int)requestJson.get("Theta").get<double>();
  std::vector<int> spCompressResult;
  std::vector<std::vector<int> > spCompressResults;
  for (auto& pressTrajectory: pressTrajectories) {
    spatialCompressor.shortestPathCompression(
      roadnet,
      spTable,
      pressTrajectory.getSpatialComponent(),
      spCompressResult
    );
    spCompressResults.emplace_back(spCompressResult);
  }
  acAutomaton.build(roadnet, spCompressResults, theta);
  huffman.build(acAutomaton);
  auxiliary.build(roadnet, spTable, acAutomaton);
  acAutomatonReady = true;
  huffmanReady = true;
  auxiliaryReady = true;
  response = successResponse("AC automaton, huffman tree and auxiliary are ready.");
}

// Handle dump AC automaton, huffman tree and auxiliary.
void handleDumpACAutomatonHuffmanTreeAndAuxiliaryToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman is not ready.");
    return;
  }
  if (!auxiliaryReady) {
    response = errorResponse("Auxiliary is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/";
  if (!fileExists(folderName.c_str()) && !createFolder(folderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << folderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  auto acName = folderName + "ac_automaton.bin";
  FileWriter acWriter(acName.c_str(), true);
  acAutomaton.store(acWriter);
  auto huffmanName = folderName + "huffman.bin";
  FileWriter huffmanWriter(huffmanName.c_str(), true);
  huffman.store(huffmanWriter);
  auto auxiliaryName = folderName + "auxiliary.bin";
  FileWriter auxiliaryWriter(auxiliaryName.c_str(), true);
  auxiliary.store(auxiliaryWriter);

  response = successResponse(
    "AC automaton, huffman tree and auxiliary are dumped to " + folderName + "."
  );
}

// Handle load AC automaton, huffman tree and auxiliary from ${TMP_FOLDER}/[folder]/
void handleLoadACAutomatonHuffmanTreeAndAuxiliaryFromBinary(
  picojson::value& requestJson,
  std::string& response
) {
  clearComponent(Component::TRAINER);
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready."
    );
    return;
  }
  auto acName = config.tmpFolder + roadnetName + "/ac_automaton.bin";
  if (!fileExists(acName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "AC automaton binary file does not exist: " << acName;
    response = errorResponse("AC automaton binary file does not exist.");
    return;
  }
  auto huffmanName = config.tmpFolder + roadnetName + "/huffman.bin";
  if (!fileExists(huffmanName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "Huffman tree binary file does not exist: " << huffmanName;
    response = errorResponse("Huffman tree binary file does not exist.");
    return;
  }
  auto auxiliaryName = config.tmpFolder + roadnetName + "/auxiliary.bin";
  if (!fileExists(auxiliaryName.c_str())) {
    FILE_LOG(TLogLevel::lerror) << "Auxiliary binary file does not exist: " << auxiliaryName;
    response = errorResponse("Auxiliary binary file does not exist.");
    return;
  }
  FileReader acReader(acName.c_str(), true);
  FileReader huffmanReader(huffmanName.c_str(), true);
  FileReader auxiliaryReader(auxiliaryName.c_str(), true);
  acAutomaton.load(acReader);
  huffman.load(huffmanReader);
  auxiliary.load(auxiliaryReader);
  acAutomatonReady = true;
  huffmanReady = true;
  auxiliaryReady = true;
  response = successResponse(
    "AC automaton, huffman tree and auxiliary are loaded from "
    + config.tmpFolder + roadnetName + "."
  );
}

// Handle SP compression.
void handleSPCompression(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (pressTrajectories.size() == 0) {
    response = errorResponse("There is no press trajectory to be SP compressed.");
    return;
  }
  spCompressionResults.clear();
  SpatialCompressor spatialCompressor;
  std::vector<int> spCompressResult;
  for (auto& pressTrajectory: pressTrajectories) {
    spatialCompressor.shortestPathCompression(
      roadnet,
      spTable,
      pressTrajectory.getSpatialComponent(),
      spCompressResult
    );
    spCompressionResults.emplace_back(spCompressResult);
  }
  response = successResponse("SP compression finished.");
}

// Handle clear SP compression result.
void handleClearSPCompressionResults(picojson::value& requestJson, std::string& response) {
  spCompressionResults.clear();
  response = successResponse("SP compression results cleared.");
}

// Handle dump SP compression results to ${TMP_FOLDER}/[roadnetName]/sp_compression/[0..(n-1)].bin
void handleDumpSPCompressionResultsToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto spCompressFolderName = config.tmpFolder + roadnetName + "/sp_compression/";
  if (!fileExists(spCompressFolderName.c_str()) && !createFolder(spCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << spCompressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(spCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << spCompressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < spCompressionResults.size(); ++i) {
    auto spCompressionName = spCompressFolderName + std::to_string(i) + ".bin";
    FileWriter spWriter(spCompressionName.c_str(), true);
    auto& spatial = spCompressionResults.at(i);
    spWriter.writeInt((int)spatial.size());
    for (auto id: spatial) {
      spWriter.writeSeparator();
      spWriter.writeInt(id);
    }
    spWriter.writeEol();
  }
  response = successResponse("SP compression results are dumped to " + spCompressFolderName + ".");
}

// Handle load SP compression results from ${TMP_FOLDER}/[folder]/sp_compression/[0..(n-1)].bin
void handleLoadSPCompressionResultsFromBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready or roadnet mismatch with SP compression results."
    );
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/sp_compression/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list SP compression folder: " << folderName;
    response = errorResponse("Fail to list SP compression folder.");
    return;
  }
  spCompressionResults.clear();
  for (auto& file: files) {
    FileReader spReader((folderName + file).c_str(), true);
    std::vector<int> spatial;
    auto len = spReader.nextInt();
    for (auto i = 0; i < len; ++i) {
      spatial.emplace_back(spReader.nextInt());
    }
    spCompressionResults.emplace_back(spatial);
  }
  response = successResponse("SP compression results are loaded from " + folderName + ".");
}

// Handle FST compression.
void handleFSTCompression(picojson::value& requestJson, std::string& response) {
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (spCompressionResults.size() == 0) {
    response = errorResponse("There is no SP compression result to be FST compressed.");
    return;
  }
  fstCompressionResults.clear();
  SpatialCompressor spatialCompressor;
  Binary binary;
  for (auto& spatial: spCompressionResults) {
    spatialCompressor.frequentSubTrajectoryCompresson(
      acAutomaton,
      huffman,
      spatial,
      binary
    );
    fstCompressionResults.emplace_back(binary);
  }
  response = successResponse("FST compression finished.");
}

// Handle clear FST compression result.
void handleClearFSTCompressionResults(picojson::value& requestJson, std::string& response) {
  fstCompressionResults.clear();
  response = successResponse("FST compression results cleared.");
}

// Handle dump FST compression results to ${TMP_FOLDER}/[roadnetName]/fst_compression/[0..(n-1)].bin
void handleDumpFSTCompressionResultsToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto fstCompressFolderName = config.tmpFolder + roadnetName + "/fst_compression/";
  if (!fileExists(fstCompressFolderName.c_str()) && !createFolder(fstCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << fstCompressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(fstCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << fstCompressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < fstCompressionResults.size(); ++i) {
    auto fstCompressionName = fstCompressFolderName + std::to_string(i) + ".bin";
    FileWriter fstWriter(fstCompressionName.c_str(), true);
    fstCompressionResults.at(i).store(fstWriter);
  }
  response = successResponse("FST compression results are dumped to " + fstCompressFolderName + ".");
}

// Handle load FST compression results from ${TMP_FOLDER}/[folder]/fst_compression/[0..(n-1)].bin
void handleLoadFSTCompressionResultsFromBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready."
    );
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/fst_compression/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list FST compression folder: " << folderName;
    response = errorResponse("Fail to list FST compression folder.");
    return;
  }
  fstCompressionResults.clear();
  for (auto& file: files) {
    FileReader fstReader((folderName + file).c_str(), true);
    fstCompressionResults.emplace_back(Binary(fstReader));
  }
  response = successResponse("FST compression results are loaded from " + folderName + ".");
}

// Handle BTC compression.
void handleBTCCompression(picojson::value& requestJson, std::string& response) {
  if (pressTrajectories.size() == 0) {
    response = errorResponse("There is no press trajectory to be BTC compressed.");
    return;
  }
  auto tsnd = requestJson.get("TSND").get<double>();
  auto nstd = requestJson.get("NSTD").get<double>();
  btcCompressionResults.clear();
  TemporalCompressor temporalCompressor;
  std::vector<TemporalPair> btcCompressResult;
  for (auto& pressTrajectory: pressTrajectories) {
    temporalCompressor.boundedTemporalCompression(
      pressTrajectory.getTemporalComponent(),
      btcCompressResult,
      tsnd,
      nstd
    );
    btcCompressionResults.emplace_back(btcCompressResult);
  }
  response = successResponse("BTC compression finished.");
}

// Handle clear BTC compression result.
void handleClearBTCCompressionResults(picojson::value& requestJson, std::string& response) {
  btcCompressionResults.clear();
  response = successResponse("BTC compression results cleared.");
}

// Handle dump BTC compression results to ${TMP_FOLDER}/[roadnetName]/btc_compression/[0..(n-1)].bin
void handleDumpBTCCompressionResultsToBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto btcCompressFolderName = config.tmpFolder + roadnetName + "/btc_compression/";
  if (!fileExists(btcCompressFolderName.c_str()) && !createFolder(btcCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << btcCompressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(btcCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << btcCompressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < btcCompressionResults.size(); ++i) {
    auto btcCompressionName = btcCompressFolderName + std::to_string(i) + ".bin";
    FileWriter btcWriter(btcCompressionName.c_str(), true);
    auto& temporal = btcCompressionResults.at(i);
    btcWriter.writeInt((int)temporal.size());
    for (auto& tpPair: temporal) {
      btcWriter.writeSeparator();
      btcWriter.writeDouble(tpPair.t);
      btcWriter.writeSeparator();
      btcWriter.writeDouble(tpPair.dist);
    }
    btcWriter.writeEol();
  }
  response = successResponse(
    "BTC compression results are dumped to " + btcCompressFolderName + "."
  );
}

// Handle load BTC compression results from ${TMP_FOLDER}/[folder]/btc_compression/[0..(n-1)].bin
void handleLoadBTCCompressionResultsFromBinary(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready."
    );
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/btc_compression/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list BTC compression folder: " << folderName;
    response = errorResponse("Fail to list BTC compression folder.");
    return;
  }
  btcCompressionResults.clear();
  for (auto& file: files) {
    FileReader btcReader((folderName + file).c_str(), true);
    std::vector<TemporalPair> temporal;
    auto len = btcReader.nextInt();
    for (auto i = 0; i < len; ++i) {
      temporal.emplace_back(TemporalPair(btcReader.nextDouble(), btcReader.nextDouble()));
    }
    btcCompressionResults.emplace_back(temporal);
  }
  response = successResponse("BTC compression results are loaded from " + folderName + ".");
}

// Handle PRESS compression.
void threadHSCCompression(
  const Graph& graph,
  const SPTable& spTable,
  const ACAutomaton& acAutomaton,
  const Huffman& huffman,
  std::vector<Binary>& hscCompressResults
) {
  SpatialCompressor spatialCompressor;
  for (auto& pressTrajectory: pressTrajectories) {
    Binary binary;
    spatialCompressor.hybridSpatialCompression(
      graph,
      spTable,
      acAutomaton,
      huffman,
      pressTrajectory.getSpatialComponent(),
      binary
    );
    hscCompressResults.emplace_back(binary);
  }
}
void threadBTCCompression(
  double tsnd,
  double nstd,
  std::vector<std::vector<TemporalPair> >& btcCompressResults
) {
  TemporalCompressor temporalCompressor;
  for (auto& pressTrajectory: pressTrajectories) {
    std::vector<TemporalPair> result;
    temporalCompressor.boundedTemporalCompression(
      pressTrajectory.getTemporalComponent(),
      result,
      tsnd,
      nstd
    );
    btcCompressResults.emplace_back(result);
  }
}
void handlePRESSCompression(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (pressTrajectories.size() == 0) {
    response = errorResponse("There is no press trajectory to be compressed.");
    return;
  }
  pressCompressedTrajectories.clear();
  auto tsnd = requestJson.get("TSND").get<double>();
  auto nstd = requestJson.get("NSTD").get<double>();
  std::vector<Binary> hscCompressResults;
  std::vector<std::vector<TemporalPair> > btcCompressResults;
  // HSC and BTC can be parallelized.
  std::thread hscThread(
    threadHSCCompression,
    std::ref(roadnet),
    std::ref(spTable),
    std::ref(acAutomaton),
    std::ref(huffman),
    std::ref(hscCompressResults)
  );
  std::thread btcThread(
    threadBTCCompression,
    tsnd,
    nstd,
    std::ref(btcCompressResults)
  );
  hscThread.join();
  btcThread.join();
//  // Sequential implementation.
//  threadHSCCompression(
//    roadnet,
//    spTable,
//    acAutomaton,
//    huffman,
//    hscCompressResults
//  );
//  threadBTCCompression(
//    tsnd,
//    nstd,
//    btcCompressResults
//  );
  for (auto i = 0; i < pressTrajectories.size(); ++i) {
    pressCompressedTrajectories.emplace_back(
      PRESSCompressedTrajectory(hscCompressResults.at(i), btcCompressResults.at(i))
    );
  }
  response = successResponse("PRESS compression finished.");
}

// Handle PRESS decompression.
void handlePRESSDeCompression(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (pressCompressedTrajectories.size() == 0) {
    response = errorResponse("There is no compressed press trajectory to be decompressed.");
    return;
  }
  pressDeCompressedTrajectories.clear();
  std::vector<int> hscDeCompressResults;
  SpatialCompressor spatialCompressor;
  for (auto& pressCompressedTrajectory: pressCompressedTrajectories) {
    spatialCompressor.hybridSpatialDecompression(
      roadnet,
      spTable,
      acAutomaton,
      huffman,
      pressCompressedTrajectory.getSpatialComponent(),
      hscDeCompressResults
    );
    pressDeCompressedTrajectories.emplace_back(
      PRESSTrajectory(hscDeCompressResults, pressCompressedTrajectory.getTemporalComponent())
    );
  }
  response = successResponse("PRESS de-compression finished.");
}

// Handle clear PRESS compression result.
void handleClearPRESSCompressionResults(picojson::value& requestJson, std::string& response) {
  pressCompressedTrajectories.clear();
  response = successResponse("PRESS compression results cleared.");
}

// Handle clear PRESS de-compression result.
void handleClearPRESSDeCompressionResults(picojson::value& requestJson, std::string& response) {
  pressDeCompressedTrajectories.clear();
  response = successResponse("PRESS de-compression results cleared.");
}

// Handle dump PRESS compression results to
// ${TMP_FOLDER}/[roadnetName]/press_compression/[0..(n-1)].bin
void handleDumpPRESSCompressionResultsToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto pressCompressFolderName = config.tmpFolder + roadnetName + "/press_compression/";
  if (!fileExists(pressCompressFolderName.c_str()) && !createFolder(pressCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << pressCompressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(pressCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << pressCompressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < pressCompressedTrajectories.size(); ++i) {
    auto pressCompressedTrajectoryFolder = pressCompressFolderName + std::to_string(i) + "/";
    if (!createFolder(pressCompressedTrajectoryFolder)) {
      FILE_LOG(TLogLevel::lerror)
        << "Failed to create press compressed folder: "
        << pressCompressedTrajectoryFolder;
      response = errorResponse("Failed to create press compressed folder.");
      return;
    }
    auto spatialName = pressCompressedTrajectoryFolder + "spatial.bin";
    FileWriter spatialWriter(spatialName.c_str(), true);
    auto temporalName = pressCompressedTrajectoryFolder + "temporal.bin";
    FileWriter temporalWriter(temporalName.c_str(), true);
    pressCompressedTrajectories.at(i).store(spatialWriter, temporalWriter);
  }
  response = successResponse(
    "Compressed PRESS trajectories are dumped to " + pressCompressFolderName + "."
  );
}

// Handle dump PRESS de-compression results to
// ${TMP_FOLDER}/[roadnetName]/press_decompression/[0..(n-1)].bin
void handleDumpPRESSDeCompressionResultsToBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto pressDeCompressFolderName = config.tmpFolder + roadnetName + "/press_decompression/";
  if (!fileExists(pressDeCompressFolderName.c_str()) && !createFolder(pressDeCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to create storage folder: " << pressDeCompressFolderName;
    response = errorResponse("Failed to create storage folder.");
    return;
  }
  if (!clearDirectory(pressDeCompressFolderName)) {
    FILE_LOG(TLogLevel::lerror) << "Failed to clear storage folder: " << pressDeCompressFolderName;
    response = errorResponse("Failed to clear storage folder.");
    return;
  }
  for (int i = 0; i < pressDeCompressedTrajectories.size(); ++i) {
    auto pressDeCompressedTrajectoryFolder = pressDeCompressFolderName + std::to_string(i) + "/";
    if (!createFolder(pressDeCompressedTrajectoryFolder)) {
      FILE_LOG(TLogLevel::lerror)
        << "Failed to create press de-compressed folder: "
        << pressDeCompressedTrajectoryFolder;
      response = errorResponse("Failed to create press de-compressed folder.");
      return;
    }
    auto spatialName = pressDeCompressedTrajectoryFolder + "spatial.bin";
    FileWriter spatialWriter(spatialName.c_str(), true);
    auto temporalName = pressDeCompressedTrajectoryFolder + "temporal.bin";
    FileWriter temporalWriter(temporalName.c_str(), true);
    pressDeCompressedTrajectories.at(i).store(spatialWriter, temporalWriter);
  }
  response = successResponse(
    "De-compressed PRESS trajectories are dumped to " + pressDeCompressFolderName + "."
  );
}

// Handle load PRESS compression results from
// ${TMP_FOLDER}/[folder]/press_compression/[0..(n-1)].bin
void handleLoadPRESSCompressionResultsFromBinary(
  picojson::value& requestJson,
  std::string& response
) {
  if (!roadnetReady) {
    response = errorResponse(
      "Roadnet is not ready or roadnet mismatch with PRESS compression results."
    );
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  auto folderName = config.tmpFolder + roadnetName + "/press_compression/";
  std::vector<std::string> files;
  if (!listDirectory(folderName, files)) {
    FILE_LOG(TLogLevel::lerror) << "Fail to list PRESS compression folder: " << folderName;
    response = errorResponse("Fail to list PRESS compression folder.");
    return;
  }
  pressCompressedTrajectories.clear();
  for (auto& file: files) {
    FileReader spatialReader((folderName + file + "/spatial.bin").c_str(), true);
    FileReader temporalReader((folderName + file + "/temporal.bin").c_str(), true);
    pressCompressedTrajectories.emplace_back(
      PRESSCompressedTrajectory(spatialReader, temporalReader)
    );
  }
  response = successResponse("Compressed PRESS trajectories are loaded from " + folderName + ".");
}

/**
 * WhereAt query on original PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       Time: double   // Queries timestamp
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     { X: double, Y: double },  // The position, (0, 0) means not found
 *     ...
 *   ]
 * }
 */
void handleWhereAtOnPRESSTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<Point2D> result;
  Point2D singleResult;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    singleResult.setPosition(0, 0);
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressTrajectories.size()) {
        throw "Index out of boundary";
      }
      auto timeStamp = queries[i].get("Time").get<double>();
      queryProcessor.whereAt(roadnet, pressTrajectories.at(idx), timeStamp, singleResult);
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
        << "Failed to query WhereAt on original PRESS trajectory: "
        << ex;
    }
    result.emplace_back(singleResult);
  }
  std::string queryResult = vecToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * WhenAt query on original PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       X: double      // position
 *       Y: double
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     double           // the timestamp when the vehicle at the position, -1 means not found
 *     ...
 *   ]
 * }
 */
void handleWhenAtOnPRESSTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<double> result;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    double singleResult = -1;
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressTrajectories.size()) {
        throw "Index out of boundary";
      }
      double x = queries[i].get("X").get<double>();
      double y = queries[i].get("Y").get<double>();
      Point2D position(x, y);
      singleResult = queryProcessor.whenAt(roadnet, pressTrajectories.at(idx), position);
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
        << "Failed to query WhenAt on original PRESS trajectory: "
        << ex;
    }
    result.push_back(singleResult);
  }
  std::string queryResult = vecPrimitiveToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * Range query on original PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       T1: double     // lower time
 *       T2: double     // upper time
 *       X1: double     // MBR lower bound
 *       Y1: double
 *       X2: double     // MBR upper bound
 *       Y2: double
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     int              // whether trajectory passes MBR within the time range, 0: false, 1: true
 *     ...
 *   ]
 * }
 */
void handleRangeOnPRESSTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<bool> result;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    bool singleResult = false;
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressTrajectories.size()) {
        throw "Index out of boundary";
      }
      double x1 = queries[i].get("X1").get<double>();
      double y1 = queries[i].get("Y1").get<double>();
      double x2 = queries[i].get("X2").get<double>();
      double y2 = queries[i].get("Y2").get<double>();
      double t1 = queries[i].get("T1").get<double>();
      double t2 = queries[i].get("T2").get<double>();
      singleResult = queryProcessor.range(
        roadnet,
        pressTrajectories.at(idx),
        t1,
        t2,
        Point2D(x1, y1),
        Point2D(x2, y2)
      );
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
      << "Failed to query Range on original PRESS trajectory: "
      << ex;
    }
    result.push_back(singleResult);
  }
  std::string queryResult = vecPrimitiveToJSONString(result);
  response = successResponseWithData(queryResult);
}

/**
 * WhereAt query on compressed PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       Time: double   // Queries timestamp
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     { X: double, Y: double },  // The position, (0, 0) means not found
 *     ...
 *   ]
 * }
 */
void handleWhereAtOnPRESSCompressedTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (!auxiliaryReady) {
    response = errorResponse("Auxiliary is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<Point2D> result;
  Point2D singleResult;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    singleResult.setPosition(0, 0);
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressCompressedTrajectories.size()) {
        throw "Index out of boundary";
      }
      auto timeStamp = queries[i].get("Time").get<double>();
      queryProcessor.whereAt(
        roadnet,
        spTable,
        huffman,
        acAutomaton,
        auxiliary,
        pressCompressedTrajectories.at(idx),
        timeStamp,
        singleResult
      );
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
      << "Failed to query WhereAt on compressed PRESS trajectory: "
      << ex;
    }
    result.emplace_back(singleResult);
  }
  std::string queryResult = vecToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * WhenAt query on compressed PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       X: double      // position
 *       Y: double
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     double           // the timestamp when the vehicle at the position, -1 means not found
 *     ...
 *   ]
 * }
 */
void handleWhenAtOnPRESSCompressedTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (!auxiliaryReady) {
    response = errorResponse("Auxiliary is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<double> result;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    double singleResult = -1;
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressCompressedTrajectories.size()) {
        throw "Index out of boundary";
      }
      double x = queries[i].get("X").get<double>();
      double y = queries[i].get("Y").get<double>();
      Point2D position(x, y);
      singleResult = queryProcessor.whenAt(
        roadnet,
        spTable,
        huffman,
        acAutomaton,
        auxiliary,
        pressCompressedTrajectories.at(idx),
        position
      );
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
      << "Failed to query WhenAt on compressed PRESS trajectory: "
      << ex;
    }
    result.push_back(singleResult);
  }
  std::string queryResult = vecPrimitiveToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * Range query on compressed PRESS trajectories.
 * Query payload:
 * {
 *   Cmd ...
 *   Query: [
 *     {
 *       Idx: int       // trajectory index
 *       T1: double     // lower time
 *       T2: double     // upper time
 *       X1: double     // MBR lower bound
 *       Y1: double
 *       X2: double     // MBR upper bound
 *       Y2: double
 *     },
 *     ...
 *   ]
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     int              // whether trajectory passes MBR within the time range, 0: false, 1: true
 *     ...
 *   ]
 * }
 */
void handleRangeOnPRESSCompressedTrajectory(picojson::value& requestJson, std::string& response) {
  if (!roadnetReady) {
    response = errorResponse("Roadnet is not ready.");
    return;
  }
  if (!spTableReady) {
    response = errorResponse("SP table is not ready.");
    return;
  }
  if (!acAutomatonReady) {
    response = errorResponse("AC automaton is not ready.");
    return;
  }
  if (!huffmanReady) {
    response = errorResponse("Huffman tree is not ready.");
    return;
  }
  if (!auxiliaryReady) {
    response = errorResponse("Auxiliary is not ready.");
    return;
  }
  auto& queries = requestJson.get("Query").get<picojson::value::array>();
  auto len = queries.size();
  std::vector<bool> result;
  QueryProcessor queryProcessor;
  for (auto i = 0; i < len; ++i) {
    bool singleResult = false;
    try {
      auto idx = (int)queries[i].get("Idx").get<double>();
      if (idx < 0 || idx >= pressCompressedTrajectories.size()) {
        throw "Index out of boundary";
      }
      double x1 = queries[i].get("X1").get<double>();
      double y1 = queries[i].get("Y1").get<double>();
      double x2 = queries[i].get("X2").get<double>();
      double y2 = queries[i].get("Y2").get<double>();
      double t1 = queries[i].get("T1").get<double>();
      double t2 = queries[i].get("T2").get<double>();
      singleResult = queryProcessor.range(
        roadnet,
        spTable,
        huffman,
        acAutomaton,
        auxiliary,
        pressCompressedTrajectories.at(idx),
        t1,
        t2,
        Point2D(x1, y1),
        Point2D(x2, y2)
      );
    } catch (const char* ex) {
      FILE_LOG(TLogLevel::lwarning)
      << "Failed to query Range on compressed PRESS trajectory: "
      << ex;
    }
    result.push_back(singleResult);
  }
  std::string queryResult = vecPrimitiveToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * Get all roadnet reader types.
 * Query payload:
 * {
 *   Cmd ...
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     SEATTLE_SAMPLE_ROADNET,
 *     GraphReaderType2,
 *     ...
 *   ]
 * }
 */
void handleGetRoadnetReaderTypes(picojson::value& requestJson, std::string& response) {
  std::vector<std::string> result;
  for (auto& graphReaderType: GraphReaderTypeStrings) {
    result.push_back(graphReaderType);
  }
  std::string queryResult = vecStringToJSONString(result);
  response = successResponseWithData(queryResult);
}
/**
 * Get all GPS reader types.
 * Query payload:
 * {
 *   Cmd ...
 * }
 * Response:
 * {
 *   Success ...
 *   Message ...
 *   Data: [
 *     SEATTLE_SAMPLE_GPS,
 *     GraphReaderType2,
 *     ...
 *   ]
 * }
 */
void handleGetGPSReaderTypes(picojson::value& requestJson, std::string& response) {
  std::vector<std::string> result;
  for (auto& gpsReaderType: GPSTrajectoryReaderTypeStrings) {
    result.push_back(gpsReaderType);
  }
  std::string queryResult = vecStringToJSONString(result);
  response = successResponseWithData(queryResult);
}

struct ReqRespHelper {
  std::string inPath;
  std::string outPath;
  std::string data;
  std::string response;

  ReqRespHelper(char* inPath, const char* outPath): inPath(inPath), outPath(outPath) { }

  std::string& readNext() {
    std::ifstream reqStream;
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
    std::ofstream respStream;
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
      FILE_LOG(TLogLevel::linfo) << "Received request << " << cmd;
      // Handles each type of requests.
      if (cmd == "ReadRoadnetFromDataSource") {
        handleReadRoadnetFromDataSource(requestJson, response);
      } else if (cmd == "GetRoadnet") {
        handleGetRoadnet(requestJson, response);
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
      } else if (cmd == "ShowGPSTrajectory") {
        handleShowGPSTrajectory(requestJson, response);
      } else if (cmd == "GetGPSTrajectoriesSampleRate") {
        handleGetGPSTrajectoriesSampleRate(requestJson, response);
      } else if (cmd == "LoadMapMatchedTrajectoriesFromBinary") {
        handleLoadMapMatchedTrajectoriesFromBinary(requestJson, response);
      } else if (cmd == "ReformatTrajectories") {
        handleReformatTrajectories(requestJson, response);
      } else if (cmd == "ClearPRESSTrajectories") {
        handleClearPRESSTrajectories(requestJson, response);
      } else if (cmd == "DumpPRESSTrajectoriesToBinary") {
        handleDumpPRESSTrajectoriesToBinary(requestJson, response);
      } else if (cmd == "LoadPRESSTrajectoriesFromBinary") {
        handleLoadPRESSTrajectoriesFromBinary(requestJson, response);
      } else if (cmd == "ShowPRESSTrajectory") {
        handleShowPRESSTrajectory(requestJson, response);
      } else if (cmd == "TrainACAutomatonHuffmanTreeAndBuildAuxiliary") {
        handleTrainACAutomatonHuffmanTreeAndBuildAuxiliary(requestJson, response);
      } else if (cmd == "DumpACAutomatonHuffmanTreeAndAuxiliaryToBinary") {
        handleDumpACAutomatonHuffmanTreeAndAuxiliaryToBinary(requestJson, response);
      } else if (cmd == "LoadACAutomatonHuffmanTreeAndAuxiliaryFromBinary") {
        handleLoadACAutomatonHuffmanTreeAndAuxiliaryFromBinary(requestJson, response);
      } else if (cmd == "SPCompression") {
        handleSPCompression(requestJson, response);
      } else if (cmd == "ClearSPCompressionResults") {
        handleClearSPCompressionResults(requestJson, response);
      } else if (cmd == "DumpSPCompressionResultsToBinary") {
        handleDumpSPCompressionResultsToBinary(requestJson, response);
      } else if (cmd == "LoadSPCompressionResultsFromBinary") {
        handleLoadSPCompressionResultsFromBinary(requestJson, response);
      } else if (cmd == "FSTCompression") {
        handleFSTCompression(requestJson, response);
      } else if (cmd == "ClearFSTCompressionResults") {
        handleClearFSTCompressionResults(requestJson, response);
      } else if (cmd == "DumpFSTCompressionResultsToBinary") {
        handleDumpFSTCompressionResultsToBinary(requestJson, response);
      } else if (cmd == "LoadFSTCompressionResultsFromBinary") {
        handleLoadFSTCompressionResultsFromBinary(requestJson, response);
      } else if (cmd == "BTCCompression") {
        handleBTCCompression(requestJson, response);
      } else if (cmd == "ClearBTCCompressionResults") {
        handleClearBTCCompressionResults(requestJson, response);
      } else if (cmd == "DumpBTCCompressionResultsToBinary") {
        handleDumpBTCCompressionResultsToBinary(requestJson, response);
      } else if (cmd == "LoadBTCCompressionResultsFromBinary") {
        handleLoadBTCCompressionResultsFromBinary(requestJson, response);
      } else if (cmd == "PRESSCompression") {
        handlePRESSCompression(requestJson, response);
      } else if (cmd == "PRESSDeCompression") {
        handlePRESSDeCompression(requestJson, response);
      } else if (cmd == "ClearPRESSCompressionResults") {
        handleClearPRESSCompressionResults(requestJson, response);
      } else if (cmd == "DumpPRESSCompressionResultsToBinary") {
        handleDumpPRESSCompressionResultsToBinary(requestJson, response);
      } else if (cmd == "ClearPRESSDeCompressionResults") {
        handleClearPRESSDeCompressionResults(requestJson, response);
      } else if (cmd == "DumpPRESSDeCompressionResultsToBinary") {
        handleDumpPRESSDeCompressionResultsToBinary(requestJson, response);
      } else if (cmd == "LoadPRESSCompressionResultsFromBinary") {
        handleLoadPRESSCompressionResultsFromBinary(requestJson, response);
      } else if (cmd == "WhereAtOnPRESSTrajectory") {
        handleWhereAtOnPRESSTrajectory(requestJson, response);
      } else if (cmd == "WhenAtOnPRESSTrajectory") {
        handleWhenAtOnPRESSTrajectory(requestJson, response);
      } else if (cmd == "RangeOnPRESSTrajectory") {
        handleRangeOnPRESSTrajectory(requestJson, response);
      } else if (cmd == "WhereAtOnPRESSCompressedTrajectory") {
        handleWhereAtOnPRESSCompressedTrajectory(requestJson, response);
      } else if (cmd == "WhenAtOnPRESSCompressedTrajectory") {
        handleWhenAtOnPRESSCompressedTrajectory(requestJson, response);
      } else if (cmd == "RangeOnPRESSCompressedTrajectory") {
        handleRangeOnPRESSCompressedTrajectory(requestJson, response);
      } else if (cmd == "GetRoadnetReaderTypes") {
        handleGetRoadnetReaderTypes(requestJson, response);
      } else if (cmd == "GetGPSReaderTypes") {
        handleGetGPSReaderTypes(requestJson, response);
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
    } catch (const char* ex) {
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
  daemonize();
  // Register signal handlers.
  registerSignalHandler();
  // Open communication channel.
  ReqRespHelper reqRespHelper(argv[2], argv[3]);
  FILE_LOG(TLogLevel::linfo) << "PRESS core started.";
  // Handle requests.
  while (true) {
    auto& request = reqRespHelper.readNext();
    reqRespHelper.handleRequest(request);
    FILE_LOG(TLogLevel::linfo) << "Response sent";
  }

  return EXIT_SUCCESS;
}
