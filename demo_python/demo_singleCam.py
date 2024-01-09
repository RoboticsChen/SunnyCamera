import sunny_camera
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

sunny_camera.device_init()
aircamera = sunny_camera.Camera("8830-93A2-132C-524C")  #改成对应的相机ID
# aircamera = sunny_camera.Camera("1270-951B-10E1-347C")
aircamera.init()

rgb = np.array(aircamera.get_rgb()) / 255.0
# 可视化RGB图像
plt.imshow(rgb)
plt.axis('off') 
plt.show()

tofData = aircamera.get_tof()
depth = np.array(tofData['depth_image'])
# np.savetxt('depth.txt', depth, fmt='%.6f', delimiter=';')

# 可视化DEPTH图像
plt.imshow(depth)
plt.axis('off')
plt.show()


aligned_rgb = tofData['aligned_rgb']
plt.imshow(aligned_rgb)
plt.axis('off')
plt.show()
H, W, _ = aligned_rgb.shape
aligned_rgb = aligned_rgb.reshape((H * W, 3))


point = tofData['point_cloud']
colored_point = np.concatenate([point, aligned_rgb], axis=1)
np.savetxt('pointCloud.txt', colored_point, fmt='%.6f', delimiter=';')

print("Rgb_size: ", aircamera.WIDTH("rgb"), "*", aircamera.HEIGHT("rgb"))
print("Rgb_intrinsic:\n", aircamera.INTRINSIC("rgb"))
print("Depth_size: ", aircamera.WIDTH("depth"), "*",
      aircamera.HEIGHT("depth"))
print("Depth_intrinsic:\n", aircamera.INTRINSIC("depth"))

aircamera.deinit()
sunny_camera.device_deinit()
