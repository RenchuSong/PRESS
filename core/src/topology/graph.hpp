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
#include <sstream>
#include <vector>
#include <unordered_map>

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "geospatial_structures.hpp"

// Node in graph represents one intersection.
class Node {
private:
  Point2D position;
  size_t edgeNumber;
  std::vector<int> edges;

public:
  Node(double x, double y);
  Node(const Point2D& point);
  void addEdge(int eid);
  size_t getEdgeNumber() const;
  const std::vector<int>& getEdgeList() const;
  int getEdgeId(size_t index) const;
  const Point2D& getPosition() const;
  void print() const ;
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
  size_t addPosition(const Point2D& point);
  int getSourceId() const;
  int getTargetId() const;
  size_t getGeoSize() const;
  const std::vector<Point2D>& getShape() const;
  double getDistance() const;
  void print() const;
  void toJSON(std::stringstream& ss) const;
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
  // Load graph from file.
  Graph(FileReader& graphReader);
  // Construct graph from given node list and edge list.
  Graph(const std::vector<Node>& nodeList, const std::vector<Edge>& edgeList);
  // Set graph from given node list and edge list.
  void setGraph(const std::vector<Node>& nodeList, const std::vector<Edge>& edgeList);
  size_t getNodeNumber() const;
  const Node& getNode(size_t index) const;
  const std::vector<Node>& getNodeList() const;
  size_t getEdgeNumber() const;
  const Edge& getEdge(size_t index) const;
  const std::vector<Edge>& getEdgeList() const;
  // Load from a file.
  void load(FileReader& graphReader);
  // Store the graph into a file.
  void store(FileWriter& graphWriter);
  void print() const;
  void clear();
  void toJSON(std::stringstream& ss) const;
  ~Graph();
  
  // FIXME: hack for check MM result. Should be removed.
  std::unordered_map<int, long long> eid2OriginalId;
  std::unordered_map<int, int> fromTo;
};

#endif /* graph_hpp */
