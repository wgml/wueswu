#pragma once
#include <pylon/PylonIncludes.h>

struct ConfigDump
{
	static void dump(Pylon::CInstantCamera& camera)
	{
		Pylon::CFeaturePersistence::Save("NodeMap.pfs", &camera.GetNodeMap());
	}
};
