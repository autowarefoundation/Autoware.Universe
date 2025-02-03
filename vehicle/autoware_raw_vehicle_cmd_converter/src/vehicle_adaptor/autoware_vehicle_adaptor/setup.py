# cspell: ignore numba
import glob
import json
from pathlib import Path
import subprocess

from setuptools import find_packages
from setuptools import setup

SKIP_PRE_INSTALL_FLAG = False

if not SKIP_PRE_INSTALL_FLAG:
    package_path = {}
    package_path["path"] = str(Path(__file__).parent)
    with open("autoware_vehicle_adaptor/package_path.json", "w") as f:
        json.dump(package_path, f)
    build_cpp_command = (
        "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
    )
    build_cpp_command += '-DBUILD_PATH=\\"' + str(Path(__file__).parent) + '\\" '
    build_cpp_command += "autoware_vehicle_adaptor/src/vehicle_adaptor_compensator.cpp autoware_vehicle_adaptor/src/vehicle_adaptor_utils.cpp autoware_vehicle_adaptor/src/inputs_prediction.cpp autoware_vehicle_adaptor/src/nominal_dynamics.cpp autoware_vehicle_adaptor/src/transform_vehicle_adaptor_model.cpp autoware_vehicle_adaptor/src/inputs_ref_smoother.cpp "
    build_cpp_command += "-o autoware_vehicle_adaptor/src/vehicle_adaptor_compensator$(python3-config --extension-suffix) "
    build_cpp_command += (
        "-lrt -I$(pwd)/autoware_vehicle_adaptor/include -I/usr/include/eigen3 -lyaml-cpp"
    )
    print(build_cpp_command)
    subprocess.run(build_cpp_command, shell=True)

    build_actuation_map_2d_command = (
        "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
    )
    build_actuation_map_2d_command += "autoware_vehicle_adaptor/src/actuation_map_2d.cpp "
    build_actuation_map_2d_command += (
        "-o autoware_vehicle_adaptor/src/actuation_map_2d$(python3-config --extension-suffix) "
    )
    build_actuation_map_2d_command += "-lrt -I/usr/include/eigen3 -lyaml-cpp"
    print(build_actuation_map_2d_command)
    subprocess.run(build_actuation_map_2d_command, shell=True)

    build_nominal_ilqr = (
        "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
    )
    build_nominal_ilqr += '-DBUILD_PATH=\\"' + str(Path(__file__).parent) + '\\" '
    build_nominal_ilqr += "autoware_vehicle_adaptor/controller/nominal_ilqr.cpp autoware_vehicle_adaptor/src/nominal_dynamics.cpp "
    build_nominal_ilqr += (
        "-o autoware_vehicle_adaptor/controller/nominal_ilqr$(python3-config --extension-suffix) "
    )
    build_nominal_ilqr += (
        "-lrt -I$(pwd)/autoware_vehicle_adaptor/include -I/usr/include/eigen3 -lyaml-cpp"
    )
    print(build_nominal_ilqr)
    subprocess.run(build_nominal_ilqr, shell=True)

    build_inputs_prediction_command = (
        "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
    )
    build_inputs_prediction_command += '-DBUILD_PATH=\\"' + str(Path(__file__).parent) + '\\" '
    build_inputs_prediction_command += "autoware_vehicle_adaptor/src/inputs_prediction.cpp autoware_vehicle_adaptor/src/vehicle_adaptor_utils.cpp "
    build_inputs_prediction_command += (
        "-o autoware_vehicle_adaptor/src/inputs_prediction$(python3-config --extension-suffix) "
    )
    build_inputs_prediction_command += (
        "-lrt -I$(pwd)/autoware_vehicle_adaptor/include -I/usr/include/eigen3 -lyaml-cpp"
    )
    print(build_inputs_prediction_command)
    subprocess.run(build_inputs_prediction_command, shell=True)

vehicle_adaptor_compensator_so_path = (
    "src/"
    + glob.glob("autoware_vehicle_adaptor/src/vehicle_adaptor_compensator.*.so")[0].split("/")[-1]
)
actuation_map_so_path = (
    "src/" + glob.glob("autoware_vehicle_adaptor/src/actuation_map_2d.*.so")[0].split("/")[-1]
)
nominal_ilqr_so_path = (
    "controller/"
    + glob.glob("autoware_vehicle_adaptor/controller/nominal_ilqr.*.so")[0].split("/")[-1]
)
inputs_prediction_so_path = (
    "src/" + glob.glob("autoware_vehicle_adaptor/src/inputs_prediction.*.so")[0].split("/")[-1]
)
subprocess.run("python3 generate_butterworth_coef.py", shell=True)
setup(
    name="autoware_vehicle_adaptor",
    version="1.0.0",
    packages=find_packages(),
    package_data={
        "autoware_vehicle_adaptor": [
            "package_path.json",
            vehicle_adaptor_compensator_so_path,
            actuation_map_so_path,
            nominal_ilqr_so_path,
            inputs_prediction_so_path,
        ],
    },
)
