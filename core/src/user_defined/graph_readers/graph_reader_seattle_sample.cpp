//
//  graph_reader_seattle_sample.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <algorithm>

#include "graph_reader_seattle_sample.hpp"

#include "../../io/file_reader.hpp"
#include "../../util/helper.hpp"

void GraphReaderSeattleSample::readGraphImpl(const std::string& fileName, Graph& graph) {
  // Data structure holding the graph nodes and edges.
  std::vector<Node> nodeList;
  std::vector<Edge> edgeList;

  FileReader reader(fileName.c_str(), false);
  
  // Skip the header line.
  do {} while (reader.nextChar() != '\n');
  
  // Hold the map from node id in the file to the internal node index.
  std::unordered_map<long long, int> nodeIdToIndex;

  int nodeNumber = 0;
  int edgeNumber = 0;
  long long edgeId;
  while ((edgeId = reader.nextLong()) != EOF) {
    long long srcRawId = reader.nextLong();
    long long tgtRawId = reader.nextLong();
    bool twoWay = (reader.nextInt() == 1);
    // speed, no use for now
    reader.nextDouble();
    int shapeSize = reader.nextInt();
    // Go to the '('
    do {} while (reader.nextChar() != '(');
    std::vector<Point2D> shape;
    for (auto i = 0; i < shapeSize; ++i) {
      // the comma
      if (i > 0) {
        reader.nextChar();
      }
      double lng = reader.nextDouble();
      double lat = reader.nextDouble();
      GPSPoint gps(lat, lng);
      shape.emplace_back(gps2Point2D(gps));
    }
    // Skip ')'
    reader.nextChar();
    // The two nodes.
    int srcId;
    if (nodeIdToIndex.find(srcRawId) != nodeIdToIndex.end()) {
      srcId = nodeIdToIndex.at(srcRawId);
    } else {
      // Create the new node.
      nodeList.emplace_back(Node(shape.at(0)));
      srcId = nodeNumber++;
      nodeIdToIndex[srcRawId] = srcId;
    }
    int tgtId;
    if (nodeIdToIndex.find(tgtRawId) != nodeIdToIndex.end()) {
      tgtId = nodeIdToIndex.at(tgtRawId);
    } else {
      // Create the new node.
      nodeList.emplace_back(Node(shape.at(shape.size() - 1)));
      tgtId = nodeNumber++;
      nodeIdToIndex[tgtRawId] = tgtId;
    }
    // The forward edge.
    Edge forward(srcId, tgtId);
    for (auto& point: shape) {
      forward.addPosition(point);
    }
    edgeList.emplace_back(forward);
    graph.eid2OriginalId[edgeNumber] = edgeId;
    graph.fromTo[edgeNumber] = 1;
    nodeList.at(srcId).addEdge(edgeNumber++);
    // The backward edge if two way.
    if (twoWay) {
      Edge backward(tgtId, srcId);
      std::reverse(shape.begin(), shape.end());
      for (auto& point: shape) {
        backward.addPosition(point);
      }
      edgeList.emplace_back(backward);
      graph.eid2OriginalId[edgeNumber] = edgeId;
      graph.fromTo[edgeNumber] = 0;
      nodeList.at(tgtId).addEdge(edgeNumber++);
    }
  }
  
  graph.setGraph(nodeList, edgeList);
}
