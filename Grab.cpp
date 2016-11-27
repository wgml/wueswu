#include <pylon/PylonIncludes.h>
#include <chrono>
#include <thread>

//#define PYLON_WIN_BUILD
#undef PYLON_WIN_BUILD

#ifdef PYLON_WIN_BUILD

#include <pylon/PylonGUI.h>

#endif

#include <pylon/usb/BaslerUsbInstantCamera.h>

#include "ContextSaver.h"
#include "ConfigDump.h"

typedef Pylon::CBaslerUsbInstantCamera Camera_t;

using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

void grabFailed(Pylon::CGrabResultPtr ptrGrabResult, int img_idx) {
  std::cout << "#" << img_idx << " " << "Error: " << ptrGrabResult->GetErrorCode() << " "
            << ptrGrabResult->GetErrorDescription() << std::endl;
}

void waitExit() {
  std::cerr << std::endl << "Press Enter to exit." << std::endl;
  while (std::cin.get() != '\n') {
  }
}

void setPixelFormat(Pylon::CInstantCamera &camera) {
  try {
    CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
    if (IsAvailable(pixelFormat->GetEntryByName("BayerBG8"))) {
      pixelFormat->FromString("BayerBG8");
      std::cout << "set pixel format to BayerBG8" << std::endl;
    } else {
      std::cout << "BayerBG8 not available" << std::endl;
    }
  }
  catch (...) {
    std::cerr << "sumting went wong" << std::endl;
  }
}

int main(int argc, char *argv[]) {
  int exitCode = 0;
  ContextSaver contextSaver;

  Pylon::PylonInitialize();

  try {

    Camera_t camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());

    camera.Open();

    size_t images_to_grab = 10000;

    int maxWidth = 2040;
    int maxHeight = 1086;
    int desiredWidth = 800;
    int desiredHeight = 600;
    int offsetX = (((maxWidth - desiredWidth) / 2) / 2) * 2;
    int offsetY = (((maxHeight - desiredHeight) / 2) / 2) * 2;
    std::cerr << desiredWidth << "x" << desiredHeight << "+" << offsetX << "+" << offsetY
              << std::endl;

    camera.Width = desiredWidth;
    camera.Height = desiredHeight;
    camera.OffsetX = offsetX;
    camera.OffsetY = offsetY;
    camera.ExposureTime = 10000;
    setPixelFormat(camera);
    ConfigDump::dump(camera);

    camera.MaxNumBuffer = 100;
    camera.StartGrabbing(images_to_grab);

    Pylon::CGrabResultPtr ptrGrabResult;

    for (int cur_img_idx = 0; cur_img_idx < images_to_grab && camera.IsGrabbing(); cur_img_idx++) {
      camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

      if (ptrGrabResult->GrabSucceeded()) {
        contextSaver.save_context(ptrGrabResult, cur_img_idx);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif
      } else {
        grabFailed(ptrGrabResult, cur_img_idx);
      }
    }
  }
  catch (const GenericException &e) {
    std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
    exitCode = 1;
  }

  waitExit();
  Pylon::PylonTerminate();
  return exitCode;
}
