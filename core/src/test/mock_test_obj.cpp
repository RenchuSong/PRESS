//
//  mock_test_obj.cpp
//  press-v3
//
//  Created by Renchu Song on 7/12/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "mock_test_obj.hpp"

#include <vector>

#include "../util/helper.hpp"

MockTestObjects::MockTestObjects() {
  // Construct the test roadnet.
  // long
  //  .4 6-----7
  //          /
  //  .3     /    5
  //        /    /  ⟍
  //  .2    3---/--->4
  //       / ⟍ /
  //  .1  /    2
  //     /   ⟋ |
  //  .0 0 ⟋   1
  //    .0 .1 .2 .3 .4 .5 lat
  std::vector<GPSPoint> gpsList {
    GPSPoint(0, 37.0, 122.0),
    GPSPoint(0, 37.2, 122.0),
    GPSPoint(0, 37.2, 122.1),
    GPSPoint(0, 37.1, 122.2),
    GPSPoint(0, 37.4, 122.2),
    GPSPoint(0, 37.3, 122.3),
    GPSPoint(0, 37.0, 122.4),
    GPSPoint(0, 37.2, 122.4),
  };
  std::vector<Node> nodeList;
  for (auto& gps: gpsList) {
    auto point = gps2Point2D(gps);
    nodeList.emplace_back(Node(point));
  }
  std::vector<Edge> edgeList {
    Edge(0, 2),
    Edge(2, 0),
    Edge(2, 1),
    Edge(1, 2),
    Edge(0, 3),
    Edge(3, 0),
    Edge(3, 2),
    Edge(2, 3),
    Edge(3, 7),
    Edge(7, 3),
    Edge(6, 7),
    Edge(7, 6),
    Edge(2, 5),
    Edge(5, 2),
    Edge(5, 4),
    Edge(4, 5),
    Edge(3, 4),
  };
  nodeList[0].addEdge(0);
  nodeList[0].addEdge(4);
  nodeList[1].addEdge(3);
  nodeList[2].addEdge(1);
  nodeList[2].addEdge(2);
  nodeList[2].addEdge(7);
  nodeList[2].addEdge(12);
  nodeList[3].addEdge(5);
  nodeList[3].addEdge(6);
  nodeList[3].addEdge(16);
  nodeList[3].addEdge(8);
  nodeList[4].addEdge(15);
  nodeList[5].addEdge(14);
  nodeList[5].addEdge(13);
  nodeList[6].addEdge(10);
  nodeList[7].addEdge(11);
  nodeList[7].addEdge(9);
  for (auto& edge: edgeList) {
    edge.addPosition(nodeList.at(edge.getSourceId()).getPosition());
    edge.addPosition(nodeList.at(edge.getTargetId()).getPosition());
  }
  graph = new Graph(nodeList, edgeList);
  
  // Construct the SP table.
  spTable = new SPTable(*graph, 20000);

//  std::vector<std::vector<int> > sp;
//  acAutomaton = new ACAutomaton(*graph, sp, 3);
//  
}

MockTestObjects::~MockTestObjects() {
  delete graph;
  delete spTable;
}
