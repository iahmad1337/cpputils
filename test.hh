#pragma once
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

namespace utils::test {

struct TimeStat {
  int n;
  double avg;
  double min;
  double max;
};

// Measure statistics of execution time of pure procedure (in microseconds)
template<typename F>
TimeStat MeasureTime(F f, int repetitions = 10) {
  assert(repetitions > 0);
  std::vector<double> times(repetitions);
  for (int i = 0; i < repetitions; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    times[i] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
  }
  return {
    .n = repetitions,
    .avg = std::accumulate(times.begin(), times.end(), 0.0) / repetitions,
    .min = *std::min_element(times.begin(), times.end()),
    .max = *std::max_element(times.begin(), times.end()),
  };
}

}  // namespace utils::test
