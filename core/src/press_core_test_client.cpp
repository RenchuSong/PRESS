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

#include "third_party/picojson.h"
#include "util/utility.hpp"

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

  void explainResponse(std::string& response) {
    // Parse response.
    picojson::value responseJson;
    std::string err = picojson::parse(responseJson, response);
    if (!err.empty()) {
      std::cout << "Failed to parse response (" << response << ")";
      return;
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
  }
};

int main(int argc, char** argv) {
//  std::vector<std::string> files;
//  listDirectory("/Users/songrenchu/Develop/PRESS/core/tmp/WA_roadnetwork_and_single_trajectory/press_trajectories/", files);
//  for (auto& file: files) std::cout << file << std::endl;
//  std::cout << clearDirectory("/Users/songrenchu/Develop/PRESS/core/tmp/WA_roadnetwork_and_single_trajectory/press_trajectories/") << std::endl;
//  return 0;

  // Open communication channel.
  ReqRespHelper reqRespHelper(argv[1], argv[2]);

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

  // Load roadnet from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadRoadnetFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load grid index from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadGridIndexFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load SP table from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadSPTableFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load GPS trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadGPSTrajectoriesFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load map matched trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadMapMatchedTrajectoriesFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load PRESS trajectories from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadPRESSTrajectoriesFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load AC automaton, huffman tree and auxiliary from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadACAutomatonHuffmanTreeAndAuxiliaryFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load SP compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadSPCompressionResultsFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // FST compression
  reqRespHelper.writeNext("{\"Cmd\":\"FSTCompression\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump FST compression results to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpFSTCompressionResultsToBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load FST compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadFSTCompressionResultsFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // BTC compression
  reqRespHelper.writeNext("{\"Cmd\":\"BTCCompression\", \"TSND\":10, \"NSTD\":10}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump BTC compression results to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpBTCCompressionResultsToBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load BTC compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadBTCCompressionResultsFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // PRESS compression
  reqRespHelper.writeNext("{\"Cmd\":\"PRESSCompression\", \"TSND\":10, \"NSTD\":10}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump PRESS compression results to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpPRESSCompressionResultsToBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load PRESS compression results from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadPRESSCompressionResultsFromBinary\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // PRESS de-compression
  reqRespHelper.writeNext("{\"Cmd\":\"PRESSDeCompression\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump PRESS de-compression results to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpPRESSDeCompressionResultsToBinary\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());

  return EXIT_SUCCESS;
}
