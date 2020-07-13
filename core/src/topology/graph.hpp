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
#include "../io/file_writer.hpp"
#include "auxiliaries.hpp"

// Node in graph represents one intersection.
class Node {
private:
  Point2D position;
  size_t edgeNumber;
  std::vector<int> edges;

public:
  Node(double x, double y);
  Node(Point2D& point);
  void addEdge(int eid);
  size_t getEdgeNumber();
  const std::vector<int>& getEdgeList();
  int getEdgeId(size_t index);
  Point2D& getPosition();
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
  size_t addPosition(Point2D& point);
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
  // std::unordered_map<int, int> nodeId2Index;
  size_t edgeNumber;
  std::vector<Edge> edgeList;
  // std::unordered_map<int, int> edgeId2Index;

public:
  Graph();
  // Construct graph from given node list and edge list.
  Graph(std::vector<Node>& nodeList, std::vector<Edge>& edgeList);
  // Load graph from ACM SIGSPATLAL CUP 2012 format.
  Graph(FileReader& nodeReader, FileReader& edgeReader, FileReader& geoReader);
  // Load graph from file.
  Graph(FileReader& graphReader);
  size_t getNodeNumber();
  Node& getNode(size_t index);
  const std::vector<Node>& getNodeList();
  size_t getEdgeNumber();
  Edge& getEdge(size_t index);
  const std::vector<Edge>& getEdgeList();
  // Store the graph into a file.
  void store(FileWriter& graphWriter);
  void print();
  ~Graph();
};

#endif /* graph_hpp */
