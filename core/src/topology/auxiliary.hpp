//
//  auxiliary.hpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef auxiliary_hpp
#define auxiliary_hpp

#include "./graph.hpp"
#include "./sp_table.hpp"

class SPTable;

class Auxiliary {
private:
  size_t nodeNumber;
  // Stores the road network distance between pair of nodes.
  std::vector<std::unordered_map<int, double> > nodePairSPDist;

public:
  Auxiliary(const Graph& graph, const SPTable& spTable);
};

#endif /* auxiliary_hpp */
