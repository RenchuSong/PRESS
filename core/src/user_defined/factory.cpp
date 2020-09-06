//
//  factory.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "factory.hpp"
#include <cassert>
#include <string>

#include "./graph_readers/graph_reader_seattle_sample.hpp"
// [USER DEFINE]: Include your own graph reader types here.

std::shared_ptr<GraphReaderBase> Factory::getGraphReader(GraphReaderType grType) {
  switch (grType) {
    case GraphReaderType::SEATTLE_SAMPLE: {
      return std::make_shared<GraphReaderSeattleSample>();
    }
    // [USER DEFINE]: Construct your own graph reader types here.
  }
  
  // Should not go to this point. Check above if you handled all graph reader types.
  assert(false);
}
