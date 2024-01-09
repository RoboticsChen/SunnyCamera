from .libs import SunnyCamera


def device_init():
    SunnyCamera.device_init()


def Camera(Camera_id):
    return SunnyCamera.Camera(Camera_id)


def device_deinit():
    SunnyCamera.device_deinit()
