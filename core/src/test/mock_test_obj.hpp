//
//  mock_test_obj.hpp
//  press-v3
//
//  Created by Renchu Song on 7/12/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef mock_test_obj_hpp
#define mock_test_obj_hpp

#include "../topology/ac_automaton.hpp"
#include "../topology/graph.hpp"
#include "../topology/huffman.hpp"
#include "../topology/sp_table.hpp"
#include "../topology/trajectory.hpp"

class MockTestObjects {
public:
  MockTestObjects();
  ~MockTestObjects();
  ACAutomaton* acAutomaton;
  Graph* graph;
  Huffman* huffman;
  SPTable* spTable;
};

#endif /* mock_test_obj_hpp */
