
#include <iostream>
#include <thread>
#include <fstream>
#include "MockContextProvider.h"

void MockContextProvider::run() {
  const size_t samples = buffer.size();
  for (unsigned long i = 0; i < samples || runInfinitely; i++) {
    auto now = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    AcquisitionContext ctx = buffer[i % samples];
    ctx.timestamp = now;
    estimator->notify(ctx);
    std::this_thread::sleep_for(std::chrono::microseconds{us_delay});
  }
  estimator->stop();
}

void MockContextProvider::loadFromFile(std::string filename) {
  std::ifstream infile(filename);
  unsigned long sum, count;
  unsigned long long micros, prev_micros = 0, sum_diffs = 0;
  while (infile >> micros >> sum >> count) {
    buffer.push_back(AcquisitionContext{std::chrono::microseconds{0}, sum, count});
    if (prev_micros == 0) {
      prev_micros = micros;
    } else {
      sum_diffs += (micros - prev_micros);
      prev_micros = micros;
    }
  }
  us_delay = static_cast<unsigned long>(sum_diffs / (buffer.size() - 1));
  std::cout << "Camera will be mocked with " << buffer.size() << " samples and with " << us_delay
            << "us delay between samples."
            << " It " << (runInfinitely ? "will" : "won't") << " run indefinitely." << std::endl;
}
