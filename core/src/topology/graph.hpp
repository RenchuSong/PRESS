//
//  graph.hpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef graph_hpp
#define graph_hpp

#include <iostream>
#include <vector>
#include <unordered_map>

#include "../io/file_reader.hpp"
#include "auxiliaries.hpp"

// Node in graph represents one intersection.
class Node {
private:
  Point2D position;
  size_t edgeNumber;
  std::vector<int> edges;

public:
  Node(double x, double y);
  void addEdge(int eid);
  size_t getEdgeNumber();
  int getEdgeId(size_t index);
  void print();
  ~Node();
};

// Edge in graph represents one directional street.
// Each bi-direactional street is represented as two edges.
class Edge {
private:
  int sourceId;
  int targetId;
  size_t geoSize;
  std::vector<Point2D> shape;
  double distance;

public:
  Edge(int srcId, int tgtId);
  size_t addPosition(double x, double y);
  int getSourceId();
  int getTargetId();
  size_t getGeoSize();
  const std::vector<Point2D>& getShape();
  double getDistance();
  void print();
  ~Edge();
};

// Graph represents the road network.
class Graph {
private:
  size_t nodeNumber;
  std::vector<Node> nodeList;
  std::unordered_map<int, int> nodeId2Index;
  size_t edgeNumber;
  std::vector<Edge> edgeList;
  std::unordered_map<int, int> edgeId2Index;

public:
  Graph(const FileReader& nodeReader, const FileReader& edgeReader, const FileReader& geoReader);
  size_t getNodeNumber();
  Node& getNodeById(int id);
  Node& getNodeByIndex(size_t index);
  const std::vector<Node>& getNodeList();
  size_t getEdgeNumber();
  Edge& getEdgeById(int id);
  Edge& getEdgeByIndex(size_t index);
  const std::vector<Edge>& getEdgeList();
  void print();
  ~Graph();
};

#endif /* graph_hpp */
