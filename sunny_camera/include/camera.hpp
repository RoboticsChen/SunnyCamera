#ifndef CAMERA_H
#define CAMERA_H

#ifdef WIN32

#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <psapi.h>

#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0

#elif defined LINUX

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>

#endif
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <opencv2/opencv.hpp>

#include "tof_dev_sdk.h"
#include "tof_sdk_typedef.h"

typedef struct tagIntrinsic {
  cv::Mat rgb_INTRINSIC;
  cv::Mat depth_INTRINSIC;
} Intrinsic;

typedef struct tagDistortion {
  cv::Mat rgb_DISTORTION;
  cv::Mat depth_DISTORTION;
} Distortion;

//
typedef struct tagStreamCallBack {
  TofDeviceInfo struCaps;
  bool inited = false;

  UINT32 tof_frame_count;
  TofFrameData tofFrameData;
  TofModuleLensParameterV20 tofDeviceParam;
  UINT32 rgb_frame_count;
  RgbFrameData rgbFrameData;
  RgbModuleLensParameterV20 rgbDeviceParam;

} StreamCallBack;

typedef struct tagDevsDataList {
  UINT32 devIndex;
  SCHAR deviceId[64];

  HTOFD hTofD;
  TofDeviceInfo struCaps;
  TOF_MODE tofMode;

  StreamCallBack frameData;

} DevsDataList;

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp);
#endif
// unsigned long long Utils_GetTickCount(void);
void Utils_SaveBufToFile(void *pData, const unsigned int nDataLen,
                         const char *pFile, const bool bAppend);
float CalCenterPointDataZAvg(PointData *pPointData, const UINT32 width,
                             const UINT32 height);
const char *StringColorFormat(const COLOR_FORMAT type);
const char *TofMode2Str(const TOF_MODE mode);
bool SaveDepthText(float *pDepthData, const UINT32 width, const UINT32 height,
                   char *pTxtFile, const bool bWH);
bool SavePointDataXYZText(PointData *pPointData, const UINT32 width,
                          const UINT32 height, char *pTxtFile);
bool SaveColorPointDataXYZText(PointData *pPointData, RgbDData *pRgbD,
                               const UINT32 width, const UINT32 height,
                               char *pTxtFile);
bool SavePixelCoordText(PixelCoordData *pPixelCoord, const unsigned int width,
                        const unsigned int height, char *pTxtFile);
bool SaveFlagText(UINT8 *pFlag, const UINT32 width, const UINT32 height,
                  char *pTxtFile, const bool bWH);
bool SavePolarDataGroupText(FLOAT32 *pDepth, FLOAT32 *pAngle,
                            UINT8 *pConfidence, const UINT32 nCnt,
                            char *pTxtFile);
void CaptureLslFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     LslFrameData *lslFrameData);
void CaptureTofFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     TofFrameData *tofFrameData);
void CaptureRgbFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     RgbFrameData *rgbFrameData);
void CallBackTofDeviceStatus(TOFDEV_STATUS tofDevStatus, void *pUserData);
void PrintDevInfo(TofDeviceInfo *pTofDeviceInfo);

void CallBackTofStream(TofFrameData *tofFrameData, void *pUserData);
void CallBackRgbStream(RgbFrameData *rgbFrameData, void *pUserData);
bool OpenStream(HTOFD hTofD, TofDeviceCapability *pCaps,
                StreamCallBack *pStreamParam);
void CloseStream(HTOFD hTofD, TofDeviceCapability *pCaps);

void PrintfTofLensParameter(TofModuleLensParameterV20 *pTofLens);
void PrintfRgbLensParameter(RgbModuleLensParameterV20 *pRgbLens);
void PrintfDepthCalRoi(DepthCalRoi *Roi);
void GetOrSetSomeParam(HTOFD hTofD, StreamCallBack &frameData,
                       TofDeviceCapability *pCaps);
void SaveSomeData(HTOFD hTofD, TofDeviceInfo *pCaps, std::string &strSaveDir);
TofDeviceCapability *GetTofModeCaps(TofDeviceInfo &struCaps,
                                    const TOF_MODE tofMode);
void captureThread(DevsDataList &dev, std::string strSaveDir);
bool ChoseDevTofMode(TofDeviceDescriptor *pDevsDesc, const UINT32 devIndex,
                     DevsDataList &chosedDev);
TOF_MODE ChoseTofMode(const UINT32 devIndex, TofDeviceInfo &struCaps);
UINT32 OpenAllDevAndChoseTofMode(TofDeviceDescriptor *pDevsDescList,
                          const UINT32 dev_cnt, DevsDataList *pdevsDataList);
void HandleSignal(int sig);

//------------------------------------------------------------------------

extern bool g_signalCtrlC;         // ctrl-c信号
extern bool g_isConnectionBroken;  // ConnectionBroken信号

extern std::mutex rgbLock;  // 全局锁
extern std::mutex tofLock;  // 全局锁

extern TofDeviceDescriptor *pDevsDescList;
extern int device_num;

#endif