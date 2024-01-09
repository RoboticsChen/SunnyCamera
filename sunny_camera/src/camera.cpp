#include "camera.hpp"

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp) {
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;
  GetLocalTime(&wtm);
  tm.tm_year = wtm.wYear - 1900;
  tm.tm_mon = wtm.wMonth - 1;
  tm.tm_mday = wtm.wDay;
  tm.tm_hour = wtm.wHour;
  tm.tm_min = wtm.wMinute;
  tm.tm_sec = wtm.wSecond;
  tm.tm_isdst = -1;
  clock = mktime(&tm);
  tp->tv_sec = (long)clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;
  return (0);
}
#endif

unsigned long long Utils_GetTickCount(void) {
  unsigned long long tick = 0;

#ifdef WIN32
  // tick = GetTickCount();//实际精度只有15ms左右;
  // 返回的是一个32位的无符号整数，Windows连续运行49.710天后，它将再次从零开始计时;
  // tick =
  // GetTickCount64();//返回一个64位的无符号整数。Windows连续运行5.8亿年后，其计时才会归零;
  // tick = clock();//该程序从启动到函数调用占用CPU的时间, 是C/C++中的计时函数

  // struct timeval tv;
  // gettimeofday(&tv, 0);
  // tick = (tv.tv_sec * 1000 + tv.tv_usec / 1000);

  auto timePoint = std::chrono::steady_clock::now();  // std::chrono::time_point
  tick = std::chrono::duration_cast<std::chrono::milliseconds>(
             timePoint.time_since_epoch())
             .count();

#elif defined LINUX
  // struct timeval tv;
  // gettimeofday(&tv, 0);
  // tick = (tv.tv_sec * 1000 + tv.tv_usec/1000);

  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  tick = (tv.tv_sec * 1000 + tv.tv_nsec / 1000000);

#else
  printf("unknown platform in getting tick cnt, error!\n");
#endif  // WIN32

  return tick;
}

void Utils_SaveBufToFile(void *pData, const unsigned int nDataLen,
                         const char *pFile, const bool bAppend) {
  if ((NULL == pData) || (0 >= nDataLen) || (NULL == pFile)) {
    return;
  }

  FILE *fp = fopen(pFile, (bAppend ? "ab" : "wb"));
  if (NULL == fp) {
    printf("open file(%s) failed, error=%d(%s).\n", pFile, errno,
           strerror(errno));
    return;
  }

  fwrite(pData, 1, nDataLen, fp);
  fclose(fp);
}

float CalCenterPointDataZAvg(PointData *pPointData, const UINT32 width,
                             const UINT32 height) {
  if (NULL == pPointData) {
    return 0;
  }

  const int start_h = (10 < height) ? ((height / 2) - 5) : 0;
  const int end_h = (10 < height) ? ((height / 2) + 5) : (height);
  const int start_w = (10 < width) ? ((width / 2) - 5) : 0;
  const int end_w = (10 < width) ? ((width / 2) + 5) : (width);

  float sum = 0.0;
  int cnt = 0;
  for (int h = start_h; h < end_h; h++) {
    PointData *pTmp = pPointData + h * width;
    for (int w = start_w; w < end_w; w++) {
      if (0.00001 < pTmp[w].z) {
        sum += pTmp[w].z;
        cnt++;
      }
    }
  }

  return ((0 < cnt) ? (sum / cnt) : 0);
}

const char *StringColorFormat(const COLOR_FORMAT type) {
  const char *pStr = "UnknownRGB";

  switch (type) {
    case COLOR_FORMAT_MJPEG:
      pStr = "JPG";
      break;

    case COLOR_FORMAT_H264:
      pStr = "H264";
      break;

    case COLOR_FORMAT_YUV422:
      pStr = "YUV422";
      break;
    case COLOR_FORMAT_YUYV:
      pStr = "YUYV";
      break;
    case COLOR_FORMAT_I420:
      pStr = "I420";
      break;
    case COLOR_FORMAT_YV12:
      pStr = "YV12";
      break;
    case COLOR_FORMAT_NV12:
      pStr = "NV12";
      break;
    case COLOR_FORMAT_NV21:
      pStr = "NV21";
      break;

    case COLOR_FORMAT_BGR:
      pStr = "BGR";
      break;
    case COLOR_FORMAT_RGB:
      pStr = "RGB";
      break;
    case COLOR_FORMAT_BGRA:
      pStr = "BGRA";
      break;
    case COLOR_FORMAT_RGBA:
      pStr = "RGBA";
      break;

    default:
      break;
  }

  return pStr;
}

const char *TofMode2Str(const TOF_MODE mode) {
  const char *pStr = "Unknown";

  switch (mode) {
    case TOF_MODE_STERO_5FPS:
      pStr = "STERO_5FPS";
      break;
    case TOF_MODE_STERO_10FPS:
      pStr = "STERO_10FPS";
      break;
    case TOF_MODE_STERO_15FPS:
      pStr = "STERO_15FPS";
      break;
    case TOF_MODE_STERO_30FPS:
      pStr = "STERO_30FPS";
      break;
    case TOF_MODE_STERO_45FPS:
      pStr = "STERO_45FPS";
      break;
    case TOF_MODE_STERO_60FPS:
      pStr = "STERO_60FPS";
      break;

    case TOF_MODE_MONO_5FPS:
      pStr = "MONO_5FPS";
      break;
    case TOF_MODE_MONO_10FPS:
      pStr = "MONO_10FPS";
      break;
    case TOF_MODE_MONO_15FPS:
      pStr = "MONO_15FPS";
      break;
    case TOF_MODE_MONO_30FPS:
      pStr = "MONO_30FPS";
      break;
    case TOF_MODE_MONO_45FPS:
      pStr = "MONO_45FPS";
      break;
    case TOF_MODE_MONO_60FPS:
      pStr = "MONO_60FPS";
      break;

    case TOF_MODE_HDRZ_5FPS:
      pStr = "HDRZ_5FPS";
      break;
    case TOF_MODE_HDRZ_10FPS:
      pStr = "HDRZ_10FPS";
      break;
    case TOF_MODE_HDRZ_15FPS:
      pStr = "HDRZ_15FPS";
      break;
    case TOF_MODE_HDRZ_30FPS:
      pStr = "HDRZ_30FPS";
      break;
    case TOF_MODE_HDRZ_45FPS:
      pStr = "HDRZ_45FPS";
      break;
    case TOF_MODE_HDRZ_60FPS:
      pStr = "HDRZ_60FPS";
      break;

    case TOF_MODE_5FPS:
      pStr = "5FPS";
      break;
    case TOF_MODE_10FPS:
      pStr = "10FPS";
      break;
    case TOF_MODE_20FPS:
      pStr = "20FPS";
      break;
    case TOF_MODE_30FPS:
      pStr = "30FPS";
      break;
    case TOF_MODE_45FPS:
      pStr = "45FPS";
      break;
    case TOF_MODE_60FPS:
      pStr = "60FPS";
      break;

    case TOF_MODE_ADI_1M5:
      pStr = "ADI_1M5";
      break;
    case TOF_MODE_ADI_5M:
      pStr = "ADI_5M";
      break;

    case TOF_MODE_CUSTOM_1:
      pStr = "CUSTOM_1";
      break;
    case TOF_MODE_CUSTOM_2:
      pStr = "CUSTOM_2";
      break;
    case TOF_MODE_CUSTOM_3:
      pStr = "CUSTOM_3";
      break;
    case TOF_MODE_CUSTOM_4:
      pStr = "CUSTOM_4";
      break;
    case TOF_MODE_CUSTOM_5:
      pStr = "CUSTOM_5";
      break;

    case TOF_MODE_DEBUG:
      pStr = "DEBUG";
      break;

    default:
      break;
  }

  return pStr;
}

bool SaveDepthText(float *pDepthData, const UINT32 width, const UINT32 height,
                   char *pTxtFile, const bool bWH) {
  if ((NULL == pDepthData) || (0 >= width) || (0 >= height) ||
      (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  if (bWH)  // 1：W列、H行排列
  {
    UINT32 nPos = 0;
    for (UINT32 h = 0; h < height; h++) {
      for (UINT32 w = 0; w < width; w++) {
        fprintf(fp, "%0.6f,", pDepthData[nPos]);
        nPos++;
      }
      fprintf(fp, "\n");
    }
  } else  // 2：1行、W*H行排列
  {
    const UINT32 nCnt = width * height;
    for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
      fprintf(fp, "%0.6f\n", pDepthData[nPos]);
    }
  }

  fclose(fp);
  return true;
}

bool SavePointDataXYZText(PointData *pPointData, const UINT32 width,
                          const UINT32 height, char *pTxtFile) {
  if ((NULL == pPointData) || (0 >= width) || (0 >= height) ||
      (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  const UINT32 nCnt = width * height;
  for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
    fprintf(fp, "%0.6f;%0.6f;%0.6f\n", pPointData[nPos].x, pPointData[nPos].y,
            pPointData[nPos].z);
  }

  fclose(fp);
  return true;
}

bool SaveColorPointDataXYZText(PointData *pPointData, RgbDData *pRgbD,
                               const UINT32 width, const UINT32 height,
                               char *pTxtFile) {
  if ((NULL == pPointData) || (NULL == pRgbD) || (0 >= width) ||
      (0 >= height) || (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  const UINT32 nCnt = width * height;
  for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
    fprintf(fp, "v %0.6f %0.6f %0.6f %0.6f %0.6f %0.6f\n", pPointData[nPos].x,
            pPointData[nPos].y, pPointData[nPos].z, pRgbD[nPos].r * 1.0,
            pRgbD[nPos].g * 1.0, pRgbD[nPos].b * 1.0);
  }

  fclose(fp);
  return true;
}

bool SavePixelCoordText(PixelCoordData *pPixelCoord, const unsigned int width,
                        const unsigned int height, char *pTxtFile) {
  if ((NULL == pPixelCoord) || (0 >= width) || (0 >= height) ||
      (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  const unsigned int nCnt = width * height;
  for (unsigned int nPos = 0; nPos < nCnt; nPos++) {
    fprintf(fp, "%u %u\n", pPixelCoord[nPos].x, pPixelCoord[nPos].y);
  }

  fclose(fp);
  return true;
}

bool SaveFlagText(UINT8 *pFlag, const UINT32 width, const UINT32 height,
                  char *pTxtFile, const bool bWH) {
  if ((NULL == pFlag) || (0 >= width) || (0 >= height) || (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  if (bWH)  // 1：W列、H行排列
  {
    UINT32 nPos = 0;
    for (UINT32 h = 0; h < height; h++) {
      for (UINT32 w = 0; w < width; w++) {
        fprintf(fp, "%u,", pFlag[nPos]);
        nPos++;
      }
      fprintf(fp, "\n");
    }
  } else  // 2：1行、W*H行排列
  {
    const UINT32 nCnt = width * height;
    for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
      fprintf(fp, "%u\n", pFlag[nPos]);
    }
  }

  fclose(fp);
  return true;
}

bool SavePolarDataGroupText(FLOAT32 *pDepth, FLOAT32 *pAngle,
                            UINT8 *pConfidence, const UINT32 nCnt,
                            char *pTxtFile) {
  if ((NULL == pDepth) || (NULL == pAngle) || (0 >= nCnt) ||
      (NULL == pTxtFile)) {
    return false;
  }

  FILE *fp = fopen(pTxtFile, "w");
  if (NULL == fp) {
    return false;
  }

  float depthZ = 0, rad = 0;

  if (NULL != pConfidence) {
    fprintf(fp, "radial(mm),distance(mm)(vertical),angle(degree),confidence\n");
    for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
      rad = (pAngle[nPos] / 360.0) * 2 * M_PI;
      depthZ = pDepth[nPos] * cos(rad);
      fprintf(fp, "%0.2f,%0.2f,%0.2f,%u\n", pDepth[nPos], depthZ, pAngle[nPos],
              pConfidence[nPos]);
    }
  } else {
    fprintf(fp, "radial(mm),distance(mm)(vertical),angle(degree)\n");
    for (UINT32 nPos = 0; nPos < nCnt; nPos++) {
      rad = (pAngle[nPos] / 360.0) * 2 * M_PI;
      depthZ = pDepth[nPos] * cos(rad);
      fprintf(fp, "%0.2f,%0.2f,%0.2f\n", pDepth[nPos], depthZ, pAngle[nPos]);
    }
  }

  fclose(fp);
  return true;
}
void CaptureLslFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     LslFrameData *lslFrameData) {
  char szFile[512] = {0};

  for (int i = 0; i < lslFrameData->nChanCnt; i++) {
    LslFrameDataChan &chan = lslFrameData->chan[i];
    const unsigned int nPixelCnt = chan.frameWidth * chan.frameHeight;

    //
    if (NULL != chan.pGrayData) {
      sprintf(szFile, "%s/chan%u-%u-Gray.u8", strDir.c_str(), i, nCaptureIndex);
      Utils_SaveBufToFile(chan.pGrayData, nPixelCnt * sizeof(chan.pGrayData[0]),
                          szFile, false);
    }

    //
    if ((NULL != chan.pRawData) && (0 < chan.nRawDataLen)) {
      sprintf(szFile, "%s/chan%u-%u-Lsl.%s", strDir.c_str(), i, nCaptureIndex,
              "raw");
      Utils_SaveBufToFile(chan.pRawData, chan.nRawDataLen, szFile, false);
    }

    // 对于光斑个数, 则用chan.nPointCnt来表示数量
    if (0 < chan.nPointCnt) {
      sprintf(szFile, "%s/chan%u-%u-PolarDataGroup.csv", strDir.c_str(), i,
              nCaptureIndex);
      SavePolarDataGroupText(chan.pDepthData, chan.pAngle, chan.pConfidence,
                             chan.nPointCnt, szFile);
    }
    if (0 < chan.nPointCnt) {
      sprintf(szFile, "%s/chan%u-%u-PointData.txt", strDir.c_str(), i,
              nCaptureIndex);
      SavePointDataXYZText(chan.pPointData, 1, chan.nPointCnt, szFile);
    }
    if (0 < chan.nPointCnt) {
      sprintf(szFile, "%s/chan%u-%u-PointDataInRange.txt", strDir.c_str(), i,
              nCaptureIndex);
      SavePointDataXYZText(chan.pPointDataInRange, 1, chan.nPointCnt, szFile);
    }

    //
    if ((NULL != chan.pExtData) && (0 < chan.nExtDataLen)) {
      sprintf(szFile, "%s/chan%u-%u-Lsl.%s", strDir.c_str(), i, nCaptureIndex,
              "extdata");
      Utils_SaveBufToFile(chan.pExtData, chan.nExtDataLen, szFile, false);
    }
  }
}

void CaptureTofFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     TofFrameData *tofFrameData) {
  const unsigned int nPixelCnt =
      tofFrameData->frameWidth * tofFrameData->frameHeight;
  char szFile[512] = {0};

  //
  if (NULL != tofFrameData->pDepthData) {
    sprintf(szFile, "%s/%u-DepthData.dat", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pDepthData,
                        nPixelCnt * sizeof(tofFrameData->pDepthData[0]), szFile,
                        false);

    sprintf(szFile, "%s/%u-DepthData.txt", strDir.c_str(), nCaptureIndex);
    SaveDepthText(tofFrameData->pDepthData, tofFrameData->frameWidth,
                  tofFrameData->frameHeight, szFile, false);
  }

  //
  if (NULL != tofFrameData->pDepthDataFilter) {
    sprintf(szFile, "%s/%u-DepthDataFilter.dat", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pDepthDataFilter,
                        nPixelCnt * sizeof(tofFrameData->pDepthDataFilter[0]),
                        szFile, false);

    sprintf(szFile, "%s/%u-DepthDataFilter.txt", strDir.c_str(), nCaptureIndex);
    SaveDepthText(tofFrameData->pDepthDataFilter, tofFrameData->frameWidth,
                  tofFrameData->frameHeight, szFile, false);
  }

  //
  if (NULL != tofFrameData->pPointData) {
    sprintf(szFile, "%s/%u-PointData.dat", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pPointData,
                        nPixelCnt * sizeof(tofFrameData->pPointData[0]), szFile,
                        false);

    sprintf(szFile, "%s/%u-PointData.txt", strDir.c_str(), nCaptureIndex);
    SavePointDataXYZText(tofFrameData->pPointData, tofFrameData->frameWidth,
                         tofFrameData->frameHeight, szFile);
  }

  //
  if (NULL != tofFrameData->pPointDataUnfilter) {
    sprintf(szFile, "%s/%u-PointDataUnfilter.dat", strDir.c_str(),
            nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pPointDataUnfilter,
                        nPixelCnt * sizeof(tofFrameData->pPointDataUnfilter[0]),
                        szFile, false);

    sprintf(szFile, "%s/%u-PointDataUnfilter.txt", strDir.c_str(),
            nCaptureIndex);
    SavePointDataXYZText(tofFrameData->pPointDataUnfilter,
                         tofFrameData->frameWidth, tofFrameData->frameHeight,
                         szFile);
  }

  //
  if (NULL != tofFrameData->pGrayData) {
    sprintf(szFile, "%s/%u-Gray.u8", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pGrayData,
                        nPixelCnt * sizeof(tofFrameData->pGrayData[0]), szFile,
                        false);
  }
  //
  if (NULL != tofFrameData->pConfidence) {
    sprintf(szFile, "%s/%u-Confidence.u8", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pConfidence,
                        nPixelCnt * sizeof(tofFrameData->pConfidence[0]),
                        szFile, false);
  }
  //
  if (NULL != tofFrameData->pIntensity) {
    sprintf(szFile, "%s/%u-Intensity.u8", strDir.c_str(), nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pIntensity,
                        nPixelCnt * sizeof(tofFrameData->pIntensity[0]), szFile,
                        false);
  }
  //
  if (NULL != tofFrameData->pRgbD) {
    sprintf(szFile, "%s/%u-RgbD.%s", strDir.c_str(), nCaptureIndex, "bgr");
    Utils_SaveBufToFile(tofFrameData->pRgbD,
                        nPixelCnt * sizeof(tofFrameData->pRgbD[0]), szFile,
                        false);

    if (NULL != tofFrameData->pPointData) {
      sprintf(szFile, "%s/%u-PointDataColor.%s", strDir.c_str(), nCaptureIndex,
              "obj");
      SaveColorPointDataXYZText(tofFrameData->pPointData, tofFrameData->pRgbD,
                                tofFrameData->frameWidth,
                                tofFrameData->frameHeight, szFile);
    }
  }
  //
  if (NULL != tofFrameData->pRgb2TofPixelCoord) {
    sprintf(szFile, "%s/%u-Rgb2TofPixelCoord.dat", strDir.c_str(),
            nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pRgb2TofPixelCoord,
                        nPixelCnt * sizeof(tofFrameData->pRgb2TofPixelCoord[0]),
                        szFile, false);

    sprintf(szFile, "%s/%u-Rgb2TofPixelCoord.txt", strDir.c_str(),
            nCaptureIndex);
    SavePixelCoordText(tofFrameData->pRgb2TofPixelCoord,
                       tofFrameData->frameWidth, tofFrameData->frameHeight,
                       szFile);
  }
  //
  if (NULL != tofFrameData->pMaxValidPixelFlag) {
    sprintf(szFile, "%s/%u-MaxValidPixelFlag.dat", strDir.c_str(),
            nCaptureIndex);
    Utils_SaveBufToFile(tofFrameData->pMaxValidPixelFlag,
                        nPixelCnt * sizeof(tofFrameData->pMaxValidPixelFlag[0]),
                        szFile, false);

    sprintf(szFile, "%s/%u-MaxValidPixelFlag.txt", strDir.c_str(),
            nCaptureIndex);
    SaveFlagText(tofFrameData->pMaxValidPixelFlag, tofFrameData->frameWidth,
                 tofFrameData->frameHeight, szFile, false);
  }

  //
  if ((NULL != tofFrameData->pRawData) && (0 < tofFrameData->nRawDataLen)) {
    sprintf(szFile, "%s/%u-Tof.%s", strDir.c_str(), nCaptureIndex, "raw");
    Utils_SaveBufToFile(tofFrameData->pRawData, tofFrameData->nRawDataLen,
                        szFile, false);
  }
  //
  if ((NULL != tofFrameData->pExtData) && (0 < tofFrameData->nExtDataLen)) {
    sprintf(szFile, "%s/%u-Tof.%s", strDir.c_str(), nCaptureIndex, "extdata");
    Utils_SaveBufToFile(tofFrameData->pExtData, tofFrameData->nExtDataLen,
                        szFile, false);
  }
}

void CaptureRgbFrame(const std::string &strDir,
                     const unsigned int nCaptureIndex,
                     RgbFrameData *rgbFrameData) {
  char szFile[512] = {0};

  //
  if ((NULL != rgbFrameData->pFrameData) && (0 < rgbFrameData->nFrameLen)) {
    sprintf(szFile, "%s/%u-Rgb.%s", strDir.c_str(), nCaptureIndex,
            StringColorFormat(rgbFrameData->formatType));
    Utils_SaveBufToFile(rgbFrameData->pFrameData, rgbFrameData->nFrameLen,
                        szFile, false);
  }
  //
  if ((NULL != rgbFrameData->pExtData) && (0 < rgbFrameData->nExtDataLen)) {
    sprintf(szFile, "%s/%u-Rgb.%s", strDir.c_str(), nCaptureIndex, "extdata");
    Utils_SaveBufToFile(rgbFrameData->pExtData, rgbFrameData->nExtDataLen,
                        szFile, false);
  }
}

void CallBackTofDeviceStatus(TOFDEV_STATUS tofDevStatus, void *pUserData) {
  printf("device status: 0x08%x.\n", tofDevStatus);

  if (TOFDEV_STATUS_DEV_BROKEN == tofDevStatus) {
    printf(
        "a device connection is broken!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    g_isConnectionBroken = true;
  }
}

void HandleCallBackTofStream(TofFrameData *tofFrameData, void *pUserData) {
  // 特别注意：因为有些模块的tofFrameData里的各个指针仅在回调函数内有效，
  // 所以，如果要将tofFrameData数据放到其他线程里去处理，必须将tofFrameData里各个指针对应的内存内容复制一份！！！！！！

  StreamCallBack *pStreamParam = (StreamCallBack *)pUserData;
  (pStreamParam->tof_frame_count)++;

  const UINT32 frame_count = pStreamParam->tof_frame_count;
  const UINT64 host_ts = TOFD_GetHostTimeStamp();  // 主机时间

  // printf("szDevId=%s.\n",
  // pStreamParam->struCaps.szDevId);//szDevId可用于区分是哪个模块

  //
  const UINT32 width = tofFrameData->frameWidth;
  const UINT32 height = tofFrameData->frameHeight;

  const UINT64 dev_ts = tofFrameData->timeStamp;  // 设备时间
  const UINT64 frameDelay = host_ts - dev_ts;     // 帧延时
  //
  const float depthZ =
      CalCenterPointDataZAvg(tofFrameData->pPointData, tofFrameData->frameWidth,
                             tofFrameData->frameHeight);

  // printf(
  //     "[%u], TOF frame, host_ts=%llu, dev_ts=%llu, frameDelay=%llu, "
  //     "depthZ=%0.3fm.\n",
  //     frame_count, host_ts, dev_ts, frameDelay, depthZ);
  // printf("[%u], bMultiDevInterference = %u.\n", frame_count,
  // tofFrameData->bMultiDevInterference); printf("[%u], bHighReflectDetected =
  // %u.\n", frame_count, tofFrameData->bHighReflectDetected);

  std::lock_guard<std::mutex> lock(tofLock);
  pStreamParam->tofFrameData = *tofFrameData;

  return;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 单位是毫秒
  CaptureTofFrame(std::string("./Capture"), frame_count, tofFrameData);
}

void HandleCallBackLslStream(LslFrameData *lslFrameData, void *pUserData) {
  // 特别注意：因为有些模块的lslFrameData里的各个指针仅在回调函数内有效，
  // 所以，如果要将lslFrameData数据放到其他线程里去处理，必须将lslFrameData里各个指针对应的内存内容复制一份！！！！！！

  StreamCallBack *pStreamParam = (StreamCallBack *)pUserData;
  (pStreamParam->tof_frame_count)++;

  const UINT32 frame_count = pStreamParam->tof_frame_count;
  const UINT64 host_ts = TOFD_GetHostTimeStamp();  // 主机时间

  // printf("szDevId=%s.\n",
  // pStreamParam->struCaps.szDevId);//szDevId可用于区分是哪个模块

  for (UINT32 i = 0; i < lslFrameData->nChanCnt; i++) {
    LslFrameDataChan &chan = lslFrameData->chan[i];

    //
    const UINT32 nPointCnt = chan.nPointCnt;
    const UINT64 dev_ts = chan.timeStamp;        // 设备时间
    const UINT64 frameDelay = host_ts - dev_ts;  // 帧延时

    //
    // printf(
    //     "[%u], LSL frame, chan=%u, host_ts=%llu, dev_ts=%llu, "
    //     "frameDelay=%llu.\n",
    //     frame_count, i, host_ts, dev_ts, frameDelay);
  }

  return;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 单位是毫秒
  CaptureLslFrame(std::string("./Capture"), frame_count, lslFrameData);
}

void CallBackTofStream(TofFrameData *tofFrameData, void *pUserData) {
  if (PRODUCT_SERIES_TOF == tofFrameData->productSeries) {
    HandleCallBackTofStream(tofFrameData, pUserData);
  } else if (PRODUCT_SERIES_LSL == tofFrameData->productSeries) {
    HandleCallBackLslStream(tofFrameData->pLsl, pUserData);
  }
}

void CallBackRgbStream(RgbFrameData *rgbFrameData, void *pUserData) {
  // 特别注意：因为有些模块的rgbFrameData里的各个指针仅在回调函数内有效，
  // 所以，如果要将rgbFrameData数据放到其他线程里去处理，必须将rgbFrameData里各个指针对应的内存内容复制一份！！！！！！

  StreamCallBack *pStreamParam = (StreamCallBack *)pUserData;
  (pStreamParam->rgb_frame_count)++;

  const UINT32 frame_count = pStreamParam->rgb_frame_count;
  const UINT64 host_ts = TOFD_GetHostTimeStamp();  // 主机时间

  // printf("szDevId=%s.\n",
  // pStreamParam->struCaps.szDevId);//szDevId可用于区分是哪个模块

  //
  const UINT32 width = rgbFrameData->frameWidth;
  const UINT32 height = rgbFrameData->frameHeight;

  const UINT64 dev_ts = rgbFrameData->timeStamp;  // 设备时间
  const UINT64 frameDelay = host_ts - dev_ts;     // 帧延时

  //
  // printf(
  //     "[%u], RGB frame, host_ts=%llu, dev_ts=%llu, frameDelay=%llu, "
  //     "formatType=0x%08x.\n",
  //     frame_count, host_ts, dev_ts, frameDelay, rgbFrameData->formatType);

  std::lock_guard<std::mutex> lock(rgbLock);
  pStreamParam->rgbFrameData = *rgbFrameData;

  return;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 单位是毫秒
  CaptureRgbFrame(std::string("./Capture"), frame_count, rgbFrameData);
}

void PrintDevInfo(TofDeviceInfo *pTofDeviceInfo) {
  printf("Dev Info:==================================\n");
  printf(">>  szDevName=%s.\n", pTofDeviceInfo->szDevName);
  printf(">>  szDevId=%s.\n", pTofDeviceInfo->szDevId);
  printf(">>  szFirmwareVersion=%s.\n", pTofDeviceInfo->szFirmwareVersion);
  printf("Dev Info==================================\n\n");
}

TOF_MODE ChoseTofMode(const UINT32 devIndex, TofDeviceInfo &struCaps) {
  // 打印出所有支持的TOF模式，供用户选择
  printf("chose tof mode from list, dev index=%d: \n", devIndex);
  printf(">>  number: mode.\n");
  for (UINT32 i = 0; i < struCaps.capCnt; i++) {
    printf(">>  %u: %s.\n", i, TofMode2Str(struCaps.cap[i].tofMode));
  }

  if (1 == struCaps.capCnt)  // 只有一种模式，直接返回，省去输入的过程
  {
    return struCaps.cap[0].tofMode;
  }

#if 0
	//这里简单处理，选择第一种支持的模式
	return struCaps.cap[0].tofMode;
#else
  // 用于选择哪一种tof模式
  while (1) {
    printf("input mode (number) >>");

    std::string strInput;
    std::cin >> strInput;

    const UINT32 i = (UINT32)strtol(strInput.c_str(), NULL, 10);
    if (struCaps.capCnt > i) {
      return struCaps.cap[i].tofMode;
    } else {
      printf("the number is invalid.\n");
    }
  }
#endif
}

bool OpenStream(HTOFD hTofD, TofDeviceCapability *pCaps,
                StreamCallBack *pStreamParam) {
  if (pCaps->bTofSupported)  // 支持TOF的情况下
  {
    pStreamParam->tof_frame_count = 0;
    TOFRET retVal = TOFD_StartTofStream(hTofD, CallBackTofStream, pStreamParam);
    if ((TOFRET_SUCCESS != retVal) &&
        (TOFRET_SUCCESS_READING_CALIB != retVal)) {
      printf(
          "start TOF stream, [ FAILED ], "
          "retVal=0x%08x!!!!!!!!!!!!!!!!!!!!!\n\n",
          retVal);
      return false;
    }
  }

  if (pCaps->bRgbSupported)  // 支持RGB的情况下
  {
    pStreamParam->rgb_frame_count = 0;
    TOFRET retVal = TOFD_StartRgbStream(hTofD, CallBackRgbStream, pStreamParam);
    if (TOFRET_SUCCESS != retVal) {
      printf(
          "start RGB stream, [ FAILED ], "
          "retVal=0x%08x!!!!!!!!!!!!!!!!!!!!!\n\n",
          retVal);
      return false;
    }
  }

  return true;
}

void CloseStream(HTOFD hTofD, TofDeviceCapability *pCaps) {
  if (pCaps->bTofSupported)  // 支持TOF的情况下
  {
    TOFD_StopTofStream(hTofD);
  }

  if (pCaps->bRgbSupported)  // 支持RGB的情况下
  {
    TOFD_StopRgbStream(hTofD);
  }
}

void PrintfTofLensParameter(TofModuleLensParameterV20 *pTofLens) {
  if (NULL == pTofLens) return;

  const UINT32 nIndex = pTofLens->nIndex;

  if (1 == nIndex) {
    TofModuleLensGeneral *pTmp = &(pTofLens->uParam.general);

    printf("Tof Lens Paramter (general):...............................\n");
    printf(">>   fx = %f.\n", pTmp->fx);
    printf(">>   fy = %f.\n", pTmp->fy);
    printf(">>   cx = %f.\n", pTmp->cx);
    printf(">>   cy = %f.\n", pTmp->cy);
    printf(">>   k1 = %f.\n", pTmp->k1);
    printf(">>   k2 = %f.\n", pTmp->k2);
    printf(">>   p1 = %f.\n", pTmp->p1);
    printf(">>   p2 = %f.\n", pTmp->p2);
    printf(">>   k3 = %f.\n", pTmp->k3);
  } else if (2 == nIndex) {
    TofModuleLensFishEye *pTmp = &(pTofLens->uParam.fishEye);

    printf("Tof  Lens Paramter (fishEye):...............................\n");
    printf(">>   fx = %f.\n", pTmp->fx);
    printf(">>   fy = %f.\n", pTmp->fy);
    printf(">>   cx = %f.\n", pTmp->cx);
    printf(">>   cy = %f.\n", pTmp->cy);
    printf(">>   k1 = %f.\n", pTmp->k1);
    printf(">>   k2 = %f.\n", pTmp->k2);
    printf(">>   k3 = %f.\n", pTmp->k3);
    printf(">>   k4 = %f.\n", pTmp->k4);
  } else {
    printf("Tof Lens Paramter (index=%u):...............................\n",
           nIndex);
    printf(">>   unknown, not supported.\n");
  }
}

void PrintfRgbLensParameter(RgbModuleLensParameterV20 *pRgbLens) {
  if (NULL == pRgbLens) return;

  const UINT32 nIndex = pRgbLens->nIndex;

  if (1 == nIndex) {
    RgbModuleLensGeneral *pTmp = &(pRgbLens->uParam.general);

    printf("Rgb Lens Paramter (general):...............................\n");
    printf(">>   fx = %f.\n", pTmp->fx);
    printf(">>   fy = %f.\n", pTmp->fy);
    printf(">>   cx = %f.\n", pTmp->cx);
    printf(">>   cy = %f.\n", pTmp->cy);
    printf(">>   k1 = %f.\n", pTmp->k1);
    printf(">>   k2 = %f.\n", pTmp->k2);
    printf(">>   p1 = %f.\n", pTmp->p1);
    printf(">>   p2 = %f.\n", pTmp->p2);
    printf(">>   k3 = %f.\n", pTmp->k3);
  } else if (2 == nIndex) {
    RgbModuleLensFishEye *pTmp = &(pRgbLens->uParam.fishEye);

    printf("Rgb Lens Paramter (fishEye):...............................\n");
    printf(">>   fx = %f.\n", pTmp->fx);
    printf(">>   fy = %f.\n", pTmp->fy);
    printf(">>   cx = %f.\n", pTmp->cx);
    printf(">>   cy = %f.\n", pTmp->cy);
    printf(">>   k1 = %f.\n", pTmp->k1);
    printf(">>   k2 = %f.\n", pTmp->k2);
    printf(">>   k3 = %f.\n", pTmp->k3);
    printf(">>   k4 = %f.\n", pTmp->k4);
  } else {
    printf("Rgb Lens Paramter (index=%u):...............................\n",
           nIndex);
    printf(">>   unknown, not supported.\n");
  }
}

void PrintfStereoLensParameter(StereoLensParameter *pStereoLens) {
  if (NULL == pStereoLens) return;

  StereoLensParameter *pTmp = pStereoLens;

  printf("Stereo Lens Parameter: \n");
  printf(">>  Rotation Matrix:\n");
  printf("    %f, %f, %f\r\n", pTmp->szRotationMatrix[0][0],
         pTmp->szRotationMatrix[0][1], pTmp->szRotationMatrix[0][2]);
  printf("    %f, %f, %f\r\n", pTmp->szRotationMatrix[1][0],
         pTmp->szRotationMatrix[1][1], pTmp->szRotationMatrix[1][2]);
  printf("    %f, %f, %f\r\n", pTmp->szRotationMatrix[2][0],
         pTmp->szRotationMatrix[2][1], pTmp->szRotationMatrix[2][2]);
  printf(">>  Translation Matrix:\n");
  printf("    %f, %f, %f\r\n", pTmp->szTranslationMatrix[0],
         pTmp->szTranslationMatrix[1], pTmp->szTranslationMatrix[2]);
}

void PrintfDepthCalRoi(DepthCalRoi *Roi) {
  if (NULL == Roi) return;

  printf("Depth Cal Roi:...............................\n");
  printf(">>    struMax.left=%u.\n", Roi->struMax.left);
  printf(">>    struMax.right=%u.\n", Roi->struMax.right);
  printf(">>    struMax.top=%u.\n", Roi->struMax.top);
  printf(">>    struMax.bottom=%u.\n", Roi->struMax.bottom);
  printf(">>    struDefault.left=%u.\n", Roi->struDefault.left);
  printf(">>    struDefault.right=%u.\n", Roi->struDefault.right);
  printf(">>    struDefault.top=%u.\n", Roi->struDefault.top);
  printf(">>    struDefault.bottom=%u.\n", Roi->struDefault.bottom);
  printf(">>    struCurrent.left=%u.\n", Roi->struCurrent.left);
  printf(">>    struCurrent.right=%u.\n", Roi->struCurrent.right);
  printf(">>    struCurrent.top=%u.\n", Roi->struCurrent.top);
  printf(">>    struCurrent.bottom=%u.\n", Roi->struCurrent.bottom);
}

void GetOrSetSomeParam(HTOFD hTofD, StreamCallBack &frameData,
                       TofDeviceCapability *pCaps) {
  TOFRET retVal = TOFRET_ERROR_OTHER;

  if (pCaps->supportedTofExpMode & EXP_MODE_AUTO) {
    if (TOFRET_SUCCESS != (retVal = TOFD_SetTofAE(hTofD, true))) {
      printf("TOFD_SetTofAE failed, retVal=0x%08x.\n", retVal);
    }
  }

  if (pCaps->bTofHDRZSupported) {
    if (TOFRET_SUCCESS != (retVal = TOFD_SetTofHDRZ(hTofD, true))) {
      printf("TOFD_SetTofHDRZ failed, retVal=0x%08x.\n", retVal);
    }
  }

  for (UINT32 i = 0; i < 32; i++) {
    UINT32 type = (1 << i);
    if (0 != (pCaps->supportedTOFFilter & type)) {
      if (TOFRET_SUCCESS !=
          (retVal = TOFD_SetTofFilter(hTofD, (const TOF_FILTER)type, true))) {
        printf("TOFD_SetTofFilter(type:0x%08x) failed, retVal=0x%08x.\n", type,
               retVal);
      }
    }
  }

  if (pCaps->bTofRemoveINSSupported) {
    if (TOFRET_SUCCESS != (retVal = TOFD_SetTofRemoveINS(hTofD, true))) {
      printf("TOFD_SetTofRemoveINS failed, retVal=0x%08x.\n", retVal);
    }
  }

  TofDeviceParamV20 struParam;

  if (pCaps->bTofMpiCorrectSupported) {
    memset(&struParam, 0, sizeof(struParam));
    struParam.type = TOF_DEV_PARAM_TofMpiCorrect;
    struParam.uParam.struTofMpiCorrect.bEnable = true;
    if (TOFRET_SUCCESS !=
        (retVal = TOFD_SetDeviceParamV20(hTofD, &struParam))) {
      printf("TOFD_SetDeviceParamV20(TofMpiCorrect) failed, retVal=0x%08x.\n",
             retVal);
    }
  }

  if (pCaps->bTofMpiFuseSupported) {
    memset(&struParam, 0, sizeof(struParam));
    struParam.type = TOF_DEV_PARAM_TofMpiFuse;
    struParam.uParam.struTofMpiFuse.bEnable = true;
    if (TOFRET_SUCCESS !=
        (retVal = TOFD_SetDeviceParamV20(hTofD, &struParam))) {
      printf("TOFD_SetDeviceParamV20(TofMpiFuse) failed, retVal=0x%08x.\n",
             retVal);
    }
  }

  // 设置不同的滤波等级
  if (pCaps->bTofFilterLevelSupported) {
    memset(&struParam, 0, sizeof(struParam));
    struParam.type = TOF_DEV_PARAM_TofFilterLevel;
    struParam.uParam.struTofFilterLevel.nLevel = pCaps->nTofFilterLevelMin;
    // struParam.uParam.struTofFilterLevel.nLevel = pCaps->nTofFilterLevelMax;
    if (TOFRET_SUCCESS !=
        (retVal = TOFD_SetDeviceParamV20(hTofD, &struParam))) {
      printf(
          "TOFD_SetDeviceParamV20(TofFilterLevel:%u) failed, retVal=0x%08x.\n",
          struParam.uParam.struTofFilterLevel.nLevel, retVal);
    }
  }

#if 0
	//设置帧率
	if (0 < pCaps->supportedFps.nCnt)
	{
		const int index = 0;//按需选择，不超过pCaps->supportedFps.nCnt

		memset(&struParam, 0, sizeof(struParam));
		struParam.type = TOF_DEV_PARAM_Fps;
		struParam.uParam.struFps.tof = pCaps->supportedFps.fps[index].tof;
		struParam.uParam.struFps.rgb = pCaps->supportedFps.fps[index].rgb;
		if (TOFRET_SUCCESS != (retVal = TOFD_SetDeviceParamV20(hTofD, &struParam)))
		{
			printf("TOFD_SetDeviceParamV20(fps:%u/%u) failed, retVal=0x%08x.\n", struParam.uParam.struFps.tof, struParam.uParam.struFps.rgb, retVal);
		}
	}
#endif

  // 打印出TOF内参
  memset(&struParam, 0, sizeof(struParam));
  struParam.type = TOF_DEV_PARAM_TofLensParameterV20;
  if (TOFRET_SUCCESS == (retVal = TOFD_GetDeviceParamV20(hTofD, &struParam))) {
    // PrintfTofLensParameter(&(struParam.uParam.struTofLensParameterV20));
    frameData.tofDeviceParam = struParam.uParam.struTofLensParameterV20;
  }

  // 设置并打印深度计算的区域
  memset(&struParam, 0, sizeof(struParam));
  struParam.type = TOF_DEV_PARAM_DepthCalRoi;
  if (TOFRET_SUCCESS == (retVal = TOFD_GetDeviceParamV20(hTofD, &struParam))) {
    // PrintfDepthCalRoi(&(struParam.uParam.struDepthCalRoi));
  } else {
    printf("TOF depthRange Get Failed\n");
  }

  // 打印出RGB内参
  if (pCaps->bRgbSupported) {
    memset(&struParam, 0, sizeof(struParam));
    struParam.type = TOF_DEV_PARAM_RgbLensParameterV20;
    if (TOFRET_SUCCESS ==
        (retVal = TOFD_GetDeviceParamV20(hTofD, &struParam))) {
      // PrintfRgbLensParameter(&(struParam.uParam.struRgbLensParameterV20));
      frameData.rgbDeviceParam = struParam.uParam.struRgbLensParameterV20;
      printf("\n");
    } else {
      printf("RGB Get Failed\n");
    }
  }
  frameData.inited = true;
}

void SaveSomeData(HTOFD hTofD, TofDeviceInfo *pCaps, std::string &strSaveDir) {
  TOFRET retVal = TOFRET_ERROR_OTHER;
  TofDeviceParamV20 struParam;
  std::string strPath;

  // 保存标定文件
  memset(&struParam, 0, sizeof(struParam));
  struParam.type = TOF_DEV_PARAM_TofCalibData;
  if (TOFRET_SUCCESS == (retVal = TOFD_GetDeviceParamV20(hTofD, &struParam))) {
    TofCalibData *pTmp = &(struParam.uParam.struTofCalibData);
    strPath = (strSaveDir + std::string("/") + std::string(pCaps->szDevId) +
               std::string(".bin"));
    Utils_SaveBufToFile(pTmp->pData, pTmp->nDataLen, strPath.c_str(), false);
  }
}

TofDeviceCapability *GetTofModeCaps(TofDeviceInfo &struCaps,
                                    const TOF_MODE tofMode) {
  for (UINT32 i = 0; ((i < struCaps.capCnt) && (i < TOF_MAX_CAPS_CNT)); i++) {
    if (tofMode == struCaps.cap[i].tofMode) {
      return (&(struCaps.cap[i]));
    }
  }

  return NULL;
}

void captureThread(DevsDataList &dev, std::string strSaveDir) {
  TOFRET retVal = TOFD_SetTofMode(dev.hTofD, dev.tofMode);
  if (TOFRET_SUCCESS != retVal) {
    printf(
        "set tof mode (0x%08x), [ FAILED ], "
        "retVal=0x%08x!!!!!!!!!!!!!!!!!!!!!\n\n",
        dev.tofMode, retVal);
    return;
  }

  TofDeviceCapability *pCaps =
      GetTofModeCaps(dev.struCaps, dev.tofMode);  // 获取tof mode对应的能力

  //
  StreamCallBack &frameData = dev.frameData;
  memset(&(frameData), 0, sizeof(frameData));
  memcpy(&frameData.struCaps, &dev.struCaps, sizeof(dev.struCaps));
  const bool bSuc = OpenStream(dev.hTofD, pCaps, &frameData);
  if (bSuc) {
    // 成功后，等到数据流拿到后再去设置参数，这样才能保证生效(此逻辑是为了兼容所有模块)
    while ((0 >= frameData.tof_frame_count) &&
           (0 >= frameData.rgb_frame_count)) {
      printf("Waiting for stream data...\n");
      std::this_thread::sleep_for(
          std::chrono::milliseconds(1000));  // 单位是毫秒
    }

    printf("Get Or Set Some Params...\n");
    GetOrSetSomeParam(dev.hTofD, frameData, pCaps);
  }

  // 等待释放该设备资源的信号
  while ((!g_signalCtrlC)) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));  // 单位是毫秒
                                          // std::cout << "," << std::flush;
  }
}

bool ChoseDevTofMode(TofDeviceDescriptor *pDevsDesc, const UINT32 devIndex,
                     DevsDataList &chosedDev) {
  HTOFD hTofD = TOFD_OpenDevice(pDevsDesc, CallBackTofDeviceStatus, NULL);
  if (NULL == hTofD) {
    printf("Open Tof Device failed, dev index=%d!!!!!!!!!!!!!!!!!!!!!\n\n",
           devIndex);
    return false;
  }

  TofDeviceInfo struCaps;
  memset(&struCaps, 0, sizeof(struCaps));
  TOFD_GetDeviceInfo(hTofD, &struCaps);
  PrintDevInfo(&struCaps);

  //
  chosedDev.devIndex = devIndex;
  chosedDev.hTofD = hTofD;
  strcpy(chosedDev.deviceId, struCaps.szDevId);
  memcpy(&chosedDev.struCaps, &struCaps, sizeof(struCaps));
  // chosedDev.tofMode =
  //     ChoseTofMode(devIndex, struCaps);  // 选择其中一种TOF模式出TOF数据
  chosedDev.tofMode = struCaps.cap[0].tofMode;  // 选择默认模式

  return true;
}

UINT32 OpenAllDevAndChoseTofMode(TofDeviceDescriptor *pDevsDescList,
                                 const UINT32 dev_cnt,
                                 DevsDataList *pdevsDataList) {
  DevsDataList struTmp;
  UINT32 suc_cnt = 0;

  for (UINT32 devIndex = 0; devIndex < dev_cnt; devIndex++) {
    printf("chose dev mode, dev index=%d(max=%d)...\n", devIndex, dev_cnt);

    memset(&struTmp, 0, sizeof(struTmp));
    if (!ChoseDevTofMode(pDevsDescList + devIndex, devIndex, struTmp)) {
      printf("chose dev mode failed, dev index=%d, skip.\n", devIndex);
      continue;
    }

    memcpy(pdevsDataList + suc_cnt, &struTmp, sizeof(struTmp));
    suc_cnt++;
  }

  return suc_cnt;
}

void HandleSignal(int sig) {
  printf("recieve signal:%d.\n", sig);
  switch (sig) {
    case SIGINT:
      g_signalCtrlC = true;
      printf("  ctrl+c signal ...\n");
      break;
  }
}
