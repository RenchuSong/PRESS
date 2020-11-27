//
//  auxiliary.cpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>
#include <algorithm>

#include "auxiliary.hpp"
#include "../util/helper.hpp"

void findDistance(
  const Graph& graph,
  const std::unordered_map<int, int>& targetPrev,
  std::unordered_map<int, double>& targetSPDist,
  int src,
  int node
) {
  if (targetSPDist.find(node) != targetSPDist.end() || src == node) {
    return;
  }
  std::vector<int> edges;
  while (targetSPDist.find(node) == targetSPDist.end() && node != src) {
    if (targetPrev.find(node) == targetPrev.end()) {
      break;
    }
    int edge = targetPrev.at(node);
    if (edge == EDGE_NOT_EXIST) {
      break;
    }
    edges.emplace_back(edge);
    node = graph.getEdge(edge).getSourceId();
  }
  if (edges.size() > 0) {
    std::reverse(edges.begin(), edges.end());
    double dist = 0;
    auto& firstEdge = graph.getEdge(edges.at(0));
    if (targetSPDist.find(firstEdge.getSourceId()) != targetSPDist.end()) {
      dist = targetSPDist.at(firstEdge.getSourceId());
    }
    for (int eid: edges) {
      auto& edge = graph.getEdge(eid);
      dist += edge.getDistance();
      targetSPDist[edge.getTargetId()] = dist;
    }
  }
  return;
}

void findMBR(
  const Graph& graph,
  const std::unordered_map<int, int>& targetPrev,
  std::unordered_map<int, std::pair<Point2D, Point2D> >& targetMBR,
  int src,
  int node,
  const std::vector<std::pair<Point2D, Point2D> >& edgeMBR
) {
  if (targetMBR.find(node) != targetMBR.end() || src == node) {
    return;
  }
  std::vector<int> edges;
  while (targetMBR.find(node) == targetMBR.end() && node != src) {
    if (targetPrev.find(node) == targetPrev.end()) {
      break;
    }
    int edge = targetPrev.at(node);
    if (edge == EDGE_NOT_EXIST) {
      break;
    }
    edges.emplace_back(edge);
    node = graph.getEdge(edge).getSourceId();
  }
  if (edges.size() > 0) {
    std::reverse(edges.begin(), edges.end());
    auto mbr = std::make_pair(graph.getNode(src).getPosition(), graph.getNode(src).getPosition());
    auto& firstEdge = graph.getEdge(edges.at(0));
    if (targetMBR.find(firstEdge.getSourceId()) != targetMBR.end()) {
      mbr = targetMBR.at(firstEdge.getSourceId());
    }
    for (int eid: edges) {
      if (eid >= edgeMBR.size()) {
        std::cout << eid << " " << edgeMBR.size() << std::endl;
      }
      auto& edge = graph.getEdge(eid);
      extendMBR(mbr, edgeMBR.at(eid).first);
      extendMBR(mbr, edgeMBR.at(eid).second);
      targetMBR[edge.getTargetId()] = mbr;
    }
  }
  return;
}

Auxiliary::Auxiliary() { }

Auxiliary::Auxiliary(const Graph& graph, const SPTable& spTable, const ACAutomaton& acAutomaton) {
  build(graph, spTable, acAutomaton);
}

void Auxiliary::build(const Graph& graph, const SPTable& spTable, const ACAutomaton& acAutomaton) {
  clear();
  // Shortest path distance.
  nodeNumber = spTable.nodeNumber;
  for (int i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, double> targetSPDist;
    for (auto& nodePair: spTable.prevEdge[i]) {
      findDistance(graph, spTable.prevEdge[i], targetSPDist, i, nodePair.first);
    }
    nodePairSPDist.emplace_back(targetSPDist);
  }
  // Trie node distance, start node and end node.
  trieSize = acAutomaton.getTrieSize();
  trieNodeDist.emplace_back(0);
  trieNodeStartNode.emplace_back(-1);
  trieNodeEndNode.emplace_back(-1);
  for (int i = 1; i < trieSize; ++i) {
    int father = acAutomaton.getFather(i);
    auto& fatherEdge = graph.getEdge(acAutomaton.getEdge(father));
    auto& edge = graph.getEdge(acAutomaton.getEdge(i));
    trieNodeDist.emplace_back(
      trieNodeDist.at(father) +
      edge.getDistance() +
      getSPDistance(fatherEdge.getTargetId(), edge.getSourceId())
    );
    if (father == ROOT_NODE) {
      trieNodeStartNode.emplace_back(edge.getSourceId());
    } else {
      trieNodeStartNode.emplace_back(trieNodeStartNode.at(father));
    }
    trieNodeEndNode.emplace_back(edge.getTargetId());
  }
  // MBR of each edge in the graph.
  edgeNumber = graph.getEdgeNumber();
  for (int i = 0; i < edgeNumber; ++i) {
    auto& shape = graph.getEdge(i).getShape();
    auto mbr = std::make_pair(Point2D(shape.at(0)), Point2D(shape.at(0)));
    for (auto& point: shape) {
      extendMBR(mbr, point);
    }
    edgeMBR.emplace_back(mbr);
  }
  // MBR of shortest path between each pair of nodes.
  for (int i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, std::pair<Point2D, Point2D> > targetMBR;
    for (auto& nodePair: spTable.prevEdge[i]) {
      findMBR(graph, spTable.prevEdge[i], targetMBR, i, nodePair.first, edgeMBR);
    }
    nodePairMBR.emplace_back(targetMBR);
  }
  // MBR of each Trie node.
  trieNodeMBR.emplace_back(std::make_pair(Point2D(0, 0), Point2D(0, 0)));
  for (int i = 1; i < trieSize; ++i) {
    int father = acAutomaton.getFather(i);
    auto& fatherEdge = graph.getEdge(acAutomaton.getEdge(father));
    auto& edge = graph.getEdge(acAutomaton.getEdge(i));
    auto mbr = edgeMBR.at(acAutomaton.getEdge(i));
    if (father != ROOT_NODE) {
      // Extend the father
      extendMBR(mbr, trieNodeMBR.at(father).first);
      extendMBR(mbr, trieNodeMBR.at(father).second);
      // Extend the node pair.
      auto npMbr = getNodePairMBR(graph, fatherEdge.getTargetId(), edge.getSourceId());
      extendMBR(mbr, npMbr.first);
      extendMBR(mbr, npMbr.second);
    }
    trieNodeMBR.emplace_back(mbr);
  }
}

Auxiliary::Auxiliary(FileReader& auxReader) {
  load(auxReader);
}

void Auxiliary::load(FileReader& auxReader) {
  clear();
  nodeNumber = auxReader.nextInt();
  edgeNumber = auxReader.nextInt();
  trieSize = auxReader.nextInt();
  for (auto i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, double> singleSPDist;
    auto len = auxReader.nextInt();
    for (auto j = 0; j < len; ++j) {
      singleSPDist.emplace(auxReader.nextInt(), auxReader.nextDouble());
    }
    nodePairSPDist.emplace_back(singleSPDist);
  }
  for (auto i = 0; i < trieSize; ++i) {
    trieNodeDist.emplace_back(auxReader.nextDouble());
  }
  for (auto i = 0; i < trieSize; ++i) {
    trieNodeStartNode.emplace_back(auxReader.nextInt());
  }
  for (auto i = 0; i < trieSize; ++i) {
    trieNodeEndNode.emplace_back(auxReader.nextInt());
  }
  for (auto i = 0; i < edgeNumber; ++i) {
    edgeMBR.emplace_back(
      std::make_pair(
        Point2D(auxReader.nextDouble(), auxReader.nextDouble()),
        Point2D(auxReader.nextDouble(), auxReader.nextDouble())
      )
    );
  }
  for (auto i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, std::pair<Point2D, Point2D> > singleMBR;
    auto len = auxReader.nextInt();
    for (auto j = 0; j < len; ++j) {
      singleMBR.emplace(
        auxReader.nextInt(),
        std::make_pair(
          Point2D(auxReader.nextDouble(), auxReader.nextDouble()),
          Point2D(auxReader.nextDouble(), auxReader.nextDouble())
        )
      );
    }
    nodePairMBR.emplace_back(singleMBR);
  }
  for (auto i = 0; i < trieSize; ++i) {
    trieNodeMBR.emplace_back(
      std::make_pair(
        Point2D(auxReader.nextDouble(), auxReader.nextDouble()),
        Point2D(auxReader.nextDouble(), auxReader.nextDouble())
      )
    );
  }
}

void Auxiliary::store(FileWriter& auxWriter) {
  auxWriter.writeInt((int)nodeNumber);
  auxWriter.writeEol();
  auxWriter.writeInt((int)edgeNumber);
  auxWriter.writeEol();
  auxWriter.writeInt((int)trieSize);
  auxWriter.writeEol();
  for (auto& singleSPDist: nodePairSPDist) {
    auxWriter.writeInt((int)singleSPDist.size());
    for (auto& nodeDist: singleSPDist) {
      auxWriter.writeSeparator();
      auxWriter.writeInt(nodeDist.first);
      auxWriter.writeSeparator();
      auxWriter.writeDouble(nodeDist.second);
    }
    auxWriter.writeEol();
  }
  for (auto dist: trieNodeDist) {
    auxWriter.writeSeparator();
    auxWriter.writeDouble(dist);
  }
  auxWriter.writeEol();
  for (auto start: trieNodeStartNode) {
    auxWriter.writeSeparator();
    auxWriter.writeInt(start);
  }
  auxWriter.writeEol();
  for (auto end: trieNodeEndNode) {
    auxWriter.writeSeparator();
    auxWriter.writeInt(end);
  }
  auxWriter.writeEol();
  for (auto& mbr: edgeMBR) {
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.first.x);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.first.y);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.second.x);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.second.y);
  }
  auxWriter.writeEol();
  for (auto& singleMBR: nodePairMBR) {
    auxWriter.writeInt((int)singleMBR.size());
    for (auto& mbr: singleMBR) {
      auxWriter.writeSeparator();
      auxWriter.writeInt(mbr.first);
      auxWriter.writeSeparator();
      auxWriter.writeDouble(mbr.second.first.x);
      auxWriter.writeSeparator();
      auxWriter.writeDouble(mbr.second.first.y);
      auxWriter.writeSeparator();
      auxWriter.writeDouble(mbr.second.second.x);
      auxWriter.writeSeparator();
      auxWriter.writeDouble(mbr.second.second.y);
    }
    auxWriter.writeEol();
  }
  for (auto& mbr: trieNodeMBR) {
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.first.x);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.first.y);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.second.x);
    auxWriter.writeSeparator();
    auxWriter.writeDouble(mbr.second.y);
  }
  auxWriter.writeEol();
}

void Auxiliary::clear() {
  nodeNumber = 0;
  edgeNumber = 0;
  trieSize = 0;
  nodePairSPDist.clear();
  trieNodeDist.clear();
  trieNodeStartNode.clear();
  trieNodeEndNode.clear();
  edgeMBR.clear();
  nodePairMBR.clear();
  trieNodeMBR.clear();
}

// Get the node number.
size_t Auxiliary::getNodeNumber() const {
  return nodeNumber;
}

// Get the shortest path distance between two nodes.
double Auxiliary::getSPDistance(size_t srcIndex, size_t tgtIndex) const {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  auto& pairDist = nodePairSPDist.at(srcIndex);
  if (pairDist.find((int)tgtIndex) != pairDist.end()) {
    return pairDist.at((int)tgtIndex);
  } else {
    // This means the sequence is not consecutive, in this case treat as new trajectory segment.
    return 0;
  }
}

// Get the trie node distance.
double Auxiliary::getTrieNodeDistance(size_t index) const {
  assert(index < trieSize);
  return trieNodeDist.at(index);
}

// Get the trie node start node.
int Auxiliary::getTrieNodeStartNode(size_t index) const {
  assert(index < trieSize);
  return trieNodeStartNode.at(index);
}

// Get the trie node end node.
int Auxiliary::getTrieNodeEndNode(size_t index) const {
  assert(index < trieSize);
  return trieNodeEndNode.at(index);
}

// Get the shortest path distance between two nodes.
std::pair<Point2D, Point2D> Auxiliary::getNodePairMBR(
  const Graph& graph,
  size_t srcIndex,
  size_t tgtIndex
) const {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  auto& pairMBR = nodePairMBR.at(srcIndex);
  if (pairMBR.find((int)tgtIndex) != pairMBR.end()) {
    return pairMBR.at((int)tgtIndex);
  } else {
    // This means there is no data recorded.
    return std::make_pair(
      graph.getNode(srcIndex).getPosition(),
      graph.getNode(tgtIndex).getPosition()
    );
  }
}

// Get the trie node MBR.
const std::pair<Point2D, Point2D>& Auxiliary::getTrieNodeMBR(size_t index) const {
  assert(index < trieSize);
  return trieNodeMBR.at(index);
}

// Get the edge MBR.
const std::pair<Point2D, Point2D>& Auxiliary::getEdgeMBR(size_t index) const {
  assert(index < edgeNumber);
  return edgeMBR.at(index);
}

void Auxiliary::print() const {
  // TODO
}

Auxiliary::~Auxiliary() { }
