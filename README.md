# Sunny camera driver

## Installation

```shell
# git clone https://...
cd sunny_camera
chmod +x *.sh
. build.sh     
sudo ./install.sh
pip install -e .
```
>note: use command (. build.sh) instead of (./build.sh), otherwise you should source ~/.bashrc before next step

# Python API Reference

## `Camera`
- **Attributes**:
  - `WIDTH`: The horizontal width (x-axis) of RGB and depth camera in pixels.
  - `HEIGHT`: The vertical height (y-axis) of RGB and depth camera in pixels.
  - `INTRINSIC`: The 3x3 intrinsic parameter matrix of the cameras.
- **Methods**:
  - `init()`: Initialize the backend of the AirBot camera.
    - Returns: `bool` - `True` if initialization is successful, otherwise `False`.
  - `deinit()`: Uninitialize the camera.
    - Returns: `bool` - `True` if deinitialization is successful, otherwise `False`.
  - `get_rgb()`: Capture and return the color image.
    - Returns: `np.ndarray` - The captured RGB image.
  - `get_tof()`: Capture and return the depth image.
    - Returns: `Dict[str, np.ndarray]` - The captured TOF data.
      - `"depth_image"`: `np.ndarray` - The depth image.
      - `"aligned_rgb"`: `np.ndarray` - The aligned RGB image.
      - `"point_cloud"`: `np.ndarray` - The point cloud data.