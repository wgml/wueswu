#ifndef WUESWU_CAMERACONTEXTPROVIDER_H
#define WUESWU_CAMERACONTEXTPROVIDER_H

#include <pylon/PylonIncludes.h>

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pylon/usb/BaslerUsbInstantCamera.h>
#include "HeartRateEstimator.h"
#include "ContextSaver.h"

using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

/**
 * Offers context acquired from basler camera.
 * Requires camera access on application startup.
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */
class CameraContextProvider : public ContextProvider {

public:
  /**
   * Constructor for camera context provider. Camera initialization is not performed
   * on construction.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param config configuration structure.
   */
  CameraContextProvider(const Config &config)
      : configuration(config){}

  void run() override;

private:
  ContextSaver contextSaver;
  const Config configuration;

  void init(Pylon::CBaslerUsbInstantCamera &);

  void setPixelFormat(Pylon::CBaslerUsbInstantCamera &);

  void grabSucceeded(Pylon::CGrabResultPtr resultPtr);

  void grabFailed(Pylon::CGrabResultPtr resultPtr, unsigned int idx);
};

#endif //WUESWU_CAMERACONTEXTPROVIDER_H
