import os
from glob import glob

from setuptools import setup, find_packages

package_name = "learning_based_vehicle_calibration"


setup(
    name=package_name,
    version="0.0.0",
    packages=find_packages(exclude=["test"]),
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/" + package_name]),
        ("share/" + package_name, ["package.xml"]),
        # (os.path.join('share', package_name, 'launch'), glob(os.path.join('launch', '*.py'))),
        (
            os.path.join("share", package_name, "launch"),
            glob(os.path.join("launch", "*launch.[pxy][yma]*")),
        ),
        # (f"share/{package_name}/launch", [f"launch/neural_network_launch.py"]),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Cristian Gariboldi",
    maintainer_email="gariboldicristian@gmail.com",
    description="Black box model of longitudinal dynamics",
    license="Apache License 2.0",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [],
    },
)

# ('share/' + package_name + '/launch', ['launch/neural_network_throttle_launch.py']),
