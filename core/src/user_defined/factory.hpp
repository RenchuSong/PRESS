//
//  factory.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef factory_hpp
#define factory_hpp

#include <memory>

#include "./graph_readers/graph_reader_base.hpp"

// Different types of graph readers.
enum GraphReaderType {
  SEATTLE_SAMPLE,
  RR,
  // [USER DEFINE]: Define your own graph reader types here.
};

/**
 * A simple factory pattern interface for users to provide customized implementations of
 * 1. graph reader - read graph from user defined format
 */
class Factory {
public:
  static std::shared_ptr<GraphReaderBase> getGraphReader(GraphReaderType grType);
};

#endif /* factory_hpp */
