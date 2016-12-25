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

class CameraContextProvider : public ContextProvider {

public:
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
