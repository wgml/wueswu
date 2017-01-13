#include <thread>
#include <pylon/PylonIncludes.h>

#include "HeartRateEstimator.h"
#include "MockContextProvider.h"
#include "CameraContextProvider.h"

int main(int argc, char *argv[]) {

  auto config = parse_config(argc, argv);
  std::cerr << "Received configuration" << std::endl << config << std::endl;
  std::shared_ptr<ContextProvider> provider;
  if (config.camera.mock) {
    provider = std::make_shared<MockContextProvider>(
        config);
  } else {
    provider = std::make_shared<CameraContextProvider>(config);
  }
  HeartRateEstimator estimator{provider, config};
  estimator.init();
  std::thread providerThread([&provider] { provider->run(); });
  std::thread estimatorThread([&estimator] { estimator.run(); });

  providerThread.join();
  estimatorThread.join();

  std::cout << "Shutting down peacefully." << std::endl;
  return 0;
}