cmake_minimum_required(VERSION 3.0)
project(wueswu)
set(PYLON_ROOT ~/Programming/C/libs/pylon5)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -O3")

include_directories(${PYLON_ROOT}/include kissfft include)
add_subdirectory(kissfft)
link_directories(${PYLON_ROOT}/lib64)

set(GRAB_SOURCE_FILES
    ConfigDump.h
    ContextSaver.h
    Grab.cpp)

add_executable(grab ${GRAB_SOURCE_FILES})
target_link_libraries(grab
        pylonbase
        GenApi_gcc_v3_0_Basler_pylon_v5_0
        GCBase_gcc_v3_0_Basler_pylon_v5_0
        Log_gcc_v3_0_Basler_pylon_v5_0
        MathParser_gcc_v3_0_Basler_pylon_v5_0
        XmlParser_gcc_v3_0_Basler_pylon_v5_0
        NodeMapData_gcc_v3_0_Basler_pylon_v5_0
        log4cpp_gcc_v3_0_Basler_pylon_v5_0
        gxapi
        pylon_TL_camemu
        pylon_TL_gige
        pylon_TL_gtc
        pylon_TL_usb
        pylonutility
        uxapi
        pthread
        dl
        m
        gcc_s
        rt)


if(COMPILE_GUI)
    if(WIN32)
        add_definitions(-DPYLON_WIN_BUILD)
    else()
        message(FATAL_ERROR "COMPILE_GUI is windows-only")
    endif()
endif()

set(WORK_SOURCE_FILES
        Work.cc
        HeartRateEstimator.cc
        HeartRateEstimator.h
        MockContextProvider.cc
        MockContextProvider.h
        AcquisitionContext.h
        CameraContextProvider.cc
        CameraContextProvider.h
        ContextSaver.h
        FirFilter.h
        KalmanFilter.h
        ResultAverage.h
        Config.cc
        Config.h)


add_executable(project ${WORK_SOURCE_FILES})
target_link_libraries(project
        pylonbase
        GenApi_gcc_v3_0_Basler_pylon_v5_0
        GCBase_gcc_v3_0_Basler_pylon_v5_0
        Log_gcc_v3_0_Basler_pylon_v5_0
        MathParser_gcc_v3_0_Basler_pylon_v5_0
        XmlParser_gcc_v3_0_Basler_pylon_v5_0
        NodeMapData_gcc_v3_0_Basler_pylon_v5_0
        log4cpp_gcc_v3_0_Basler_pylon_v5_0
        gxapi
        pylon_TL_camemu
        pylon_TL_gige
        pylon_TL_gtc
        pylon_TL_usb
        pylonutility
        uxapi
        pthread
        dl
        m
        gcc_s
        rt
        kissfft)
