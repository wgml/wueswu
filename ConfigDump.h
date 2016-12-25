#pragma once

#include <pylon/PylonIncludes.h>

struct ConfigDump {
  /**
   * Dump camera configuration into file for future investigation.
   * Create date:
   *    16/10/30
   * Last modification date:
   *    16/10/30
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param camera Device context on which dump is performed
   */
  static void dump(Pylon::CInstantCamera &camera) {
    Pylon::CFeaturePersistence::Save("NodeMap.pfs", &camera.GetNodeMap());
  }
};
