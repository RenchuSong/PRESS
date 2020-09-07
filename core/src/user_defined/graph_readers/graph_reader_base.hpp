//
//  graph_reader_base.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef graph_reader_base_hpp
#define graph_reader_base_hpp

#include <vector>

#include "../../topology/graph.hpp"

class GraphReaderBase {
public:
  // The method for outside world to call: read graph structure from given filename.
  void readGraph(const std::string& fileName, Graph& graph);
  // Each subclass needs to implement this method and read the nodes and edges from the filename.
  virtual void readGraphImpl(const std::string& fileName, Graph& graph) = 0;

private:
  // Do sanity check of the graph structure, and make sure the graph is valid.
  void sanityCheck(Graph& graph) const;
};

#endif /* graph_reader_base_hpp */
