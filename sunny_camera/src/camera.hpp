#ifndef CAMERA_H
#define CAMERA_H

#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#include <psapi.h>
#include <io.h>
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0

#elif defined LINUX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <fcntl.h>

#endif

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <memory>
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>
#include "tof_sdk_typedef.h"
#include "tof_dev_sdk.h"
#include <opencv2/opencv.hpp>

extern bool g_signalCtrlC;   // ctrl-c信号
extern std::mutex frameLock; // 全局锁

typedef struct tagIntrinsic
{
    cv::Mat rgb_INTRINSIC;
    cv::Mat depth_INTRINSIC;
} Intrinsic;

//
typedef struct tagStreamCallBack
{
    TofDeviceInfo struCaps;
    bool inited;

    UINT32 tof_frame_count;
    TofFrameData tofFrameData;
    TofModuleLensParameterV20 tofDeviceParam;
    UINT32 rgb_frame_count;
    RgbFrameData rgbFrameData;
    RgbModuleLensParameterV20 rgbDeviceParam;
    
} StreamCallBack;

extern StreamCallBack frameData;

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp);
#endif
unsigned long long Utils_GetTickCount(void);
void Utils_SaveBufToFile(void *pData, const unsigned int nDataLen, const char *pFile, const bool bAppend);
float CalCenterPointDataZAvg(PointData *pPointData, const UINT32 width, const UINT32 height);
const char *StringColorFormat(const COLOR_FORMAT type);
const char *TofMode2Str(const TOF_MODE mode);
bool SaveDepthText(float *pDepthData, const UINT32 width, const UINT32 height, char *pTxtFile, const bool bWH);
bool SavePointDataXYZText(PointData *pPointData, const UINT32 width, const UINT32 height, char *pTxtFile);
bool SaveColorPointDataXYZText(PointData *pPointData, RgbDData *pRgbD, const UINT32 width, const UINT32 height, char *pTxtFile);
bool SavePixelCoordText(PixelCoordData *pPixelCoord, const unsigned int width, const unsigned int height, char *pTxtFile);
bool SaveFlagText(UINT8 *pFlag, const UINT32 width, const UINT32 height, char *pTxtFile, const bool bWH);
void CaptureTofFrame(const std::string &strDir, const unsigned int nCaptureIndex, TofFrameData *tofFrameData);
void CaptureRgbFrame(const std::string &strDir, const unsigned int nCaptureIndex, RgbFrameData *rgbFrameData);
void CallBackTofDeviceStatus(TOFDEV_STATUS tofDevStatus, void *pUserData);
void fnRgbStream(RgbFrameData *rgbFrameData, void *pUserData);
void PrintDevInfo(TofDeviceInfo *pTofDeviceInfo);
TOF_MODE ChoseTofMode(TofDeviceInfo &struCaps);
bool OpenStream(HTOFD hTofD, TofDeviceCapability *pCaps, StreamCallBack *pStreamParam);
void CloseStream(HTOFD hTofD, TofDeviceCapability *pCaps);
void PrintfTofLensParameter(TofModuleLensParameterV20 *pTofLens);
void PrintfRgbLensParameter(RgbModuleLensParameterV20 *pRgbLens);
void PrintfDepthCalRoi(DepthCalRoi *Roi);
void GetOrSetSomeParam(HTOFD hTofD, TofDeviceCapability *pCaps, int camera_type);
void SaveSomeData(HTOFD hTofD, TofDeviceInfo *pCaps, std::string &strSaveDir);
TofDeviceCapability *GetTofModeCaps(TofDeviceInfo &struCaps, const TOF_MODE tofMode);
void captureThread(HTOFD hTofD, int camera_type, std::string strSaveDir);
// void DoTestDemo(TofDeviceDescriptor *pDevsDesc, std::string &strSaveDir);
UINT32 ChoseDev(const UINT32 dev_cnt);
void HandleSignal(int sig);
#endif