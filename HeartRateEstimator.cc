#include <iostream>
#include <thread>
#include "HeartRateEstimator.h"

void HeartRateEstimator::init() {
  provider.subscribe(this);
}

void HeartRateEstimator::run() {
  work = true;
  while (work) {
    std::this_thread::sleep_for(std::chrono::seconds{1});
    std::cerr << "Determined heart rate is " << estimate()
              << ". There are " << data.size() << " samples in context." << std::endl;
  }
}

double HeartRateEstimator::estimate() {
  if (!data.empty())
    return 1.0 * data.back().pixelSum / data.back().pixelCount;
  return -1;
}
