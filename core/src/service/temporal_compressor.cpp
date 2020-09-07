//
//  temporal_compressor.cpp
//  press-v3
//
//  Created by Renchu Song on 5/31/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <algorithm> 

#include "temporal_compressor.hpp"
#include "../util/helper.hpp"

TemporalCompressor::TemporalCompressor() { }

// Bounded temporal compression.
void TemporalCompressor::boundedTemporalCompression(
  const std::vector<TemporalPair>& temporal,
  std::vector<TemporalPair>& result,
  double tsnd,
  double nstd
) const {
  result.clear();

  // Empty temporal sequence.
  size_t len = temporal.size();
  if (len == 0) {
    return;
  }

  // Always add starting pair to result.
  result.emplace_back(temporal.at(0));

  // Adding all the important pairs to result.
  double lower = -1e100, higher = 1e100;
  TemporalPair anchor = temporal.at(0);
  for (int i = 1; i < temporal.size(); i++) {
    auto point = temporal.at(i);
    double direction = slope(anchor.t, anchor.dist, point.t, point.dist);
    if (direction < lower || direction > higher) {
      result.emplace_back(temporal.at(i - 1));
      lower = -1e100;
      higher = 1e100;
      anchor = temporal.at(i - 1);
    }
    lower = std::max(lower, slope(anchor.t, anchor.dist, point.t, point.dist - tsnd));
    higher = std::min(higher, slope(anchor.t, anchor.dist, point.t, point.dist + tsnd));
    lower = std::max(lower, slope(anchor.t, anchor.dist, point.t + nstd, point.dist));
    higher = std::min(higher, slope(anchor.t, anchor.dist, point.t - nstd, point.dist));
  }

  // Always add ending pair to result.
  if (len > 1) {
    result.emplace_back(temporal.at(len - 1));
  }
}
