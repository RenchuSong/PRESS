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
  int nodeNumber;
  std::vector<Node> nodeList;
  std::unordered_map<int, int> nodeId2Index;
  int edgeNumber;
  std::vector<Edge> edgeList;
  std::unordered_map<int, int> edgeId2Index;

public:
  Graph(FileReader& nodeReader, FileReader& edgeReader, FileReader& geoReader);
  const Node& getNodeById(int id);
  const Node& getNodeByIndex(size_t index);
  const Edge& getEdgeById(int id);
  const Edge& getEdgeByIndex(size_t index);
  void print();
  ~Graph();
};

#endif /* graph_hpp */
