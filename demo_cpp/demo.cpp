#include <SunnyCamera/include/sunny_camera.hpp>
#include <chrono>

#include "base64.h"

using namespace SunnyCamera;

int main()
{
  device_init();
  Camera camera("8830-93A2-132C-524C");
  // Camera camera("1270-951B-10E1-347C");
  camera.init();
  cv::Mat rgb_image = camera.get_rgb();

  // Front-end transcoding
  // std::vector<uchar> buffer;
  // cv::imencode(".jpg", rgb_image, buffer);
  // std::string base64_encoded = base64_encode(buffer.data(), buffer.size());
  // // std::cout << "Base64 Encoded Image:\n" << base64_encoded << std::endl;

  std::map<std::string, cv::Mat> tof_data = camera.get_tof();
  cv::Mat depth_image = tof_data["depth_image"];
  cv::Mat aligned_rgb = tof_data["aligned_rgb"];
  cv::Mat point_cloud = tof_data["point_cloud"];
  cv::Mat intrinsic = camera.INTRINSIC("rgb");
  int rgb_width = camera.WIDTH("rgb");
  int depth_width = camera.WIDTH("depth");
  int rgb_height = camera.HEIGHT("rgb");
  int depth_height = camera.HEIGHT("depth");

  cv::namedWindow("Live Video", cv::WINDOW_NORMAL);
  cv::moveWindow("Live Video", 100, 100);

  // 记录帧率相关的变量
  int frameCount = 0;
  auto start = std::chrono::high_resolution_clock::now();

  while (true)
  {
    cv::Mat frame_rgb = camera.get_rgb();
    cv::Mat frame_bgr;
    cv::cvtColor(frame_rgb, frame_bgr, cv::COLOR_BGR2RGB);
    cv::imshow("Live Video", frame_bgr);
    frameCount++;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    if (duration >= 1)
    {
      double fps = frameCount / static_cast<double>(duration);
      std::cout << "FPS: " << fps << std::endl;
      frameCount = 0;
      start = std::chrono::high_resolution_clock::now();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(17));
    char key = cv::waitKey(1);
    if (key == 27)
    { // ESC键退出
      break;
    }
  }
  cv::destroyAllWindows();

  // 输出或可视化
  cv::cvtColor(rgb_image, rgb_image, cv::COLOR_BGR2RGB);
  cv::imshow("RGB Image", rgb_image);
  cv::imshow("Depth Image", depth_image);
  cv::cvtColor(aligned_rgb, aligned_rgb, cv::COLOR_BGR2RGB);
  cv::imshow("Aligned RGB Image", aligned_rgb);

  // 输出一些文本信息
  std::cout << "Intrinsic Matrix (RGB):" << std::endl
            << intrinsic << std::endl;
  std::cout << "RGB Width:  " << rgb_width << ", Height: " << rgb_height
            << std::endl;
  std::cout << "Depth Width: " << depth_width << ", Height: " << depth_height
            << std::endl;

  // 等待用户按键
  cv::waitKey(0);

  // 保存点云为txt文件
  cv::FileStorage fs("point_cloud.txt", cv::FileStorage::WRITE);
  fs << "point_cloud" << point_cloud;
  fs.release();

  printf("point_cloud data saved to \"point_cloud.txt\"...\n");

  device_deinit();
  return 0;
}
