//
//  graph.cpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>

#include "graph.hpp"
#include "../util/helper.hpp"

// Node constructor.
Node::Node(double x, double y): position(x, y), edgeNumber(0) { }

// Add one edge to the node edge list.
void Node::addEdge(int eid) {
  edgeNumber++;
  edges.push_back(eid);
}

// Get edge number.
size_t Node::getEdgeNumber() {
  return edgeNumber;
}

// Get edge id by index.
int Node::getEdgeId(size_t index) {
  assert (index < edgeNumber);
  return edges[index];
}

// Print the node for debug.
void Node::print() {
  std::cout << "position ";
  position.print();
  std::cout << std::endl;
  std::cout << "edge list: " << std::endl;
  for (auto eid : edges) {
    std::cout << eid << ", ";
  }
  std::cout << std::endl;
}

Node::~Node() { }

// Edge constructor.
Edge::Edge(int srcId, int tgtId): sourceId(srcId), targetId(tgtId), geoSize(0), distance(0) { }

// Add one position in the shape.
size_t Edge::addPosition(double x, double y) {
  geoSize++;
  Point2D point(x, y);
  if (shape.size() > 0) {
    distance += euclideanDistance(shape.back(), point);
  }
  shape.push_back(point);
  return geoSize;
}

// Get the source node id.
int Edge::getSourceId() {
  return sourceId;
}

// Get the target node id.
int Edge::getTargetId() {
  return targetId;
}

// Get the geo size.
size_t Edge::getGeoSize() {
  return geoSize;
}

// Get the shape of the edge.
const std::vector<Point2D>& Edge::getShape() {
  return shape;
}

// Get the distance of the edge.
double Edge::getDistance() {
  return distance;
}

// Print the edge for debug.
void Edge::print() {
  std::cout << "srcId: " << sourceId << std::endl;
  std::cout << "tgtId: " << targetId << std::endl;
  std::cout << "geoSize: " << geoSize << std::endl;
  std::cout << "shape: " << std::endl;
  for (auto pt : shape) {
    pt.print();
  }
  std::cout << std::endl;
  std::cout << "distance: " << distance << std::endl;
  std::cout << std::endl;
}

Edge::~Edge() { }


// Construct graph from the files.
Graph::Graph(const FileReader& nodeReader,
             const FileReader& edgeReader,
             const FileReader& geoReader) {
  // TODO: read graph from file.
}

// Get node number in the graph.
size_t Graph::getNodeNumber() {
  return nodeNumber;
}

// Get the node with given id.
const Node& Graph::getNodeById(int id) {
  return nodeList[nodeId2Index.at(id)];
}

// Get the node by index.
const Node& Graph::getNodeByIndex(size_t index) {
  return nodeList[index];
}

// Get the node list.
const std::vector<Node>& Graph::getNodeList() {
  return nodeList;
}

// Get edge number in the graph.
size_t Graph::getEdgeNumber() {
  return edgeNumber;
}

// Get the edge with given id.
const Edge& Graph::getEdgeById(int id) {
  return edgeList[edgeId2Index.at(id)];
}

// Get the edge by index.
const Edge& Graph::getEdgeByIndex(size_t index) {
  return edgeList[index];
}

// Get the edge list.
const std::vector<Edge>& Graph::getEdgeList() {
  return edgeList;
}

// Print the graph for debug.
void Graph::print() {
  // TODO
}

Graph::~Graph() { }
