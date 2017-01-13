
#ifndef WUESWU_MOCKCONTEXTPROVIDER_H
#define WUESWU_MOCKCONTEXTPROVIDER_H

#include <vector>
#include "HeartRateEstimator.h"
#include "Config.h"

/**
 * Uses persisted data to mock camera provider.
 * Estimator should provide exact result for original and mocked data.
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */
class MockContextProvider : public ContextProvider {
public:

  /**
   * Provides context from given file with desired signal frequency.
   * Can run infinitely or one-time.
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param filename File containing context entries
   * @param runInfinitely flag determining if data from file should be looped
   */
  MockContextProvider(const Config &configuration)
      : runInfinitely(configuration.camera.mock_infinitely) {
    loadFromFile(configuration.camera.mock_filename);
  }

  void run() override;

private:
  void loadFromFile(std::string filename);

  std::vector<AcquisitionContext> buffer;
  unsigned long us_delay;
  bool runInfinitely;
};

#endif //WUESWU_MOCKCONTEXTPROVIDER_H
