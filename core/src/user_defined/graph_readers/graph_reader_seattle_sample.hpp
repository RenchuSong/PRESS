//
//  graph_reader_seattle_sample.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef graph_reader_seattle_sample_hpp
#define graph_reader_seattle_sample_hpp

#include "./graph_reader_base.hpp"

class GraphReaderSeattleSample: public GraphReaderBase {
public:
  void readGraphImpl(const std::string& fileName, Graph& graph);
};

#endif /* graph_reader_seattle_sample_hpp */
