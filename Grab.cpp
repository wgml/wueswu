#include <pylon/PylonIncludes.h>
#undef PYLON_WIN_BUILD

#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include <pylon/usb/BaslerUsbInstantCamera.h>

#include "ContextSaver.h"
#include "ConfigDump.h"

typedef Pylon::CBaslerUsbInstantCamera Camera_t;

using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

static size_t imageCount = 0;

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
	try
	{
		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
		if (IsAvailable(pixelFormat->GetEntryByName("BayerBG8")))
		{
			pixelFormat->FromString("BayerBG8");
			std::cout << "set pixel format to BayerBG8" << std::endl;
		} else
		{
			std::cout << "BayerBG8 not available" << std::endl;
		}
	}
	catch (...)
	{
		std::cerr << "sumting went wong" << std::endl;
	}
}



int main(int argc, char* argv[])
{
	int exitCode = 0;
	ContextSaver contextSaver;

	Pylon::PylonInitialize();

	try
	{

		Camera_t camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		
		camera.Open();
		
		camera.Width = 320;
		camera.Height = 240;
		
		setPixelFormat(camera);
		ConfigDump::dump(camera);

		const size_t imagesToGrab = 1000;
		camera.MaxNumBuffer = 100;
		camera.StartGrabbing(imagesToGrab);

		Pylon::CGrabResultPtr ptrGrabResult;

		while (camera.IsGrabbing())
		{
			camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

			if (ptrGrabResult->GrabSucceeded())
			{
				contextSaver.save_context(ptrGrabResult);
#ifdef PYLON_WIN_BUILD
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
