from pathlib import Path
import subprocess

package_path = {}
package_path["path"] = str(Path(__file__).parent)
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
