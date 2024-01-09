#include "camera.hpp"

//*----------------------------Device Initialize-----------------------------*
namespace SunnyCamera {
  void device_init();
  void device_deinit();

  //*------------------------------Camera Class--------------------------------*
  class Camera
  {
  private:
    // 设备
    HTOFD hTofD;
    TofDeviceInfo pCaps;
    UINT32 camera_index;
    std::string camera_id;
    // 声明数据变量
    bool inited;
    Intrinsic cameraIntrinsic;
    Distortion cameraDistortion;
    TofFrameData tofFrameData;
    RgbFrameData rgbFrameData;

  protected:
    void init_intrinsic_distortion();
    void updateThread();

  public:
    Camera(std::string target_id);
    ~Camera();
    bool init();
    bool deinit();

    int WIDTH(std::string image_type = "rgb") const;
    int HEIGHT(std::string image_type = "rgb") const;
    cv::Mat INTRINSIC(std::string image_type = "rgb");
    cv::Mat DISTORTION(std::string image_type = "rgb");
    cv::Mat get_rgb();
    std::map<std::string, cv::Mat> get_tof();
};
}  // namespace SunnyCamera
