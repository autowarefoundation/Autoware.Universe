from pathlib import Path
import subprocess

package_path = {}
package_path["path"] = str(Path(__file__).parent)
build_cpp_command = "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
build_cpp_command += '-DBUILD_PATH=\\"' + str(Path(__file__).parent) + '\\" '
build_cpp_command += "autoware_vehicle_adaptor/src/vehicle_adaptor_compensator.cpp autoware_vehicle_adaptor/src/vehicle_adaptor_utils.cpp autoware_vehicle_adaptor/src/inputs_prediction.cpp autoware_vehicle_adaptor/src/nominal_dynamics.cpp autoware_vehicle_adaptor/src/transform_vehicle_adaptor_model.cpp autoware_vehicle_adaptor/src/inputs_ref_smoother.cpp "
build_cpp_command += "-o autoware_vehicle_adaptor/src/vehicle_adaptor_compensator$(python3-config --extension-suffix) "
build_cpp_command += (
    "-lrt -I$(pwd)/autoware_vehicle_adaptor/include -I/usr/include/eigen3 -lyaml-cpp"
)
print(build_cpp_command)
subprocess.run(build_cpp_command, shell=True)
