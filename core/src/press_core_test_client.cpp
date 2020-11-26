//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 11/23/20.
//  Copyright © 2018 Renc. All rights reserved.
//

#include <iostream>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "io/file_reader.hpp"
#include "third_party/picojson.h"
#include "topology/trajectory.hpp"
#include "util/utility.hpp"
#include "util/timer.hpp"

struct WhereAtQuery {
  int idx;
  double timeStamp;
  WhereAtQuery(int idx, double timeStamp): idx(idx), timeStamp(timeStamp) { }
  std::string toJSONString() const {
    return std::string("{\"Idx\":") + std::to_string(idx) + ",\"Time\":" + std::to_string(timeStamp) + "}";
  }
};

struct WhenAtQuery {
  int idx;
  double x;
  double y;
  WhenAtQuery(int idx, double x, double y): idx(idx), x(x), y(y) { }
  std::string toJSONString() const {
    return std::string("{\"Idx\":") + std::to_string(idx) + ",\"X\":" + std::to_string(x) + ",\"Y\":" + std::to_string(y) + "}";
  }
};

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
      std::cout << "Fail to open request stream.";
      exit(EXIT_FAILURE);
    }
    std::getline(reqStream, data);
    reqStream.close();
    return data;
  }

  void writeNext(std::string data) {
    respStream.open(outPath, std::ofstream::out);
    if (!respStream.is_open()) {
      std::cout << "Fail to open response stream.";
      exit(EXIT_FAILURE);
    }
    respStream << data;
    respStream.close();
  }

  picojson::value explainResponse(std::string& response) {
    // Parse response.
    picojson::value responseJson;
    std::string err = picojson::parse(responseJson, response);
    if (!err.empty()) {
      std::cout << "Failed to parse response (" << response << ")";
      return responseJson;
    }
    if (responseJson.get("Success").get<bool>()) {
      if (responseJson.contains("Message")) {
        std::cout << responseJson.get("Message").get<std::string>() << std::endl;
      }
      if (responseJson.contains("Data")) {
        std::cout << responseJson.get("Data") << std::endl;
      }
    } else {
      std::cout << "Failed " << responseJson.get("Message").get<std::string>() << std::endl;
    }
    return responseJson;
  }
};

std::string queryPayload(std::string cmd, std::string query) {
  return "{\"Cmd\":\"" + cmd + "\", \"Query\":" + query + "}";
}

int main(int argc, char** argv) {
//  std::vector<std::string> files;
//  listDirectory("/Users/songrenchu/Develop/PRESS/core/tmp/WA_roadnetwork_and_single_trajectory/press_trajectories/", files);
//  for (auto& file: files) std::cout << file << std::endl;
//  std::cout << clearDirectory("/Users/songrenchu/Develop/PRESS/core/tmp/WA_roadnetwork_and_single_trajectory/press_trajectories/") << std::endl;
//  return 0;

  // Open communication channel.
  ReqRespHelper reqRespHelper(argv[1], argv[2]);
  
  Timer timer;

//  // Read roadnet from file.
//  reqRespHelper.writeNext("{\"Cmd\":\"ReadRoadnetFromDataSource\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\", \"GraphReaderType\":\"SEATTLE_SAMPLE_ROADNET\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump roadnet to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpRoadnetToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Build grid index.
//  reqRespHelper.writeNext("{\"Cmd\":\"BuildGridIndex\", \"CellWidth\":50, \"CellHeight\":50}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump grid index to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpGridIndexToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Build SP table.
//  reqRespHelper.writeNext("{\"Cmd\":\"BuildSPTable\", \"MaxDist\":4000}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump SP table to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpSPTableToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Add GPS trajectory and map match.
//  reqRespHelper.writeNext("{\"Cmd\":\"AddGPSTrajectoryAndMapMatch\", \"FileName\":\"WA_roadnetwork_and_single_trajectory/gps_data.txt\", \"SigmaZ\":4.07, \"MaxGPSBias\":50, \"MaxDistDifference\":2000, \"GPSTrajectoryReaderType\":\"SEATTLE_SAMPLE_GPS\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump GPS trajectories to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpGPSTrajectoriesToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump map matched trajectories to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpMapMatchedTrajectoriesToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Reformat GPS trajectories and map matched trajectories as PRESS trajectories.
//  reqRespHelper.writeNext("{\"Cmd\":\"ReformatTrajectories\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump PRESS trajectories to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpPRESSTrajectoriesToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Train
//  reqRespHelper.writeNext("{\"Cmd\":\"TrainACAutomatonHuffmanTreeAndBuildAuxiliary\", \"Theta\":3}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump AC automaton, huffman tree and auxiliary to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpACAutomatonHuffmanTreeAndAuxiliaryToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // SP compression
//  reqRespHelper.writeNext("{\"Cmd\":\"SPCompression\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump SP compression results to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpSPCompressionResultsToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // FST compression
//  reqRespHelper.writeNext("{\"Cmd\":\"FSTCompression\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump FST compression results to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpFSTCompressionResultsToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // BTC compression
//  reqRespHelper.writeNext("{\"Cmd\":\"BTCCompression\", \"TSND\":10, \"NSTD\":10}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump BTC compression results to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpBTCCompressionResultsToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // PRESS compression
//  reqRespHelper.writeNext("{\"Cmd\":\"PRESSCompression\", \"TSND\":10, \"NSTD\":10}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump PRESS compression results to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpPRESSCompressionResultsToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // PRESS de-compression
//  reqRespHelper.writeNext("{\"Cmd\":\"PRESSDeCompression\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());
//  // Dump PRESS de-compression results to binary.
//  reqRespHelper.writeNext("{\"Cmd\":\"DumpPRESSDeCompressionResultsToBinary\"}");
//  reqRespHelper.explainResponse(reqRespHelper.readNext());

  // Load roadnet from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadRoadnetFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load grid index from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadGridIndexFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load SP table from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadSPTableFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load GPS trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadGPSTrajectoriesFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load map matched trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadMapMatchedTrajectoriesFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load PRESS trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadPRESSTrajectoriesFromBinary\"}");
  auto response = reqRespHelper.explainResponse(reqRespHelper.readNext());
  std::string message = response.get("Message").get<std::string>();
  std::string pressPath = message.substr(message.find('/'));
  pressPath = pressPath.substr(0, pressPath.length() - 1);
  // Load AC automaton, huffman tree and auxiliary from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadACAutomatonHuffmanTreeAndAuxiliaryFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load SP compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadSPCompressionResultsFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load FST compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadFSTCompressionResultsFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load BTC compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadBTCCompressionResultsFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load PRESS compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadPRESSCompressionResultsFromBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());

  // Load the PRESS trajectories and do queries.
  std::vector<std::string> files;
  listDirectory(pressPath, files);
  std::vector<PRESSTrajectory> pressTrajectories;
  for (auto& file: files) {
    FileReader spatialReader((pressPath + file + "/spatial.bin").c_str(), true);
    FileReader temporalReader((pressPath + file + "/temporal.bin").c_str(), true);
    pressTrajectories.emplace_back(PRESSTrajectory(spatialReader, temporalReader));
  }
  srand((unsigned)time(NULL));

  // 5000 random WhereAt queries.
  std::vector<WhereAtQuery> whereAtQueries;
  for (int i = 0; i < 5000; ++i) {
    int idx = rand() % pressTrajectories.size();
    double minT = pressTrajectories.at(idx).getTemporalComponent().at(0).t;
    double maxT = pressTrajectories.at(idx).getTemporalComponent().at(pressTrajectories.at(idx).getTemporalLength() - 1).t;
    double timeStamp = (rand() % 10000) / 10000.0 * (maxT - minT) + minT;
    whereAtQueries.emplace_back(WhereAtQuery(idx, timeStamp));
  }
  timer.reset();
  timer.start();
  reqRespHelper.writeNext(queryPayload("WhereAtOnPRESSTrajectory", vecToJSONString(whereAtQueries)));
  auto whereAtResult = reqRespHelper.explainResponse(reqRespHelper.readNext()).get("Data").get<picojson::value::array>();
  std::cout << "Original whereAt takes (millisesonds): " << timer.getMilliSeconds() << std::endl;
  timer.pause();
  // 5000 random WhenAt queries.
  std::vector<WhenAtQuery> whenAtQueries;
  auto len = whereAtResult.size();
  for (int i = 0; i < len; ++i) {
    int idx = whereAtQueries.at(i).idx;
    double x = whereAtResult.at(i).get("X").get<double>();
    double y = whereAtResult.at(i).get("Y").get<double>();
    whenAtQueries.emplace_back(WhenAtQuery(idx, x, y));
  }
  timer.reset();
  timer.start();
  reqRespHelper.writeNext(queryPayload("WhenAtOnPRESSTrajectory", vecToJSONString(whenAtQueries)));
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  std::cout << "Original whenAt takes (millisesonds): " << timer.getMilliSeconds() << std::endl;
  timer.pause();

  return EXIT_SUCCESS;
}
