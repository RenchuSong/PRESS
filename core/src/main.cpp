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
#include <vector>

int main(int argc, const char * argv[]) {
//  FileWriter fw("/Users/songrenchu/Develop/test2.txt", true);
//  fw.writeChar('a');
//  fw.writeInt(123);
//  char* a = new char[4];
//  a[0] = 'A';
//  a[1] = 'B';
//  a[2] = 'C';
//  a[3] = '\0';
//  const char* b = a;
//  fw.writeString(b);
//  fw.writeDouble(3.456);
//  fw.writeChar(',');
//  fw.writeLong(23423);
//  fw.writeChar(',');
//  fw.writeShort(2);
//  
//
  
//  FileReader fr("/Users/songrenchu/Develop/test2.txt", true);
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextInt() << std::endl;
//  std::cout << fr.nextString() << std::endl;
//  std::cout << fr.nextDouble() << std::endl;
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextLong() << std::endl;
//  std::cout << fr.nextChar() << std::endl;
//  std::cout << fr.nextShort() << std::endl;
  
//  Node n(10, 20);
//  
//  n.print();
//  
//  n.addEdge(1);
//  n.addEdge(5);
//  n.addEdge(6);
//  n.addEdge(4);
//  
//  n.print();
//  std::cout << n.getEdgeNumber() << std::endl;
//  std::cout << n.getEdgeId(1) << std::endl;
//  std::cout << n.getEdgeId(3) << std::endl;
////  std::cout << n.getEdgeId(5) << std::endl;
//  
//  Edge e(3, 5);
//  e.addPosition(0, 0);
//  e.addPosition(1, 1);
//  e.addPosition(3, 1);
//  std::cout << e.getSourceId() << std::endl;
//  std::cout << e.getTargetId() << std::endl;
//  std::cout << e.getGeoSize() << std::endl;
//  std::cout << e.getDistance() << std::endl;
//  e.print();
//  const std::vector<Point2D>& sp = e.getShape();
//  for (auto s : sp) {
//    s.print();
//  }
//  std::cout << std::endl;
  
  
  
//  // insert code here...
//  FileReader fp("/Users/songrenchu/Develop/test.txt", (char *)"r");
//  std::cout << fp.isBinary() << std::endl;
//  std::cout << fp.fileOpened() << std::endl;
//  
//  
//  std::cout << fp.nextInt() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextLong() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextDouble() << std::endl;
//  std::cout << fp.nextChar() << std::endl;
//  std::cout << fp.nextString(1024) << std::endl;

//  std::vector<GPSPoint> trajectory;
//  trajectory.emplace_back(GPSPoint(1, 2.0, 3.0));
//  trajectory.emplace_back(GPSPoint(2, 2.1, 3.2));
//  trajectory.emplace_back(GPSPoint(3, 2.2, 3.3));
//  trajectory.emplace_back(GPSPoint(4, 2.3, 3.3));
//  trajectory.emplace_back(GPSPoint(5, 2.4, 3.2));
//  trajectory.emplace_back(GPSPoint(6, 2.5, 3.0));
//  
//  GPSTrajectory traj(trajectory);
//  traj.print();
//  std::cout << std::endl;
//  std::cout << traj.getLength() << std::endl;
//  traj.getGPSPoint(0).print();
//  
////  FileWriter fw0("/Users/songrenchu/Develop/traj.txt", true);
////  traj.store(fw0);
//
//  FileReader fw1("/Users/songrenchu/Develop/traj.txt", true);
//  FileReader fw2("/Users/songrenchu/Develop/traj2.txt", false);
//  
//  std::cout << std::endl;
//  GPSTrajectory traj1(fw1);
//  traj1.print();
//  std::cout << std::endl;
//  for (auto gps: traj1.getTrajectory()) {
//    gps.print();
//  }
//  std::cout << std::endl;
//  GPSTrajectory traj2(fw2);
//  traj2.print();
//  std::cout << std::endl;
//  for (auto gps: traj2.getTrajectory()) {
//    gps.print();
//  }
//  std::cout << std::endl;
  
//  std::vector<int> spatial;
//  std::vector<TemporalPair> temporal;
//  spatial.emplace_back(1);
//  spatial.emplace_back(3);
//  spatial.emplace_back(2);
//  spatial.emplace_back(5);
//  temporal.emplace_back(TemporalPair(0, 0));
//  temporal.emplace_back(TemporalPair(1, 1.5));
//  temporal.emplace_back(TemporalPair(2, 2));
//  temporal.emplace_back(TemporalPair(3, 5));
//  temporal.emplace_back(TemporalPair(4, 5.1));
//  
//  PRESSTrajectory pTraj(spatial, temporal);
//  pTraj.print();
//  std::cout << pTraj.getSpatialLength() << std::endl;
//  std::cout << pTraj.getTemporalLength() << std::endl;
////  FileWriter fw0("/Users/songrenchu/Develop/ptraj1.txt", true);
////  FileWriter fw1("/Users/songrenchu/Develop/ptraj2.txt", true);
////  FileWriter fw2("/Users/songrenchu/Develop/ptraj3.txt", false);
////  FileWriter fw3("/Users/songrenchu/Develop/ptraj4.txt", false);
////  pTraj.store(fw0,fw1);
////  pTraj.store(fw2,fw3);
//  FileReader fr0("/Users/songrenchu/Develop/ptraj1.txt", true);
//  FileReader fr1("/Users/songrenchu/Develop/ptraj2.txt", true);
//  FileReader fr2("/Users/songrenchu/Develop/ptraj3.txt", false);
//  FileReader fr3("/Users/songrenchu/Develop/ptraj4.txt", false);
//
//  PRESSTrajectory pTraj1(fr0, fr1);
//  pTraj1.print();
//  std::cout << pTraj1.getSpatialLength() << std::endl;
//  std::cout << pTraj1.getTemporalLength() << std::endl;
//  for (auto id: pTraj1.getSpatialComponent()) {
//    std::cout << id << " ";
//  }
//  std::cout << std::endl;
//  for (auto tp: pTraj1.getTemporalComponent()) {
//    tp.print();
//  }
//  std::cout << std::endl;
//  
//  PRESSTrajectory pTraj2(fr2, fr3);
//  pTraj2.print();
//  std::cout << pTraj2.getSpatialLength() << std::endl;
//  std::cout << pTraj2.getTemporalLength() << std::endl;
//  for (auto id: pTraj2.getSpatialComponent()) {
//    std::cout << id << " ";
//  }
//  std::cout << std::endl;
//  for (auto tp: pTraj2.getTemporalComponent()) {
//    tp.print();
//  }
//  std::cout << std::endl;
  
//  std::vector<bool> bitArr;
  
//  std::vector<bool> b1;
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(false);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(false);
//  b1.push_back(true);
//  b1.push_back(false);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(false);
//  b1.push_back(true);
//  b1.push_back(true);
//  b1.push_back(false);
//  b1.push_back(true);
//  b1.push_back(false);
//  
//  Binary bin1(b1);
//  bin1.print();
//  std::cout << bin1.getLength() << std::endl;
//  for (auto c: bin1.getBinary()) {
//    std::cout << "(" << (int)c << ")";
//  }
//  std::cout << std::endl;
//  FileWriter fw0("/Users/songrenchu/Develop/bin1.txt", true);
//  bin1.store(fw0);
  
//  FileReader fr0("/Users/songrenchu/Develop/bin1.txt", true);
//  Binary bin2(fr0);
//  bin2.print();
//  std::cout << bin2.getLength() << std::endl;
//  for (auto c: bin2.getBinary()) {
//    std::cout << "(" << (int)c << ")";
//  }
//  std::cout << std::endl;
//  
//  Binary bin3(bin2.getLength(), bin2.getBinary());
//  bin3.print();
//  std::cout << bin3.getLength() << std::endl;
//  for (auto c: bin3.getBinary()) {
//    std::cout << "(" << (int)c << ")";
//  }
//  std::cout << std::endl;
//  
//  Binary bin4 = bin3;
//  
//  bin4.print();
//  std::cout << bin4.getLength() << std::endl;
//  for (auto c: bin4.getBinary()) {
//    std::cout << "(" << (int)c << ")";
//  }
//  std::cout << std::endl;
//  bin4.getBitArray(bitArr);
//  for (auto c: bitArr) {
//    std::cout << c << " ";
//  }
//  std::cout << std::endl;
  
//  Timer t1, t2;
//  t1.start(); t2.start();
//  for (int i = 0; i < 100000000; i++) {
//    
//  }
//  t1.resume();
//
//  t2.pause();
//  for (int i = 0; i < 100000000; i++) {
//    
//  }
//
//  t2.resume();
//  for (int i = 0; i < 100000000; i++) {
//    
//  }
//  
//  std::cout << t1.getMilliSeconds() << std::endl;
//  std::cout << t1.getSeconds() << std::endl;
//  std::cout << t1.getSystemClockDuration() << std::endl;
//  
//  std::cout << t2.getMilliSeconds() << std::endl;
//  std::cout << t2.getSeconds() << std::endl;
//  std::cout << t2.getSystemClockDuration() << std::endl;
  
//    std::vector<bool> b1;
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(false);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(false);
//    b1.push_back(true);
//    b1.push_back(false);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(false);
//    b1.push_back(true);
//    b1.push_back(true);
//    b1.push_back(false);
//    b1.push_back(true);
//    b1.push_back(false);
//    Binary spatial(b1);
//    std::vector<TemporalPair> temporal;
//    temporal.emplace_back(TemporalPair(0, 0));
//    temporal.emplace_back(TemporalPair(1, 1.5));
//    temporal.emplace_back(TemporalPair(2, 2));
//    temporal.emplace_back(TemporalPair(3, 5));
//    temporal.emplace_back(TemporalPair(4, 5.1));
//  
//    PRESSCompressedTrajectory pTraj(spatial, temporal);
//    pTraj.print();
//    std::cout << pTraj.getTemporalLength() << std::endl;
//    FileWriter fw0("/Users/songrenchu/Develop/pctraj1.txt", true);
//    FileWriter fw1("/Users/songrenchu/Develop/pctraj2.txt", true);
//  //  FileWriter fw2("/Users/songrenchu/Develop/ptraj3.txt", false);
//  //  FileWriter fw3("/Users/songrenchu/Develop/ptraj4.txt", false);
//    pTraj.store(fw0,fw1);
  //  pTraj.store(fw2,fw3);
//    FileReader fr0("/Users/songrenchu/Develop/pctraj1.txt", true);
//    FileReader fr1("/Users/songrenchu/Develop/pctraj2.txt", true);
////    FileReader fr2("/Users/songrenchu/Develop/ptraj3.txt", false);
////    FileReader fr3("/Users/songrenchu/Develop/ptraj4.txt", false);
////  
//    PRESSCompressedTrajectory pTraj1(fr0, fr1);
//    pTraj1.print();
////    std::cout << pTraj1.getSpatialLength() << std::endl;
//    std::cout << pTraj1.getTemporalLength() << std::endl;
////    for (auto id: pTraj1.getSpatialComponent()) {
////      std::cout << id << " ";
////    }
//    pTraj1.getSpatialComponent().print();
//    for (auto tp: pTraj1.getTemporalComponent()) {
//      tp.print();
//    }
//    std::cout << std::endl;
////
////    PRESSTrajectory pTraj2(fr2, fr3);
////    pTraj2.print();
////    std::cout << pTraj2.getSpatialLength() << std::endl;
////    std::cout << pTraj2.getTemporalLength() << std::endl;
////    for (auto id: pTraj2.getSpatialComponent()) {
////      std::cout << id << " ";
////    }
////    std::cout << std::endl;
////    for (auto tp: pTraj2.getTemporalComponent()) {
////      tp.print();
////    }
////    std::cout << std::endl;
  
  return 0;
}
