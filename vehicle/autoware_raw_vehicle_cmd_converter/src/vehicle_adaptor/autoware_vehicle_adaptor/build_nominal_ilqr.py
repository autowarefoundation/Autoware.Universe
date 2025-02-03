from pathlib import Path
import subprocess

package_path = {}
package_path["path"] = str(Path(__file__).parent)
build_nominal_ilqr = "g++ -Ofast -Wall -shared -std=c++23 -fPIC $(python3 -m pybind11 --includes) "
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
