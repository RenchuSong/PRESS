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
    if (responseJson.get("success").get<bool>()) {
      if (responseJson.contains("message")) {
        std::cout << responseJson.get("message").get<std::string>() << std::endl;
      }
      if (responseJson.contains("data")) {
        std::cout << responseJson.get("data") << std::endl;
      }
    } else {
      std::cout << "Failed " << responseJson.get("message").get<std::string>() << std::endl;
    }
  }
};

int main(int argc, char** argv) {
  // Open communication channel.
  ReqRespHelper reqRespHelper(argv[1], argv[2]);

  // Read roadnet from file.
  reqRespHelper.writeNext("{\"Cmd\":\"ReadRoadnetFromDataSource\", \"Folder\":\"WA_roadnetwork_and_single_trajectory\", \"GraphReaderType\":\"SEATTLE_SAMPLE_ROADNET\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump roadnet to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpRoadnetToBinary\", \"Folder\":\"WA_roadnetwork\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Build grid index.
  reqRespHelper.writeNext("{\"Cmd\":\"BuildGridIndex\", \"CellWidth\":50, \"CellHeight\":50}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Dump grid index to binary.
  reqRespHelper.writeNext("{\"Cmd\":\"DumpGridIndexToBinary\", \"Folder\":\"WA_roadnetwork\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());

  // Load roadnet from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadRoadnetFromBinary\", \"Folder\":\"WA_roadnetwork\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());
  // Load grid index from binary.
  reqRespHelper.writeNext("{\"Cmd\":\"LoadGridIndexFromBinary\", \"Folder\":\"WA_roadnetwork\"}");
  reqRespHelper.explainResponse(reqRespHelper.readNext());

  return EXIT_SUCCESS;
}
