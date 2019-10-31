//
//  graph.cpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>

#include "graph.hpp"

// Constructor.
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

// Print the for debug.
void Node::print() {
  std::cout << "position (" << position.x << ", " << position.y << ")" << std::endl;
  std::cout << "edge list: " << std::endl;
  for (auto eid : edges) {
    std::cout << eid << ", ";
  }
  std::cout << std::endl;
}

Node::~Node() { }
