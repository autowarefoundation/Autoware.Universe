from pathlib import Path
import subprocess

package_path = {}
package_path["path"] = str(Path(__file__).parent)
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
