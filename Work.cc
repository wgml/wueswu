#include <thread>
#include <pylon/PylonIncludes.h>

#include "HeartRateEstimator.h"
#include "MockContextProvider.h"
#include "CameraContextProvider.h"

int main(int argc, char *argv[]) {
  bool mock = argc > 1;
  std::shared_ptr<ContextProvider> provider;
  if (mock) {
    provider = std::make_shared<MockContextProvider>(
        "/home/vka/Programming/C/workspace/wueswu/data/ania_biale_6_exp=30000.txt", true);
  } else {
    Pylon::PylonInitialize();
    Pylon::IPylonDevice *pDevice;
    try {
      pDevice = Pylon::CTlFactory::GetInstance().CreateFirstDevice();
    } catch (GenICam::GenericException &e) {
      std::cerr << e.what() << std::endl;
      return 2;
    }
    if (pDevice == nullptr) {
      std::cerr << "Basler camera is not accessible." << std::endl;
      return 1;
    }
    Pylon::CBaslerUsbInstantCamera camera(pDevice);
    provider = std::make_shared<CameraContextProvider>(camera, 10000,
                                                       800, 600, 30000);
  }

  HeartRateEstimator estimator{*provider};
  estimator.init();

  std::thread estimatorThread([&estimator] {estimator.run();});
  std::thread providerThread([&] { provider->run(); });
  providerThread.join();
  estimatorThread.join();
  if (!mock)
    Pylon::PylonTerminate();
  return 0;
}