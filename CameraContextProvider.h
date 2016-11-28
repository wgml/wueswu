#ifndef WUESWU_CAMERACONTEXTPROVIDER_H
#define WUESWU_CAMERACONTEXTPROVIDER_H

#include <pylon/PylonIncludes.h>

#undef PYLON_WIN_BUILD
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pylon/usb/BaslerUsbInstantCamera.h>
#include "HeartRateEstimator.h"
#include "ContextSaver.h"

using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

class CameraContextProvider : public ContextProvider {

public:
  CameraContextProvider(Pylon::CBaslerUsbInstantCamera &camera, size_t imagesToGrab,
                        int width, int height, int exposureTime)
      : camera(camera), imagesToGrab(imagesToGrab) {
    init(width, height, exposureTime);
  }

  void init(int width, int height, int exposureTime);

  void run() override;

private:
  ContextSaver contextSaver;
  Pylon::CBaslerUsbInstantCamera &camera;
  size_t imagesToGrab;

  void setPixelFormat();

  void grabSucceeded(Pylon::CGrabResultPtr resultPtr, unsigned int idx);

  void grabFailed(Pylon::CGrabResultPtr resultPtr, unsigned int idx);
};

#endif //WUESWU_CAMERACONTEXTPROVIDER_H
