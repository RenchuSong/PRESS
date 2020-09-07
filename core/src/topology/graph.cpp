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

// Node constructor.
Node::Node(Point2D& point): position(point.x, point.y), edgeNumber(0) { }

// Add one edge to the node edge list.
void Node::addEdge(int eid) {
  edgeNumber++;
  edges.push_back(eid);
}

// Get the position.
Point2D& Node::getPosition() {
  return position;
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

// Get the edge list.
const std::vector<int>& Node::getEdgeList() {
  return edges;
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

// Add one position in the shape.
size_t Edge::addPosition(Point2D& point) {
  return addPosition(point.x, point.y);
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


Graph::Graph() { }

// Construct the graph from a file.
Graph::Graph(FileReader& graphReader) {
  // Load the nodes.
  nodeNumber = graphReader.nextInt();
  for (auto i = 0; i < nodeNumber; i++) {
    nodeList.emplace_back(Node(graphReader.nextDouble(), graphReader.nextDouble()));
    auto& node = nodeList.back();
    auto edgeSize = graphReader.nextInt();
    for (auto j = 0; j < edgeSize; j++) {
      node.addEdge(graphReader.nextInt());
    }
  }

  // Load the edges.
  edgeNumber = graphReader.nextInt();
  for (auto i = 0; i < edgeNumber; i++) {
    edgeList.emplace_back(Edge(graphReader.nextInt(), graphReader.nextInt()));
    auto& edge = edgeList.back();
    auto shapeSize = graphReader.nextInt();
    for (auto j = 0; j < shapeSize; j++) {
      edge.addPosition(graphReader.nextDouble(), graphReader.nextDouble());
    }
  }
}

Graph::Graph(std::vector<Node>& nodeList, std::vector<Edge>& edgeList) {
  setGraph(nodeList, edgeList);
}

void Graph::setGraph(std::vector<Node>& nodeList, std::vector<Edge>& edgeList) {
  nodeNumber = nodeList.size();
  this->nodeList.clear();
  this->nodeList.insert(this->nodeList.end(), nodeList.begin(), nodeList.end());
  edgeNumber = edgeList.size();
  this->edgeList.clear();
  this->edgeList.insert(this->edgeList.end(), edgeList.begin(), edgeList.end());
}

// Get node number in the graph.
size_t Graph::getNodeNumber() {
  return nodeNumber;
}

//// Get the node with given id.
//Node& Graph::getNodeById(int id) {
//  return nodeList[nodeId2Index.at(id)];
//}

// Get the node by index.
Node& Graph::getNode(size_t index) {
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

//// Get the edge with given id.
//Edge& Graph::getEdgeById(int id) {
//  return edgeList[edgeId2Index.at(id)];
//}

// Get the edge by index.
Edge& Graph::getEdge(size_t index) {
  return edgeList[index];
}

// Get the edge list.
const std::vector<Edge>& Graph::getEdgeList() {
  return edgeList;
}

// Store the graph into a file.
void Graph::store(FileWriter& graphWriter) {
  // Store the nodes.
  graphWriter.writeInt((int)nodeNumber);
  graphWriter.writeEol();
  for (auto node: nodeList) {
    // Node position.
    Point2D& point = node.getPosition();
    graphWriter.writeDouble(point.x);
    graphWriter.writeSeparator();
    graphWriter.writeDouble(point.y);
    graphWriter.writeSeparator();
    // Node edge list.
    graphWriter.writeInt((int)node.getEdgeNumber());
    for (auto edgeId: node.getEdgeList()) {
      graphWriter.writeSeparator();
      graphWriter.writeInt(edgeId);
    }
    graphWriter.writeEol();
  }

  // Store the edges.
  graphWriter.writeInt((int)edgeNumber);
  graphWriter.writeEol();
  for (auto edge: edgeList) {
    // Source and target node id.
    graphWriter.writeInt(edge.getSourceId());
    graphWriter.writeSeparator();
    graphWriter.writeInt(edge.getTargetId());

    // Write the shape.
    graphWriter.writeSeparator();
    graphWriter.writeInt((int)edge.getGeoSize());
    for (auto point: edge.getShape()) {
      graphWriter.writeSeparator();
      graphWriter.writeDouble(point.x);
      graphWriter.writeSeparator();
      graphWriter.writeDouble(point.y);
    }
    graphWriter.writeEol();
  }
}

// Print the graph for debug.
void Graph::print() {
  // Print nodes.
  std::cout << "Node: " << getNodeNumber() << std::endl;
  for (auto i = 0; i < nodeNumber; i++) {
    Node& node = getNode(i);
    std::cout << i << ": ";
    node.getPosition().print();
    for (auto j = 0; j < node.getEdgeNumber(); j++) {
      std::cout << " " << node.getEdgeId(j);
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // Print edges.
  std::cout << "Edge: " << getEdgeNumber() << std::endl;
  for (auto i = 0; i < edgeNumber; i++) {
    Edge& edge = getEdge(i);
    std::cout << i << ": (" << edge.getSourceId() << ")-[" << edge.getDistance() << "]->(" << edge.getTargetId() << ") " << edge.getGeoSize() << "->";
    for (auto pt : edge.getShape()) {
      pt.print();
    }
    std::cout << std::endl;
  }
}

Graph::~Graph() { }
