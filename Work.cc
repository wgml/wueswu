#include <thread>
#include <pylon/PylonIncludes.h>

#include "HeartRateEstimator.h"
#include "MockContextProvider.h"
#include "CameraContextProvider.h"

// i suggest passing argument to main, e.g. C:/Users/vka/Documents/wueswu/Grab/data/ania_biale_6_exp=30000.txt
int main(int argc, char *argv[]) {
  bool mock = true;
  std::shared_ptr<ContextProvider> provider;
  if (mock) {
    provider = std::make_shared<MockContextProvider>(
        std::string(argv[1]), true);
  } else {
    provider = std::make_shared<CameraContextProvider>( 100000, 800, 600, 30000);
  }
  HeartRateEstimator estimator{provider};
  estimator.init();
  std::thread providerThread([&provider] { provider->run(); });
  std::thread estimatorThread([&estimator] {estimator.run(/*tps=*/10);});

  providerThread.join();
  estimatorThread.join();
  if (!mock)
    Pylon::PylonTerminate();
  return 0;
}