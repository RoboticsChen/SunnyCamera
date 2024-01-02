#include "camera.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

bool g_signalCtrlC = false; // ctrl-c信号
std::mutex frameLock;       // 全局锁
StreamCallBack frameData;

namespace py = pybind11;

//****************************************************************************
//*--------------------------Convert functions-------------------------------*
//****************************************************************************

/*
numpy ->C++ Mat
*/
// cv::Mat numpy_uint8_1c_to_cv_mat(py::array_t<unsigned char> &input)
// {
//     if (input.ndim() != 2)
//         throw std::runtime_error("1-channel image must be 2 dims ");
//     py::buffer_info buf = input.request();
//     cv::Mat mat(buf.shape[0], buf.shape[1], CV_8UC1, (unsigned char *)buf.ptr);
//     return mat;
// }

// cv::Mat numpy_uint8_3c_to_cv_mat(py::array_t<unsigned char> &input)
// {
//     if (input.ndim() != 3)
//         throw std::runtime_error("3-channel image must be 3 dims ");
//     py::buffer_info buf = input.request();
//     cv::Mat mat(buf.shape[0], buf.shape[1], CV_8UC3, (unsigned char *)buf.ptr);
//     return mat;
// }

/*
C++ Mat ->numpy
*/
py::array_t<float> cv_mat_uint8_1c_to_numpy(cv::Mat &input)
{
    py::array_t<float> dst = py::array_t<unsigned char>({input.rows, input.cols}, input.data);
    return dst;
}

py::array_t<unsigned char> cv_mat_uint8_3c_to_numpy(cv::Mat &input)
{
    py::array_t<unsigned char> dst = py::array_t<unsigned char>({input.rows, input.cols, 3}, input.data);
    return dst;
}

// py::array_t<std::complex<float>> cv_mat_float_3c_to_numpy(cv::Mat& input) {
//     py::array_t<std::complex<float>> dst = py::array_t<std::complex<float>>({ input.rows,input.cols,3}, input.data);
//     return dst;
// }

py::array_t<float> mat_to_py(cv::Mat input)
{
    py::buffer_info buffer(
        input.data,
        sizeof(float),
        py::format_descriptor<float>::format(),
        2,
        {input.rows, input.cols},
        {sizeof(float) * input.cols, sizeof(float)});
    return py::array_t<float>(buffer);
}

//****************************************************************************
//*------------------------------Camera Type---------------------------------*
//****************************************************************************
class Camera
{
private:
    int cameraType = 2;
    bool inited;
    // 声明数据变量
    Intrinsic camera_INTRINSIC;
    Distortion camera_DISTORTION;
    TofFrameData tofFrameData;
    RgbFrameData rgbFrameData;

public:
    // 设备
    HTOFD hTofD;
    Camera()
    {
        inited = false;
    }

    bool init()
    {
        printf("******************Camera Initializing**********************\n");
        signal(SIGINT, HandleSignal);
        frameData.inited = false;

        TofDevInitParam struInitParam;
        memset(&struInitParam, 0, sizeof(struInitParam));
        strncpy(struInitParam.szDepthCalcCfgFileDir, "./parameter", sizeof(struInitParam.szDepthCalcCfgFileDir) - 1);
        struInitParam.bSupUsb = true;
        struInitParam.bSupNetWork = false;
        struInitParam.bSupSerialCOM = false;
        if (struInitParam.bSupSerialCOM)
        {
#ifdef WIN32
            // strncpy(struInitParam.szSerialDev, "COM1", sizeof(struInitParam.szSerialDev));//windows下可以不用赋值
#elif defined LINUX
            strncpy(struInitParam.szSerialDev, "/dev/ttyUSB0", sizeof(struInitParam.szSerialDev)); // linux下必须赋值一个实际使用的串口设备，这里随便写了一个
#endif
        }
        struInitParam.bWeakAuthority = false;
        struInitParam.bDisablePixelOffset = false;
        strncpy(struInitParam.szLogFile, "./tof_dev_sdk_log.txt", sizeof(struInitParam.szLogFile)); // 不赋值则不记录日志到文件
        struInitParam.nLogFileMaxSize = 10 * 1024 * 1024;
        TOFD_Init(&struInitParam);

        printf("SDK Version: %s.\n", TOFD_GetSDKVersion());

        TofDeviceDescriptor *pDevsDescList = NULL;
        UINT32 dev_num = 0;
        TOFD_SearchDevice(&pDevsDescList, &dev_num);
        if (dev_num < 0)
        {
            printf("can not find tof device!\n");
        }
        else
        {
            const UINT32 dev_index = ChoseDev(dev_num) - 1; // 决定测试哪一个设备
            hTofD = TOFD_OpenDevice(pDevsDescList + dev_index, CallBackTofDeviceStatus, NULL);
            if (NULL == hTofD)
            {
                printf("Open Tof Device failed.\n");
                return false;
            }
            else
            {
                std::cout << "Begin update frame!!!" << std::endl;
                std::thread thread_update([this]()
                                          { updateThread(); });

                std::string strSaveDir = (""); //(".");//用于保存文件的目录，可以自己指定，为空则表示不保存文件
                std::thread thread_capture(captureThread, hTofD, cameraType, strSaveDir);

                while (!frameData.inited)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 单位是毫秒
                }
                init_intrinsic_distortion();
                thread_capture.detach();
                thread_update.detach();
                // std::thread thread_test([this]()
                //                           { testThread(); });
                // thread_test.join();
                inited = true;
                return true;
            }
        }
    }

    void updateThread()
    {
        signal(SIGINT, HandleSignal);
        // std::cout << "updating.." << std::flush;
        while ((!g_signalCtrlC))
        {
            std::lock_guard<std::mutex>
                lock(frameLock);
            tofFrameData = frameData.tofFrameData;
            rgbFrameData = frameData.rgbFrameData;
            // std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 单位是毫秒
            // std::cout << "." << std::flush;
        }
    }

    void init_intrinsic_distortion()
    {
        if (cameraType == 1)
        {
            RgbModuleLensGeneral *rgbGeneral = &(frameData.rgbDeviceParam.uParam.general);
            cv::Mat rgbI = cv::Mat::zeros(3, 3, CV_32F);
            rgbI.at<float>(0, 0) = rgbGeneral->fx;
            rgbI.at<float>(0, 1) = 0;
            rgbI.at<float>(0, 2) = rgbGeneral->cx;
            rgbI.at<float>(1, 0) = 0;
            rgbI.at<float>(1, 1) = rgbGeneral->fy;
            rgbI.at<float>(1, 2) = rgbGeneral->cy;
            rgbI.at<float>(2, 0) = 0;
            rgbI.at<float>(2, 1) = 0;
            rgbI.at<float>(2, 2) = 1;
            camera_INTRINSIC.rgb_INTRINSIC = rgbI;
            cv::Mat rgbD = cv::Mat::zeros(1, 5, CV_32F);
            rgbD.at<float>(0, 0) = rgbGeneral->k1;
            rgbD.at<float>(0, 1) = rgbGeneral->k2;
            rgbD.at<float>(0, 2) = rgbGeneral->p1;
            rgbD.at<float>(0, 3) = rgbGeneral->p2;
            rgbD.at<float>(0, 4) = rgbGeneral->k3;
            camera_DISTORTION.rgb_DISTORTION = rgbD;

            // // depth intrinsic
            TofModuleLensGeneral *tofGeneral = &(frameData.tofDeviceParam.uParam.general);
            cv::Mat depthI = cv::Mat::zeros(3, 3, CV_32F);
            depthI.at<float>(0, 0) = tofGeneral->fx;
            depthI.at<float>(0, 1) = 0;
            depthI.at<float>(0, 2) = tofGeneral->cx;
            depthI.at<float>(1, 0) = 0;
            depthI.at<float>(1, 1) = tofGeneral->fy;
            depthI.at<float>(1, 2) = tofGeneral->cy;
            depthI.at<float>(2, 0) = 0;
            depthI.at<float>(2, 1) = 0;
            depthI.at<float>(2, 2) = 1;
            camera_INTRINSIC.depth_INTRINSIC = depthI;
            cv::Mat depthD = cv::Mat::zeros(1, 5, CV_32F);
            depthD.at<float>(0, 0) = tofGeneral->k1;
            depthD.at<float>(0, 1) = tofGeneral->k2;
            depthD.at<float>(0, 2) = tofGeneral->p1;
            depthD.at<float>(0, 3) = tofGeneral->p2;
            depthD.at<float>(0, 4) = tofGeneral->k3;
            camera_DISTORTION.depth_DISTORTION = depthD;
        }
        else if (cameraType == 2)
        {

            RgbModuleLensFishEye *rgbFish = &(frameData.rgbDeviceParam.uParam.fishEye);
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
            camera_INTRINSIC.rgb_INTRINSIC = rgbI;
            cv::Mat rgbD = cv::Mat::zeros(1, 4, CV_32F);
            rgbD.at<float>(0, 0) = rgbFish->k1;
            rgbD.at<float>(0, 1) = rgbFish->k2;
            rgbD.at<float>(0, 2) = rgbFish->k3;
            rgbD.at<float>(0, 3) = rgbFish->k4;
            camera_DISTORTION.rgb_DISTORTION = rgbD;

            // // depth intrinsic
            TofModuleLensFishEye *tofFish = &(frameData.tofDeviceParam.uParam.fishEye);
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
            camera_INTRINSIC.depth_INTRINSIC = depthI;
            cv::Mat depthD = cv::Mat::zeros(1, 4, CV_32F);
            depthD.at<float>(0, 0) = tofFish->k1;
            depthD.at<float>(0, 1) = tofFish->k2;
            depthD.at<float>(0, 2) = tofFish->k3;
            depthD.at<float>(0, 3) = tofFish->k4;
            camera_DISTORTION.depth_DISTORTION = depthD;
        }
        std::cout << "intrinsic initialized !!!" << std::endl;
        printf("*********************init success!*********************\n");
    }

    bool deinit()
    {
        TOFD_CloseDevice(hTofD);
        TOFD_Uninit();
        inited = false;
        return true;
    }

    int WIDTH(std::string image_type = "rgb") const
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

    int HEIGHT(std::string image_type = "rgb") const
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

    py::array_t<float> INTRINSIC(std::string image_type = "rgb") const
    {
        py::array_t<float> result;
        if (image_type == "rgb")
        {
            result = mat_to_py(camera_INTRINSIC.rgb_INTRINSIC);
        }
        else if (image_type == "depth")
        {
            result = mat_to_py(camera_INTRINSIC.depth_INTRINSIC);
        }
        else
        {
            printf("Error image_type to getINTRINSIC !!!");
        }
        return result;
    }

    py::array_t<float> DISTORTION(std::string image_type = "rgb") const
    {
        py::array_t<float> result;
        if (image_type == "rgb")
        {
            result = mat_to_py(camera_DISTORTION.rgb_DISTORTION);
        }
        else if (image_type == "depth")
        {
            result = mat_to_py(camera_DISTORTION.depth_DISTORTION);
        }
        else
        {
            printf("Error image_type to getDISTORTION !!!");
        }
        return result;
    }

    py::array_t<float> get_rgb()
    {
        py::array_t<float> result;
        if (rgbFrameData.pFrameData != nullptr)
        {
            cv::Mat rgb_image(rgbFrameData.frameHeight,
                              rgbFrameData.frameWidth,
                              CV_8UC3,
                              rgbFrameData.pFrameData);
            result = cv_mat_uint8_3c_to_numpy(rgb_image);
        }
        else
        {
            printf("!!!   empty rgbData   !!!\n");
        }
        return result;
    }

    py::dict get_tof()
    {
        py::dict result;
        if (tofFrameData.pDepthDataFilter != nullptr)
        {
            cv::Mat depth_image(tofFrameData.frameHeight,
                                tofFrameData.frameWidth,
                                CV_32FC1,
                                tofFrameData.pDepthDataFilter);

            cv::Mat aligned_bgr_image(tofFrameData.frameHeight,
                                      tofFrameData.frameWidth,
                                      CV_8UC3,
                                      tofFrameData.pRgbD);
            cv::Mat aligned_rgb_image;
            cv::cvtColor(aligned_bgr_image, aligned_rgb_image, cv::COLOR_BGR2RGB);

            cv::Mat point_cloud(tofFrameData.frameHeight * tofFrameData.frameWidth,
                                3,
                                CV_32FC1,
                                tofFrameData.pPointData);
            // Add arrays to the dictionary
            result["depth_image"] = mat_to_py(depth_image);
            result["aligned_rgb"] = cv_mat_uint8_3c_to_numpy(aligned_rgb_image);
            result["point_cloud"] = mat_to_py(point_cloud);
        }
        else
        {
            printf("!!!   empty tofData   !!!\n");
        }
        return result;
    }

    // void testThread()
    // {
    //     int operate;
    //     cv::Mat rgb_image;
    //     cv::Mat depth_image;
    //     cv::Mat point_cloud;
    //     while (!g_signalCtrlC)
    //     {
    //         signal(SIGINT, HandleSignal);
    //         std::cout << "which operate do you want?" << std::endl;
    //         std::cin >> operate;
    //         switch (operate)
    //         {
    //         case 1:
    //         {
    //             rgb_image = get_rgb();
    //             cv::namedWindow("Display Window", cv::WINDOW_NORMAL);
    //             // OPENCV默认以BGR显示图片,因此先将图片的通道转换一下
    //             cv::Mat bgr_image;
    //             cv::cvtColor(rgb_image, bgr_image, cv::COLOR_BGR2RGB);
    //             cv::imshow("rgb image", bgr_image);
    //             cv::resizeWindow("Resized Window", 960, 540);
    //             std::cout << "rgb image_size:" << rgb_image.cols << "x" << rgb_image.rows << "x" << rgb_image.channels() << std::endl;
    //             // 关闭窗口
    //             cv::waitKey(0);
    //             cv::destroyAllWindows();
    //             break;
    //         };
    //         case 2:
    //         {
    //             depth_image = get_depth();
    //             cv::resize(depth_image, depth_image, cv::Size(896, 436)); // 调整为更大的尺寸
    //             std::cout << "Rows: " << depth_image.rows << std::endl;
    //             std::cout << "Cols: " << depth_image.cols << std::endl;
    //             cv::namedWindow("Display Window", cv::WINDOW_NORMAL);
    //             cv::Mat depth_color_image;
    //             cv::applyColorMap(depth_image, depth_color_image, cv::COLORMAP_JET);
    //             cv::imshow("depth image", depth_image);
    //             cv::imshow("Depth Color Map", depth_color_image);
    //             // cv::resizeWindow("Resized Window", 960, 540);
    //             std::cout << "depth image_size:" << depth_image.cols << "x" << depth_image.rows << "x" << depth_image.channels() << std::endl;
    //             cv::waitKey(0);
    //             cv::destroyAllWindows();
    //             break;
    //         };
    //         case 3:
    //         {
    //             point_cloud = get_pointCloud();
    //             std::cout << "Rows: " << point_cloud.rows << std::endl;
    //             std::cout << "Cols: " << point_cloud.cols << std::endl;
    //             break;
    //         };
    //         case 4:
    //         {
    //             int width = WIDTH();
    //             int height = HEIGHT();
    //             cv::Mat Intrinsic = INTRINSIC();
    //             std::cout << "RGB_Frame:  width_" << width << "\t height_" << height << "\n"
    //                       << "RGB_Intrinsic:\n"
    //                       << Intrinsic << "\n"
    //                       << std::endl;

    //             width = WIDTH("depth");
    //             height = HEIGHT("depth");
    //             Intrinsic = INTRINSIC("depth");
    //             std::cout << "DEPTH_Frame:  width_" << width << "\t height_" << height << "\n"
    //                       << "DEPTH_Intrinsic:\n"
    //                       << Intrinsic << "\n"
    //                       << std::endl;
    //             break;
    //         };
    //         default:
    //         {
    //             continue;
    //         };
    //         }
    //     }
    // }
};

// int main()
// {
//     Camera airbot_camera(1);
//     airbot_camera.init();
// }
//     int camera_type;
//     frameData.inited = false;
//     signal(SIGINT, HandleSignal);
//     std::cout << "input type of the camera: ";
//     std::cin >> camera_type;
//     Camera airbot_camera(camera_type);
//     airbot_camera.init();

//     std::cout << "Begin update frame!!!" << std::endl;
//     std::thread thread_update([&airbot_camera]()
//                               { airbot_camera.updateThread(); });

//     std::string strSaveDir = (""); //(".");//用于保存文件的目录，可以自己指定，为空则表示不保存文件
//     std::thread thread_capture(captureThread, airbot_camera.hTofD, camera_type, strSaveDir);

//     while (!frameData.inited)
//     {
//         std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 单位是毫秒
//     }
//     airbot_camera.init_intrinsic_distortion();

//     // test
//     std::thread thread_test([&airbot_camera]()
//                             { airbot_camera.testThread(); });

//     thread_capture.join();
//     thread_update.join();
// }

PYBIND11_MODULE(sunny_camera, m)
{
    m.doc() = "sunny_camera";

    py::class_<Camera>(m, "Camera")
        .def(py::init())
        .def("WIDTH", &Camera::WIDTH)
        .def("HEIGHT", &Camera::HEIGHT)
        .def("INTRINSIC", &Camera::INTRINSIC)
        .def("DISTORTION", &Camera::DISTORTION)
        .def("init", &Camera::init)
        .def("deinit", &Camera::deinit)
        .def("get_rgb", &Camera::get_rgb)
        .def("get_tof", &Camera::get_tof);
}