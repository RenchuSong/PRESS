//
//  query_processor.hpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef query_processor_hpp
#define query_processor_hpp

#include "../topology/auxiliary.hpp"
#include "../topology/geospatial_structures.hpp"
#include "../topology/graph.hpp"
#include "../topology/huffman.hpp"
#include "../topology/trajectory.hpp"

class QueryProcessor {
public:
  // WhereAt query on original PRESS trajectory.
  void whereAt(
    const Graph& graph,
    const PRESSTrajectory& press,
    double timeStamp,
    Point2D& result
  );
  // WhenAt query on original PRESS trajectory.
  double whenAt(
    const Graph& graph,
    const PRESSTrajectory& press,
    const Point2D& position
  );
  // Range query on original PRESS trajectory.
  bool range(
    const Graph& graph,
    const PRESSTrajectory& press,
    double t1,
    double t2,
    const Point2D& lowerBound,
    const Point2D& upperBound
  );
  // WhereAt query on compressed PRESS trajectory.
  void whereAt(
    const Graph& graph,
    const SPTable& spTable,
    const Huffman& huffman,
    const ACAutomaton& acAutomaton,
    const Auxiliary& auxiliary,
    const PRESSCompressedTrajectory& press,
    double timeStamp,
    Point2D& result
  );
  // WhenAt query on compressed PRESS trajectory.
  double whenAt(
    const Graph& graph,
    const SPTable& spTable,
    const Huffman& huffman,
    const ACAutomaton& acAutomaton,
    const Auxiliary& auxiliary,
    const PRESSCompressedTrajectory& press,
    const Point2D& position
  );
  // Range query on compressed PRESS trajectory.
  bool range(
    const Graph& graph,
    const SPTable& spTable,
    const Huffman& huffman,
    const ACAutomaton& acAutomaton,
    const Auxiliary& auxiliary,
    const PRESSCompressedTrajectory& press,
    double t1,
    double t2,
    const Point2D& lowerBound,
    const Point2D& upperBound
  );
};

#endif /* query_processor_hpp */
