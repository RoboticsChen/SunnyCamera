import sunny_camera
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import time

sunny_camera.device_init()
camera1 = sunny_camera.Camera("8830-93A2-132C-524C")
camera2 = sunny_camera.Camera("1270-951B-10E1-347C")
camera1.init()
camera2.init()

rgb1 = np.array(camera1.get_rgb()) / 255.0
# 可视化RGB图像
plt.imshow(rgb1)
plt.axis('off') 
plt.show()

rgb2 = np.array(camera2.get_rgb()) / 255.0
# 可视化RGB图像
plt.imshow(rgb2)
plt.axis('off')
plt.show()

tofData1 = camera1.get_tof()
depth = np.array(tofData1['depth_image'])
# np.savetxt('depth.txt', depth, fmt='%.6f', delimiter=';')
# 可视化DEPTH图像
plt.imshow(depth)
plt.axis('off')
plt.show()
#
aligned_rgb1 = tofData1['aligned_rgb']
plt.imshow(aligned_rgb1)
plt.axis('off')
plt.show()
H, W, _ = aligned_rgb1.shape
aligned_rgb1 = aligned_rgb1.reshape((H * W, 3))
#
point1 = tofData1['point_cloud']
colorred_point1 = np.concatenate([point1, aligned_rgb1], axis=1)
np.savetxt('pointCloud.txt', colorred_point1, fmt='%.6f', delimiter=';')
#
print("Rgb_size: ", camera1.WIDTH("rgb"), "*", camera1.HEIGHT("rgb"))
print("Rgb_intrinsic:\n", camera1.INTRINSIC("rgb"))
print("Depth_size: ", camera1.WIDTH("depth"), "*",
      camera1.HEIGHT("depth"))
print("Depth_intrinsic:\n", camera1.INTRINSIC("depth"))


tofData2 = camera2.get_tof()
depth = np.array(tofData2['depth_image'])
# np.savetxt('depth.txt', depth, fmt='%.6f', delimiter=';')
# 可视化DEPTH图像
plt.imshow(depth)
plt.axis('off')  # 关闭坐标轴
plt.show()
#
aligned_rgb2 = tofData2['aligned_rgb']
plt.imshow(aligned_rgb2)
plt.axis('off')  # 关闭坐标轴
plt.show()
H, W, _ = aligned_rgb2.shape
aligned_rgb2 = aligned_rgb2.reshape((H * W, 3))
#
point2 = tofData2['point_cloud']
colorred_point2 = np.concatenate([point2, aligned_rgb2], axis=1)
np.savetxt('pointCloud.txt', colorred_point2, fmt='%.6f', delimiter=';')
#
print("Rgb_size: ", camera2.WIDTH("rgb"), "*", camera2.HEIGHT("rgb"))
print("Rgb_intrinsic:\n", camera2.INTRINSIC("rgb"))
print("Depth_size: ", camera2.WIDTH("depth"), "*",
      camera2.HEIGHT("depth"))
print("Depth_intrinsic:\n", camera2.INTRINSIC("depth"))

camera1.deinit()
camera2.deinit()
sunny_camera.device_deinit()
