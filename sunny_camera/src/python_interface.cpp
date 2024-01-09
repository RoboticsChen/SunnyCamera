#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "sunny_camera.hpp"

namespace py = pybind11;

//****************************************************************************
//*--------------------------Convert functions-------------------------------*
//****************************************************************************

py::array_t<float> cv_mat_uint8_1c_to_numpy(cv::Mat &input)
{
  py::array_t<float> dst =
      py::array_t<unsigned char>({input.rows, input.cols}, input.data);
  return dst;
}

py::array_t<unsigned char> cv_mat_uint8_3c_to_numpy(cv::Mat &input)
{
  py::array_t<unsigned char> dst =
      py::array_t<unsigned char>({input.rows, input.cols, 3}, input.data);
  return dst;
}

py::array_t<float> mat_to_py(cv::Mat input)
{
  py::buffer_info buffer(
      input.data, sizeof(float), py::format_descriptor<float>::format(), 2,
      {input.rows, input.cols}, {sizeof(float) * input.cols, sizeof(float)});
  return py::array_t<float>(buffer);
}

namespace SunnyCamera
{
  PYBIND11_MODULE(SunnyCamera, m)
  {
    m.doc() = "SunnyCamera";
    m.def("device_init", &device_init, "init func: should be called first");
    m.def("device_deinit", &device_deinit, "deinit func: should be called final");
    py::class_<Camera>(m, "Camera")
        .def(py::init<std::string>())
        .def("init", &Camera::init)
        .def("deinit", &Camera::deinit)
        .def("WIDTH", &Camera::WIDTH)
        .def("HEIGHT", &Camera::HEIGHT)
        .def("INTRINSIC",
             [](Camera &self, const std::string image_type)
             { return mat_to_py(self.INTRINSIC(image_type)); })
        .def("DISTORTION",
             [](Camera &self, const std::string image_type)
             { return mat_to_py(self.DISTORTION(image_type)); })
        .def("get_rgb",
             [](Camera &self)
             {
               cv::Mat rgbData = self.get_rgb();
               py::array_t<float> result;
               result = cv_mat_uint8_3c_to_numpy(rgbData);
               return result; })
        .def("get_tof", [](Camera &self)
             {
               std::map<std::string, cv::Mat> tofData = self.get_tof();
               py::dict result;
               result["depth_image"] = mat_to_py(tofData["depth_image"]);
               result["aligned_rgb"] =
                   cv_mat_uint8_3c_to_numpy(tofData["aligned_rgb"]);
               result["point_cloud"] = mat_to_py(tofData["point_cloud"]);
               return result; });
  }
} // namespace SunnyCamera