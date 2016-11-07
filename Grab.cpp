#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include <pylon/usb/BaslerUsbInstantCamera.h>

#include "ImageSaver.h"
#include "ConfigDump.h"

typedef Pylon::CBaslerUsbInstantCamera Camera_t;

/*
 * camera.ChunkSelector.FrameCounter
 */


#undef PYLON_WIN_BUILD
using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

static size_t imageCount = 0;

void grabSucceeded(Pylon::CGrabResultPtr ptrGrabResult)
{
	std::cout << ptrGrabResult->GetWidth() << "x" << ptrGrabResult->GetHeight() << std::endl;
	const uint8_t* pImageBuffer = static_cast<uint8_t *>(ptrGrabResult->GetBuffer());

	long sum = 0;
	long cnt = 0;
	for (size_t row = 0; row < ptrGrabResult->GetWidth(); row++)
	{
		for (size_t col = (row + 1) % 2; col < ptrGrabResult->GetHeight(); col += 2)
		{
			cnt++;
			sum += pImageBuffer[row * ptrGrabResult->GetHeight() + col];
		}
	}
	std::cout << "sum: " << sum << " cnt: " << cnt << " result: " << sum / cnt << std::endl;
}

void grabFailed(Pylon::CGrabResultPtr ptrGrabResult)
{
	std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
}

void waitExit()
{
	std::cerr << std::endl << "Press Enter to exit." << std::endl;

	while (std::cin.get() != '\n')
	{
	}
}

void setPixelFormat(Pylon::CInstantCamera& camera)
{
//	try
//	{
//		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
//		if (IsAvailable(pixelFormat->GetEntryByName("RGB8packed")))
//		{
//			pixelFormat->FromString("RGB8packed");
//		} else
//		{
//			std::cout << "packed not available" << std::endl;
//		}
//	}
//	catch (...)
//	{
//		std::cerr << "sumting went wong" << std::endl;
//	}
}



int main(int argc, char* argv[])
{
	int exitCode = 0;
	ImageSaver imageSaver;

	Pylon::PylonInitialize();

	try
	{

		Camera_t camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		
		camera.Open();
		
		camera.Width = 320;
		camera.Height = 240;
		//		Pylon::CInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
//		std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;


		// debug 
		ConfigDump::dump(camera);
		
		setPixelFormat(camera);

		const size_t imagesToGrab = 1000;
		camera.MaxNumBuffer = 100;
		camera.StartGrabbing(imagesToGrab);

		Pylon::CGrabResultPtr ptrGrabResult;

		while (camera.IsGrabbing())
		{
			camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

			if (ptrGrabResult->GrabSucceeded())
			{
				grabSucceeded(ptrGrabResult);
//				imageSaver.save(ptrGrabResult);

#ifdef PYLON_WIN_BUILD
				// Display the grabbed image.
				Pylon::DisplayImage(1, ptrGrabResult);
#endif
			}
			else
			{
				grabFailed(ptrGrabResult);
			}
		}
	}
	catch (const GenericException& e)
	{
		std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
		exitCode = 1;
	}

	waitExit();
	Pylon::PylonTerminate();
	return exitCode;
}
