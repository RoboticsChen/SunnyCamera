#include "sunny_camera.hpp"

bool g_signalCtrlC = false;        // ctrl-c信号
bool g_isConnectionBroken = false; // ConnectionBroken信号

std::mutex rgbLock; // 全局锁
std::mutex tofLock; // 全局锁
TofDeviceDescriptor *pDevsDescList = NULL;
std::unique_ptr<DevsDataList[]> devsDataList; // 全局设备数据列表
int device_num = 0;

//****************************************************************************
//*----------------------------Device Initialize-----------------------------*
//****************************************************************************
void SunnyCamera::device_init()
{
  g_signalCtrlC = false;
  signal(SIGINT, HandleSignal);

  g_isConnectionBroken = false;

  std::string strSaveDir =
      (""); //(".");//用于保存文件的目录，可以自己指定，为空则表示不保存文件

  TofDevInitParam struInitParam;
  memset(&struInitParam, 0, sizeof(struInitParam));
  strncpy(struInitParam.szDepthCalcCfgFileDir, "./parameter",
          sizeof(struInitParam.szDepthCalcCfgFileDir) - 1);
  struInitParam.bSupUsb = true;
  struInitParam.bSupNetWork = false;
  struInitParam.bSupSerialCOM = false;
  if (struInitParam.bSupSerialCOM)
  {
    strncpy(
        struInitParam.szSerialDev, "COM1",
        sizeof(
            struInitParam
                .szSerialDev)); // windows下必须赋值一个实际使用的串口，这里随便写了一个
    // strncpy(struInitParam.szSerialDev, "/dev/ttyUSB0",
    // sizeof(struInitParam.szSerialDev));//linux下必须赋值一个实际使用的串口，这里随便写了一个
  }
  struInitParam.bWeakAuthority = false;
  struInitParam.bDisablePixelOffset = false;
  strncpy(struInitParam.szLogFile, "./tof_dev_sdk_log.txt",
          sizeof(struInitParam.szLogFile)); // 不赋值则不记录日志到文件
  struInitParam.nLogFileMaxSize = 10 * 1024 * 1024;
  TOFD_Init(&struInitParam);

  printf("SDK Version: %s.\n", TOFD_GetSDKVersion());

  UINT32 searched_dev_cnt = 0;
  TOFD_SearchDevice(&pDevsDescList, &searched_dev_cnt);
  if (searched_dev_cnt > 0)
  {
    printf("Find total %d devices!\n", searched_dev_cnt);
    std::unique_ptr<DevsDataList[]> newdevsDataList(
        new DevsDataList[searched_dev_cnt]);
    devsDataList = std::move(newdevsDataList);
    const UINT32 suc_dev_cnt = OpenAllDevAndChoseTofMode(
        pDevsDescList, searched_dev_cnt,
        devsDataList.get()); // 打开每个设备，并选择每个设备使用的模式
    device_num = suc_dev_cnt;
  }
  else
  {
    printf("can not find tof device!\n");
  }
}

void SunnyCamera::device_deinit()
{
  TOFD_Uninit();
  g_signalCtrlC = true;
}
//****************************************************************************
//*------------------------------Camera Class--------------------------------*
//****************************************************************************

SunnyCamera::Camera::Camera(std::string target_id)
{
  bool find_camera = false;
  for (int i = 0; i < device_num; i++)
  {
    if (devsDataList.get()[i].deviceId == target_id)
    {
      camera_index = devsDataList.get()[i].devIndex;
      hTofD = devsDataList.get()[i].hTofD;
      pCaps = devsDataList.get()[i].struCaps;
      camera_id = devsDataList.get()[i].deviceId;

      find_camera = true;
      break;
    }
  }
  if (!find_camera)
  {
    printf("Can't Find %s .Instantiate Object Failed \n", target_id.c_str());
    if (device_num == 0)
    {
      printf("Without available camera, please check camera connection!!!\n");
    }
    else
    {
      printf("Total %d available camera IDs are as follows:\n", device_num);
      for (int i = 0; i < device_num; i++)
      {
        printf("%s\n", devsDataList.get()[i].deviceId);
      }
    }
  }
  inited = false;
}

SunnyCamera::Camera::~Camera()
{
  TofDeviceCapability *pCaps =
      GetTofModeCaps(devsDataList.get()[camera_index].struCaps,
                     devsDataList.get()[camera_index].tofMode);
  CloseStream(hTofD, pCaps);
  TOFD_CloseDevice(hTofD);
}

bool SunnyCamera::Camera::init()
{
  // std::cout << "Begin update thread!!!" << std::endl << std::flush;
  std::thread thread_update([this]()
                            { updateThread(); });

  std::string strSaveDir = (""); // 用于保存文件的目录，为空则表示不保存文件
  // std::cout << "Begin capture thread!!!   " << std::endl << std::flush;
  std::thread thread_capture(
      captureThread, std::ref(devsDataList.get()[camera_index]), strSaveDir);

  // wait for inited
  while (!devsDataList.get()[camera_index].frameData.inited)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 单位是毫秒
  }
  init_intrinsic_distortion();

  thread_capture.detach();
  thread_update.detach();
  printf("***************Camera %s init success!****************\n",
         camera_id.c_str());
  inited = true;
  return true;
}

bool SunnyCamera::Camera::deinit()
{
  TOFD_CloseDevice(hTofD);
  inited = false;
  return true;
}

void SunnyCamera::Camera::updateThread()
{
  signal(SIGINT, HandleSignal);
  while ((!g_signalCtrlC))
  {
    {
      std::lock_guard<std::mutex> lock(tofLock);
      tofFrameData = devsDataList.get()[camera_index].frameData.tofFrameData;
    }
    {
      std::lock_guard<std::mutex> lock(rgbLock);
      rgbFrameData = devsDataList.get()[camera_index].frameData.rgbFrameData;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 单位是毫秒
  }
}

void SunnyCamera::Camera::init_intrinsic_distortion()
{
  RgbModuleLensFishEye *rgbFish = &(
      devsDataList.get()[camera_index].frameData.rgbDeviceParam.uParam.fishEye);
  cv::Mat rgbI = cv::Mat::zeros(3, 3, CV_32F);
  rgbI.at<float>(0, 0) = rgbFish->fx;
  rgbI.at<float>(0, 1) = 0;
  rgbI.at<float>(0, 2) = rgbFish->cx;
  rgbI.at<float>(1, 0) = 0;
  rgbI.at<float>(1, 1) = rgbFish->fy;
  rgbI.at<float>(1, 2) = rgbFish->cy;
  rgbI.at<float>(2, 0) = 0;
  rgbI.at<float>(2, 1) = 0;
  rgbI.at<float>(2, 2) = 1;
  cameraIntrinsic.rgb_INTRINSIC = rgbI;
  cv::Mat rgbD = cv::Mat::zeros(1, 4, CV_32F);
  rgbD.at<float>(0, 0) = rgbFish->k1;
  rgbD.at<float>(0, 1) = rgbFish->k2;
  rgbD.at<float>(0, 2) = rgbFish->k3;
  rgbD.at<float>(0, 3) = rgbFish->k4;
  cameraDistortion.rgb_DISTORTION = rgbD;

  // // depth intrinsic
  TofModuleLensFishEye *tofFish = &(
      devsDataList.get()[camera_index].frameData.tofDeviceParam.uParam.fishEye);
  cv::Mat depthI = cv::Mat::zeros(3, 3, CV_32F);
  depthI.at<float>(0, 0) = tofFish->fx;
  depthI.at<float>(0, 1) = 0;
  depthI.at<float>(0, 2) = tofFish->cx;
  depthI.at<float>(1, 0) = 0;
  depthI.at<float>(1, 1) = tofFish->fy;
  depthI.at<float>(1, 2) = tofFish->cy;
  depthI.at<float>(2, 0) = 0;
  depthI.at<float>(2, 1) = 0;
  depthI.at<float>(2, 2) = 1;
  cameraIntrinsic.depth_INTRINSIC = depthI;
  cv::Mat depthD = cv::Mat::zeros(1, 4, CV_32F);
  depthD.at<float>(0, 0) = tofFish->k1;
  depthD.at<float>(0, 1) = tofFish->k2;
  depthD.at<float>(0, 2) = tofFish->k3;
  depthD.at<float>(0, 3) = tofFish->k4;
  cameraDistortion.depth_DISTORTION = depthD;
  // std::cout << "intrinsic initialized !!!" << std::endl;
}

int SunnyCamera::Camera::WIDTH(std::string image_type) const
{
  if (image_type == "rgb")
  {
    return rgbFrameData.frameWidth;
  }
  else if (image_type == "depth")
  {
    return tofFrameData.frameWidth;
  }
  else
  {
    printf("Error image_type to getWIDTH !!!");
    return -1;
  }
}

int SunnyCamera::Camera::HEIGHT(std::string image_type) const
{
  if (image_type == "rgb")
  {
    return rgbFrameData.frameHeight;
  }
  else if (image_type == "depth")
  {
    return tofFrameData.frameHeight;
  }
  else
  {
    printf("Error image_type to getHEIGHT !!!");
    return -1;
  }
}

cv::Mat SunnyCamera::Camera::INTRINSIC(std::string image_type)
{
  if (image_type == "rgb")
  {
    return cameraIntrinsic.rgb_INTRINSIC;
  }
  else if (image_type == "depth")
  {
    return cameraIntrinsic.depth_INTRINSIC;
  }
  else
  {
    printf("Error image_type to getINTRINSIC !!!");
    return cv::Mat();
  }
}

cv::Mat SunnyCamera::Camera::DISTORTION(std::string image_type)
{
  if (image_type == "rgb")
  {
    return cameraDistortion.rgb_DISTORTION;
  }
  else if (image_type == "depth")
  {
    return cameraDistortion.depth_DISTORTION;
  }
  else
  {
    printf("Error image_type to getDISTORTION !!!");
    return cv::Mat();
  }
}

cv::Mat SunnyCamera::Camera::get_rgb()
{
  if (rgbFrameData.pFrameData != nullptr)
  {
    if (rgbFrameData.formatType == COLOR_FORMAT_MJPEG)
    {
      // 解码JPEG数据
      cv::Mat Image = cv::imdecode(
          cv::Mat(1, rgbFrameData.nFrameLen, CV_8UC1, rgbFrameData.pFrameData),
          cv::IMREAD_UNCHANGED);
      if (Image.empty())
      {
        std::cerr << "Error decoding JPEG data." << std::endl;
      }
      cv::Mat bgr_image(rgbFrameData.frameHeight, rgbFrameData.frameWidth,
                        CV_8UC3, Image.data);
      cv::Mat rgb_image;
      cv::cvtColor(bgr_image, rgb_image, cv::COLOR_BGR2RGB);
      return rgb_image;
    }
    else if (rgbFrameData.formatType == COLOR_FORMAT_RGB)
    {
      cv::Mat rgb_image(rgbFrameData.frameHeight, rgbFrameData.frameWidth,
                        CV_8UC3, rgbFrameData.pFrameData);
      return rgb_image;
    }
    else
    {
      printf("!!!   Can't Handdl rgbData Type %08x !!!\n",
             rgbFrameData.formatType);
      return cv::Mat();
    }
  }
  else
  {
    printf("!!!   empty rgbData   !!!\n");
    return cv::Mat();
  }
}

std::map<std::string, cv::Mat> SunnyCamera::Camera::get_tof()
{
  std::map<std::string, cv::Mat> result;
  if (tofFrameData.pDepthDataFilter != nullptr)
  {
    cv::Mat depth_image(tofFrameData.frameHeight, tofFrameData.frameWidth,
                        CV_32FC1, tofFrameData.pDepthDataFilter);

    cv::Mat aligned_bgr_image(tofFrameData.frameHeight, tofFrameData.frameWidth,
                              CV_8UC3, tofFrameData.pRgbD);
    cv::Mat aligned_rgb_image;
    cv::cvtColor(aligned_bgr_image, aligned_rgb_image, cv::COLOR_BGR2RGB);

    cv::Mat point_cloud(tofFrameData.frameHeight * tofFrameData.frameWidth, 3,
                        CV_32FC1, tofFrameData.pPointData);
    // Add arrays to the dictionary
    result["depth_image"] = depth_image;
    result["aligned_rgb"] = aligned_rgb_image;
    result["point_cloud"] = point_cloud;
  }
  else
  {
    printf("!!!   empty tofData   !!!\n");
  }
  return result;
};
