//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 5/13/18.
//  Copyright © 2018 Renc. All rights reserved.
//

#include <iostream>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "io/binary.hpp"
#include "topology/graph.hpp"
#include "topology/trajectory.hpp"
#include "topology/auxiliaries.hpp"
#include "topology/sp_table.hpp"
#include "util/timer.hpp"
#include "service/spatial_compressor.hpp"
#include "service/temporal_compressor.hpp"
#include <vector>
#include <deque>
#include "topology/ac_automaton.hpp"
#include "topology/huffman.hpp"
#include "./user_defined/factory.hpp"
#include "./topology/grid_index.hpp"

#include "gtest/gtest.h"
#include "util/test/test_helper.hpp"
//#include "util/test/test_timer.hpp"
//#include "service/test/test_trajectory_reformattor.hpp"


int main(int argc, char** argv) {
  
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  
  auto graphReader = Factory::getGraphReader(GraphReaderType::SEATTLE_SAMPLE_ROADNET);
  Graph g;
  std::string path = "/Users/songrenchu/Develop/PRESS/data/WA_roadnetwork_and_single_trajectory/road_network.txt";
  graphReader->readGraph(path, g);
//  FileWriter fw("/Users/songrenchu/Develop/graph.txt", false);
//  g.store(fw);
//  auto gpsReader = Factory::getGPSTrajectoryReader(GPSTrajectoryReaderType::SEATTLE_SAMPLE_GPS);
//  GPSTrajectory gpsTrajectory;
//  std::string path2 = "/Users/songrenchu/Develop/PRESS/data/WA_roadnetwork_and_single_trajectory/gps_data.txt";
//  gpsReader->readGPSTrajectory(path2, gpsTrajectory);
//  FileWriter fw2("/Users/songrenchu/Develop/gps.txt", false);
//  gpsTrajectory.store(fw2);
  GridIndex gIndex(g, 100, 128);
//  FileWriter fw3("/Users/songrenchu/Develop/gridindex.txt", false);
//  gIndex.store(fw3);
//  gIndex.print();

//  FileReader fr("/Users/songrenchu/Develop/gridindex.txt", false);
//  GridIndex gIndex2(fr);
//  FileWriter fw4("/Users/songrenchu/Develop/gridindex2.txt", false);
//  gIndex2.store(fw4);
  std::unordered_set<int> edges;
  Point2D p1(-9164900.000000, 5318000.000000);
  gIndex.search(p1, 500, edges);
  for (auto eid: edges) {
    std::cout << eid << " ";
  }
  std::cout << std::endl;
  
////  FileWriter fw("/Users/songrenchu/Develop/test2.txt", true);
////  fw.writeChar('a');
////  fw.writeInt(123);
////  char* a = new char[4];
////  a[0] = 'A';
////  a[1] = 'B';
////  a[2] = 'C';
////  a[3] = '\0';
////  const char* b = a;
////  fw.writeString(b);
////  fw.writeDouble(3.456);
////  fw.writeChar(',');
////  fw.writeLong(23423);
////  fw.writeChar(',');
////  fw.writeShort(2);
////  
////
//  
////  FileReader fr("/Users/songrenchu/Develop/test2.txt", true);
////  std::cout << fr.nextChar() << std::endl;
////  std::cout << fr.nextInt() << std::endl;
////  std::cout << fr.nextString() << std::endl;
////  std::cout << fr.nextDouble() << std::endl;
////  std::cout << fr.nextChar() << std::endl;
////  std::cout << fr.nextLong() << std::endl;
////  std::cout << fr.nextChar() << std::endl;
////  std::cout << fr.nextShort() << std::endl;
////  
////  Node n(10, 20);
////  
////  n.print();
////  
////  n.addEdge(1);
////  n.addEdge(5);
////  n.addEdge(6);
////  n.addEdge(4);
////  
////  n.print();
////  std::cout << n.getEdgeNumber() << std::endl;
////  std::cout << n.getEdgeId(1) << std::endl;
////  std::cout << n.getEdgeId(3) << std::endl;
//////  std::cout << n.getEdgeId(5) << std::endl;
////  
////  Edge e(3, 5);
////  e.addPosition(0, 0);
////  e.addPosition(1, 1);
////  e.addPosition(3, 1);
////  std::cout << e.getSourceId() << std::endl;
////  std::cout << e.getTargetId() << std::endl;
////  std::cout << e.getGeoSize() << std::endl;
////  std::cout << e.getDistance() << std::endl;
////  e.print();
////  const std::vector<Point2D>& sp = e.getShape();
////  for (auto s : sp) {
////    s.print();
////  }
////  std::cout << std::endl;
//  
//  
//  
////  // insert code here...
////  FileReader fp("/Users/songrenchu/Develop/test.txt", (char *)"r");
////  std::cout << fp.isBinary() << std::endl;
////  std::cout << fp.fileOpened() << std::endl;
////  
////  
////  std::cout << fp.nextInt() << std::endl;
////  std::cout << fp.nextChar() << std::endl;
////  std::cout << fp.nextLong() << std::endl;
////  std::cout << fp.nextChar() << std::endl;
////  std::cout << fp.nextDouble() << std::endl;
////  std::cout << fp.nextChar() << std::endl;
////  std::cout << fp.nextString(1024) << std::endl;
////
////  std::vector<GPSPoint> trajectory;
////  trajectory.emplace_back(GPSPoint(1, 2.0, 3.0));
////  trajectory.emplace_back(GPSPoint(2, 2.1, 3.2));
////  trajectory.emplace_back(GPSPoint(3, 2.2, 3.3));
////  trajectory.emplace_back(GPSPoint(4, 2.3, 3.3));
////  trajectory.emplace_back(GPSPoint(5, 2.4, 3.2));
////  trajectory.emplace_back(GPSPoint(6, 2.5, 3.0));
////
////  GPSTrajectory traj(trajectory);
////  traj.print();
////  std::cout << std::endl;
////  std::cout << traj.getLength() << std::endl;
////  traj.getGPSPoint(0).print();
////
////  FileWriter fw0("/Users/songrenchu/Develop/traj.txt", true);
////  traj.store(fw0);
////  FileWriter fw1("/Users/songrenchu/Develop/traj2.txt", false);
////  traj.store(fw1);
//  
////
////  FileReader fw1("/Users/songrenchu/Develop/traj.txt", true);
////  FileReader fw2("/Users/songrenchu/Develop/traj2.txt", false);
////
////  std::cout << std::endl;
////  GPSTrajectory traj1(fw1);
////  traj1.print();
////  std::cout << std::endl;
////  for (auto gps: traj1.getTrajectory()) {
////    gps.print();
////  }
////  std::cout << std::endl;
////  GPSTrajectory traj2(fw2);
////  traj2.print();
////  std::cout << std::endl;
////  for (auto gps: traj2.getTrajectory()) {
////    gps.print();
////  }
////  std::cout << std::endl;
//  
////  std::vector<int> spatial;
////  std::vector<TemporalPair> temporal;
////  spatial.emplace_back(1);
////  spatial.emplace_back(3);
////  spatial.emplace_back(2);
////  spatial.emplace_back(5);
////  temporal.emplace_back(TemporalPair(0, 0));
////  temporal.emplace_back(TemporalPair(1, 1.5));
////  temporal.emplace_back(TemporalPair(2, 2));
////  temporal.emplace_back(TemporalPair(3, 5));
////  temporal.emplace_back(TemporalPair(4, 5.1));
////
////  PRESSTrajectory pTraj(spatial, temporal);
////  pTraj.print();
////  std::cout << pTraj.getSpatialLength() << std::endl;
////  std::cout << pTraj.getTemporalLength() << std::endl;
////  FileWriter fw0("/Users/songrenchu/Develop/ptraj1.txt", true);
////  FileWriter fw1("/Users/songrenchu/Develop/ptraj2.txt", true);
////  FileWriter fw2("/Users/songrenchu/Develop/ptraj3.txt", false);
////  FileWriter fw3("/Users/songrenchu/Develop/ptraj4.txt", false);
////  pTraj.store(fw0,fw1);
////  pTraj.store(fw2,fw3);
////  FileReader fr0("/Users/songrenchu/Develop/ptraj1.txt", true);
////  FileReader fr1("/Users/songrenchu/Develop/ptraj2.txt", true);
////  FileReader fr2("/Users/songrenchu/Develop/ptraj3.txt", false);
////  FileReader fr3("/Users/songrenchu/Develop/ptraj4.txt", false);
////
////  PRESSTrajectory pTraj1(fr0, fr1);
////  pTraj1.print();
////  std::cout << pTraj1.getSpatialLength() << std::endl;
////  std::cout << pTraj1.getTemporalLength() << std::endl;
////  for (auto id: pTraj1.getSpatialComponent()) {
////    std::cout << id << " ";
////  }
////  std::cout << std::endl;
////  for (auto tp: pTraj1.getTemporalComponent()) {
////    tp.print();
////  }
////  std::cout << std::endl;
////
////  PRESSTrajectory pTraj2(fr2, fr3);
////  pTraj2.print();
////  std::cout << pTraj2.getSpatialLength() << std::endl;
////  std::cout << pTraj2.getTemporalLength() << std::endl;
////  for (auto id: pTraj2.getSpatialComponent()) {
////    std::cout << id << " ";
////  }
////  std::cout << std::endl;
////  for (auto tp: pTraj2.getTemporalComponent()) {
////    tp.print();
////  }
////  std::cout << std::endl;
//  
////  std::vector<bool> bitArr;
//  
////  std::vector<bool> b1;
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(false);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(false);
////  b1.push_back(true);
////  b1.push_back(false);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(false);
////  b1.push_back(true);
////  b1.push_back(true);
////  b1.push_back(false);
////  b1.push_back(true);
////  b1.push_back(false);
////  
////  Binary bin1(b1);
////  bin1.print();
////  std::cout << bin1.getLength() << std::endl;
////  for (auto c: bin1.getBinary()) {
////    std::cout << "(" << (int)c << ")";
////  }
////  std::cout << std::endl;
////  FileWriter fw0("/Users/songrenchu/Develop/bin1.txt", true);
////  bin1.store(fw0);
//  
////  FileReader fr0("/Users/songrenchu/Develop/bin1.txt", true);
////  Binary bin2(fr0);
////  bin2.print();
////  std::cout << bin2.getLength() << std::endl;
////  for (auto c: bin2.getBinary()) {
////    std::cout << "(" << (int)c << ")";
////  }
////  std::cout << std::endl;
////  
////  Binary bin3(bin2.getLength(), bin2.getBinary());
////  bin3.print();
////  std::cout << bin3.getLength() << std::endl;
////  for (auto c: bin3.getBinary()) {
////    std::cout << "(" << (int)c << ")";
////  }
////  std::cout << std::endl;
////  
////  Binary bin4 = bin3;
////  
////  bin4.print();
////  std::cout << bin4.getLength() << std::endl;
////  for (auto c: bin4.getBinary()) {
////    std::cout << "(" << (int)c << ")";
////  }
////  std::cout << std::endl;
////  bin4.getBitArray(bitArr);
////  for (auto c: bitArr) {
////    std::cout << c << " ";
////  }
////  std::cout << std::endl;
//  
////  Timer t1, t2;
////  t1.start(); t2.start();
////  for (int i = 0; i < 100000000; i++) {
////    
////  }
////  t1.resume();
////
////  t2.pause();
////  for (int i = 0; i < 100000000; i++) {
////    
////  }
////
////  t2.resume();
////  for (int i = 0; i < 100000000; i++) {
////    
////  }
////  
////  std::cout << t1.getMilliSeconds() << std::endl;
////  std::cout << t1.getSeconds() << std::endl;
////  std::cout << t1.getSystemClockDuration() << std::endl;
////  
////  std::cout << t2.getMilliSeconds() << std::endl;
////  std::cout << t2.getSeconds() << std::endl;
////  std::cout << t2.getSystemClockDuration() << std::endl;
//  
////    std::vector<bool> b1;
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(false);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(false);
////    b1.push_back(true);
////    b1.push_back(false);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(false);
////    b1.push_back(true);
////    b1.push_back(true);
////    b1.push_back(false);
////    b1.push_back(true);
////    b1.push_back(false);
////    Binary spatial(b1);
////    std::vector<TemporalPair> temporal;
////    temporal.emplace_back(TemporalPair(0, 0));
////    temporal.emplace_back(TemporalPair(1, 1.5));
////    temporal.emplace_back(TemporalPair(2, 2));
////    temporal.emplace_back(TemporalPair(3, 5));
////    temporal.emplace_back(TemporalPair(4, 5.1));
////  
////    PRESSCompressedTrajectory pTraj(spatial, temporal);
////    pTraj.print();
////    std::cout << pTraj.getTemporalLength() << std::endl;
////    FileWriter fw0("/Users/songrenchu/Develop/pctraj1.txt", true);
////    FileWriter fw1("/Users/songrenchu/Develop/pctraj2.txt", true);
//////    FileWriter fw2("/Users/songrenchu/Develop/ptraj3.txt", false);
//////    FileWriter fw3("/Users/songrenchu/Develop/ptraj4.txt", false);
////    pTraj.store(fw0,fw1);
//////    pTraj.store(fw2,fw3);
////    FileReader fr0("/Users/songrenchu/Develop/pctraj1.txt", true);
////    FileReader fr1("/Users/songrenchu/Develop/pctraj2.txt", true);
//////    FileReader fr2("/Users/songrenchu/Develop/ptraj3.txt", false);
//////    FileReader fr3("/Users/songrenchu/Develop/ptraj4.txt", false);
//////  
////    PRESSCompressedTrajectory pTraj1(fr0, fr1);
////    pTraj1.print();
//////    std::cout << pTraj1.getSpatialLength() << std::endl;
////    std::cout << pTraj1.getTemporalLength() << std::endl;
//////    for (auto id: pTraj1.getSpatialComponent()) {
//////      std::cout << id << " ";
//////    }
////    pTraj1.getSpatialComponent().print();
////    for (auto tp: pTraj1.getTemporalComponent()) {
////      tp.print();
////    }
////    std::cout << std::endl;
//////
//////    PRESSTrajectory pTraj2(fr2, fr3);
//////    pTraj2.print();
//////    std::cout << pTraj2.getSpatialLength() << std::endl;
//////    std::cout << pTraj2.getTemporalLength() << std::endl;
//////    for (auto id: pTraj2.getSpatialComponent()) {
//////      std::cout << id << " ";
//////    }
//////    std::cout << std::endl;
//////    for (auto tp: pTraj2.getTemporalComponent()) {
//////      tp.print();
//////    }
//////    std::cout << std::endl;
//  
//  Graph g;
////  g.print();
////  FileWriter fw0("/Users/songrenchu/Develop/graph.txt", false);
////  g.store(fw0);
////  FileWriter fw1("/Users/songrenchu/Develop/graph1.txt", true);
////  g.store(fw1);
////  
//
////  FileReader fr0("/Users/songrenchu/Develop/graph.txt", false);
////  Graph g2(fr0);
////  g2.print();
////  std::cout << std::endl;
////  FileReader fr1("/Users/songrenchu/Develop/graph1.txt", true);
////  Graph g3(fr1);
////  g3.print();
//  
//  
//  FileReader fr0("/Users/songrenchu/Develop/ac,txt", false);
//  FileWriter fw0("/Users/songrenchu/Develop/ac2,txt", false);
//  ACAutomaton ac(fr0);
//  ac.print();
//  ac.store(fw0);
//  
//  Huffman hm(ac);
//  std::cout << "here" << std::endl;
//  hm.print();
//  
//  {
//    FileWriter fw1("/Users/songrenchu/Develop/hm.txt", false);
//    hm.store(fw1);
//  }
//  
//  {
//    FileReader fr1("/Users/songrenchu/Develop/hm.txt", false);
//    Huffman hm2(fr1);
//    hm2.print();
//    
//    std::vector<int> spatial {30, 8, 6, 23, 25, 11};
//    std::vector<bool> result;
//    hm2.encode(spatial, result);
//    for (auto c: result) {
//      std::cout << c;
//    }
//    std::cout << std::endl;
//    std::vector<int> result2;
//    hm2.decode(result, result2);
//    for (auto s: result2) {
//      std::cout << s << " ";
//    }
//    std::cout << std::endl;
//  }
//  
//  {
//    SPTable sp(g);
////    FileWriter fw0("/Users/songrenchu/Develop/sp.txt", false);
////    sp.store(fw0);
//////    FileReader fr0("/Users/songrenchu/Develop/sp.txt", false);
//////    SPTable sp(fr0);
//    sp.print();
//
//    SpatialCompressor sc;
//
//    std::vector<int> spComp;
//    spComp.emplace_back(4);
//    spComp.emplace_back(16);
//    spComp.emplace_back(15);
//    spComp.emplace_back(13);
//    spComp.emplace_back(7);
//    spComp.emplace_back(5);
//    spComp.emplace_back(0);
//    spComp.emplace_back(2);
//
//
//    std::vector<int> result;
//    sc.shortestPathCompression(g, sp, spComp, result);
//    std::cout << "sp compression" << std::endl;
//    for (auto s: result) {
//      std::cout << s << " ";
//    }
//    std::cout << std::endl;
//
//    std::vector<std::vector<int>> spComps;
//    spComps.emplace_back(std::vector<int>{ 1, 5, 8, 6, 3 });
//    spComps.emplace_back(std::vector<int>{ 1, 5, 2, 1, 4, 8 });
//    spComps.emplace_back(std::vector<int>{ 2, 1, 4, 6 });
//    ACAutomaton ac(g, spComps, 3);
//
//    ac.print();
//    ac.store(fw0);
//
//    Huffman hm(ac);
//    std::vector<bool> b;
//    sc.hybridSpatialCompression(g, sp, ac, hm, spComp, b);
//    std::cout << "hsc" << std::endl;
//    for (auto bb: b) {
//      std::cout << bb;
//    }
//    std::cout << std::endl;
//
//    std::vector<int> spComp2 { 1, 4, 7, 5, 8, 6, 3, 1, 5, 2, 10 };
//    std::cout << "sp compression" << std::endl;
//    sc.shortestPathCompression(g, sp, spComp2, result);
//    for (auto s: result) {
//      std::cout << s << " ";
//    }
//    std::cout << std::endl;
//    sc.frequentSubTrajectoryCompresson(ac, hm, spComp2, b);
//    std::cout << "hsc" << std::endl;
//    for (auto bb: b) {
//      std::cout << bb;
//    }
//    std::cout << std::endl;
//  }
//
//  std::vector<TemporalPair> pairs;
//  pairs.emplace_back(TemporalPair(0, 0));
//  pairs.emplace_back(TemporalPair(1, 0));
//  pairs.emplace_back(TemporalPair(2, 0));
//  pairs.emplace_back(TemporalPair(3, 0));
//  pairs.emplace_back(TemporalPair(4, 0));
//  pairs.emplace_back(TemporalPair(5, 1));
//  std::vector<TemporalPair> result;
//  TemporalCompressor ts;
//  ts.boundedTemporalCompression(pairs, result, 3, 3);
//  for (auto& p: result) {
//    p.print();
//  }
//  std::cout << std::endl;
//
////  std::vector<int> eList { 1, 3, 2, 5, 4, 3 };
////  MapMatchedTrajectory mt(eList);
////  mt.print();
////  FileWriter fw3("/Users/songrenchu/Develop/mm.txt", false);
////  mt.store(fw3);
//  FileReader fr3("/Users/songrenchu/Develop/mm.txt", false);
//  MapMatchedTrajectory mt(fr3);
//  mt.print();
//  std::cout << mt.getLength() << std::endl;
//  for (auto e : mt.getEdgeList()) {
//    std::cout << e << " ";
//  }
//  std::cout << std::endl;
//  std::cout << mt.getEdgeId(1) << std::endl;
//  
//  
//  // TODO: test formatter math functions.
//
  
}
