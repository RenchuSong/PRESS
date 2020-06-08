//
//  temporal_compressor.hpp
//  press-v3
//
//  Created by Renchu Song on 5/31/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef temporal_compressor_hpp
#define temporal_compressor_hpp

#include <vector>

#include "../topology/auxiliaries.hpp"

class TemporalCompressor {
public:
  TemporalCompressor();
  // Bounded temporal compression.
  void boundedTemporalCompression(
    const std::vector<TemporalPair>& temporal,
    std::vector<TemporalPair>& result,
    float tsnd,
    float nstd
  );
};

#endif /* temporal_compressor_hpp */
