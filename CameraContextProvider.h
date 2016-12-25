#ifndef WUESWU_CAMERACONTEXTPROVIDER_H
#define WUESWU_CAMERACONTEXTPROVIDER_H

#include <pylon/PylonIncludes.h>

#ifdef PYLON_WIN_BUILD
#warning "PYLON_WIN_BUILD is defined. I'll compile with acquisition result presented. It might limit algorithm capabilities."
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
   * @param imagesToGrab number of images should be acquired. Execution is stopped when desired number is reached.
   * @param width width of roi, centered
   * @param height height of roi, centered
   * @param exposureTime exposure time for camera
   */
  CameraContextProvider(size_t imagesToGrab,
                        int width, int height, int exposureTime)
      : imagesToGrab(imagesToGrab), width(width), height(height), exposureTime(exposureTime) {}

  void run() override;

private:
  ContextSaver contextSaver;
  size_t imagesToGrab;
  int width;
  int height;
  int exposureTime;

  void init(Pylon::CBaslerUsbInstantCamera &, int width, int height, int exposureTime);

  void setPixelFormat(Pylon::CBaslerUsbInstantCamera &);

  void grabSucceeded(Pylon::CGrabResultPtr resultPtr, unsigned int idx);

  void grabFailed(Pylon::CGrabResultPtr resultPtr, unsigned int idx);
};

#endif //WUESWU_CAMERACONTEXTPROVIDER_H
