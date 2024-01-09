import cv2
from matplotlib import pyplot as plt
import numpy as np
import sunny_camera

camera = sunny_camera.Camera()
camera.init()

# 读取鱼眼图像
image = camera.get_rgb() 
image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
print("image:\n",image)

# 相机标定参数（需要根据实际情况调整）
K = camera.INTRINSIC("rgb")
print("INTRINSIC:\n", K)
D = camera.DISTORTION("rgb")
print("DISTORTION:\n", D)
# 畸变校正
undistorted_image = cv2.fisheye.undistortImage(image, K, D)
print("undistorted_image",undistorted_image)

# 显示原始和校正后的图像
cv2.imshow('Original Image', image / 255.0)
cv2.imshow('Undistorted Image', undistorted_image*1000)
cv2.waitKey(0)
cv2.destroyAllWindows()

camera.deinit()
