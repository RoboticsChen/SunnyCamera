from setuptools import setup, find_packages

setup(
    name='SunnyCamera',
    version='2.0',
    packages=find_packages(),  # 自动查找项目中的包
    install_requires=[
        'numpy',
        'pybind11',
        'pillow',
        'matplotlib',
    ],
    # 其他配置选项
    author='ZhiXing Chen',
    description='driver for sunny camera',
    package_data={'sunny_camera': ['libs/*']},
    url='https://github.com/',
    classifiers=[
        'Programming Language :: Python :: 3',
        # 其他分类器
    ],
)
