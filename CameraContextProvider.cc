
#include "CameraContextProvider.h"

void CameraContextProvider::run() {
  try {
    Pylon::PylonInitialize();
    Pylon::CBaslerUsbInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    init(camera);

    camera.StartGrabbing(configuration.camera.images_to_grab);

    Pylon::CGrabResultPtr ptrGrabResult;

    for (unsigned int idx = 0;
         idx < configuration.camera.images_to_grab && camera.IsGrabbing(); idx++) {
      camera.RetrieveResult(10000, ptrGrabResult, Pylon::TimeoutHandling_Return);

      if (ptrGrabResult->GrabSucceeded()) {
        grabSucceeded(ptrGrabResult, idx);
      } else {
        grabFailed(ptrGrabResult, idx);
      }
    }
    estimator->stop();
    Pylon::PylonTerminate();
  } catch (...) {
    std::cout << "Unexpected exception. Is basler camera accessible?" << std::endl;
    estimator->stop();
  }

}

void CameraContextProvider::init(Pylon::CBaslerUsbInstantCamera &camera) {
  camera.Open();

  int maxWidth = 2040;
  int maxHeight = 1086;
  int offsetX = (((maxWidth - configuration.camera.width) / 2) / 2) * 2;
  int offsetY = (((maxHeight - configuration.camera.height) / 2) / 2) * 2;
  std::cerr << configuration.camera.width << "x" << configuration.camera.height << "+" << offsetX
            << "+" << offsetY << " e=" << configuration.camera.exposure_time << "f="
            << configuration.camera.fps << std::endl;

  camera.Width = configuration.camera.width;
  camera.Height = configuration.camera.height;
  camera.OffsetX = offsetX;
  camera.OffsetY = offsetY;
  camera.ExposureTime = configuration.camera.exposure_time;
  camera.AcquisitionFrameRateEnable = true;
  camera.AcquisitionFrameRate = configuration.camera.fps;
  camera.Gain = configuration.camera.gain;
  camera.MaxNumBuffer = 100;
  setPixelFormat(camera);
}

void CameraContextProvider::setPixelFormat(Pylon::CBaslerUsbInstantCamera &camera) {
  try {
    CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
    if (IsAvailable(pixelFormat->GetEntryByName("BayerBG8"))) {
      pixelFormat->FromString("BayerBG8");
      std::cerr << "set pixel format to BayerBG8" << std::endl;
    } else {
      std::cerr << "BayerBG8 not available" << std::endl;
    }
  }
  catch (...) {
    std::cerr << "Unexpected exception caught" << std::endl;
  }
}

void CameraContextProvider::grabSucceeded(Pylon::CGrabResultPtr resultPtr, unsigned int idx) {
#ifdef PYLON_WIN_BUILD
  Pylon::DisplayImage(1, resultPtr);
#endif

  auto width = resultPtr->GetWidth();
  auto height = resultPtr->GetHeight();

  std::cerr << "#" << idx << " " << width << "x" << height << std::endl;

  const uint8_t *pImageBuffer = static_cast<uint8_t *>(resultPtr->GetBuffer());

  unsigned long sum = 0;
  unsigned long cnt = 0;
  size_t offset_x = resultPtr->GetOffsetX();
  for (size_t row = 0; row < height; row++) {
    for (size_t col = (row + 1) % 2; col < width; col += 2) {
      cnt++;
      sum += pImageBuffer[row * (width + offset_x) + col];
    }
  }

  std::cerr << "#" << idx << " " << "sum: " << sum << " cnt: " << cnt << " result: " << sum / cnt
            << std::endl;

  auto now = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  AcquisitionContext ctx{now, sum, cnt};
  estimator->notify(ctx);
  contextSaver.save_context(now, sum, cnt);
}

void CameraContextProvider::grabFailed(Pylon::CGrabResultPtr resultPtr, unsigned int idx) {
  std::cerr << "#" << idx << " " << "Error: " << resultPtr->GetErrorCode() << " "
            << resultPtr->GetErrorDescription() << std::endl;

}
