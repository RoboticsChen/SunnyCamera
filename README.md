# Sunny camera driver
## Requirements
```shell
# opencv 
git clone https://github.com/opencv/opencv
cd opencv
mkdir build && cd build
cmake ..
make -j12
sudo make install
cd ..
```
```shell
# pybind （ when python shared libraries is wanted）
git clone https://github.com/pybind/pybind11.git
cd pybind11
mkdir build && cd build
cmake ..
make -j12
sudo make install
cd ..
```
## Installation

```shell
git clone https://github.com/RoboticsChen/Sunny_camera.git
cd Sunny_camera
chmod +x *.sh
```

#### C++ shared libraries only
```shell
. build.sh         # default C++ shared libraries only.
sudo ./install.sh
```

#### C++ shared libraries and python shared libraries
```shell
conda activate your_python_env  
. build.sh -p     # -p option to build Python shared libraries additionally.
sudo ./install.sh
pip install -e .
```
>Note:
>● Use command (. build.sh) instead of (./build.sh), otherwise you should open a new terminal for path set activate. 
>●If build fails, check whether python_env is the desired environment for installing the driver.

## demos
Use following command to verify installation

#### C++ version
```shell
cd demo_cpp
mkdir build && cd build
cmake ..
make -j12
./demo_cpp
```

#### Python version
```shell
cd demo_python
python demo_singleCam.py
```
> remember to specify camera_id accordingly

## C++ API Reference
### `namespace`
  - `SunnyCamera::`

### `Functions`
  - `void device_init()` : search and open searched sunny cameras
  - `void device_deinit()` : close all openned sunny cameras

### `Camera`
- **Attributes**:
  - `int WIDTH(std::string img_type)`: The horizontal width (x-axis) of RGB and depth camera in pixels.
    - The "img_type" can be either "rgb" or "depth," with the default value "rgb" (str type). 
  - `int HEIGHT(std::string img_type)`: The vertical height (y-axis) of RGB and depth camera in pixels.
  - `cv::Mat INTRINSIC(std::string img_type)`: The 3x3 intrinsic matrix of the cameras.
  - `cv::Mat DISTORTION(std::string img_type)`: The 1x4(or 1x5) distortion vector of the cameras.
- **Methods**:
  - `Camera(std::string camera_id)`: Initializes a camera object with the provided camera ID.
  - `~Camera()`: Releases the resources occupied by the camera object
  - `bool init()`: Initialize the backend of the AirBot camera.`True` when successful, otherwise `False`.
  - `bool deinit()`: Uninitialize the camera.
  - `cv::Mat get_rgb()`: Capture and return the RGB image(H\*W\*3).
  - `std::map<std::string, cv::Mat> get_tof()`: Capture and return the captured TOF data.
      - `"depth_image"`: `cv::Mat` - The depth image(H\*W).
      - `"aligned_rgb"`: `cv::Mat` - The aligned RGB image(H\*W\*3).
      - `"point_cloud"`: `cv::Mat` - The point cloud data(N\*3).

## Python API Reference

### `Functions`
  - ` device_init()` : search and open searched sunny cameras
  - ` device_deinit()` : close all openned sunny cameras

### `Camera`
- **Attributes**:
  - `WIDTH(img_type)`: The horizontal width (x-axis) of RGB and depth camera in pixels.
    - The "img_type" can be either "rgb" or "depth," with the default value "rgb" (str type). 
  - `HEIGHT(img_type)`: The vertical height (y-axis) of RGB and depth camera in pixels.
  - `INTRINSIC(img_type)`: The 3x3 intrinsic matrix of the cameras.
  - `DISTORTION(img_type)`: The 1x4(or 1x5) distortion vector of the cameras.
- **Methods**:
  - `__init__(self, camera_id)`: Initializes a camera object with the provided camera ID (str type).
  - `init(self)`: Initialize the backend of the AirBot camera. `True` when successful, otherwise `False`.
  - `deinit(self)`: Uninitialize the camera.
  - `get_rgb(self)`: Capture and return the color image.
    - Returns: `np.ndarray` - The captured RGB image(H\*W\*3).
  - `get_tof(self)`: Capture and return the depth image.
    - Returns: `Dict[str, np.ndarray]` - The captured TOF data.
      - `"depth_image"`: `np.ndarray` - The depth image(H\*W).
      - `"aligned_rgb"`: `np.ndarray` - The aligned RGB image(H\*W\*3).
      - `"point_cloud"`: `np.ndarray` - The point cloud data(N\*3).