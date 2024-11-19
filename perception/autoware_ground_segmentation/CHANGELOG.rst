^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_ground_segmentation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(autoware_ground_segmentation): implementing linear least square fitting for local gradient calculation (`#9116 <https://github.com/autowarefoundation/autoware.universe/issues/9116>`_)
  * refactor: calculate local ground gradient in classifyPointCloudGridScan
  Calculate the local ground gradient by fitting a line to the ground grids in the classifyPointCloudGridScan function. This improves the accuracy of the gradient calculation and ensures more precise extrapolation of the ground height.
  * refactor: calculate local ground gradient in classifyPointCloudGridScan
  * refactor: update ground gradient calculation in classifyPointCloudGridScan function
  * style(pre-commit): autofix
  * chore: rename gradient variables
  * refactor: initialize all the member of the struct GridCenter
  * refactor: fix ground gradient calculation in checkContinuousGndGrid function
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_ground_segmentation): fix scan ground filter logic  (`#9084 <https://github.com/autowarefoundation/autoware.universe/issues/9084>`_)
  * refactor: initialize gnd_grids in ScanGroundFilterComponent::initializeFirstGndGrids
  Initialize gnd_grids vector in the ScanGroundFilterComponent::initializeFirstGndGrids function to ensure it is empty and has the correct capacity. This improves the efficiency of the function and ensures accurate grid initialization.
  * refactor: initialize gnd_grids vector in initializeFirstGndGrids function
  Initialize the gnd_grids vector in the initializeFirstGndGrids function to ensure it is empty and has the correct capacity. This improves the efficiency of the function and ensures accurate grid initialization.
  * refactor: improve efficiency and accuracy of grid initialization
  Initialize the gnd_grids vector in the initializeFirstGndGrids function to ensure it is empty and has the correct capacity. This refactor improves the efficiency of the function and ensures accurate grid initialization.
  * refactor: improve efficiency of checkDiscontinuousGndGrid function
  Refactor the checkDiscontinuousGndGrid function in node.cpp to improve its efficiency. The changes include optimizing the conditional statements and reducing unnecessary calculations.
  * refactor: improve efficiency of checkDiscontinuousGndGrid function
  * fix: add missing condition
  * style(pre-commit): autofix
  * refactor: fix height_max initialization in node.hpp
  * fix: bring back inequality sign
  * fix: parameters from float to double
  following the guideline https://docs.ros.org/en/foxy/Concepts/About-ROS-2-Parameters.html#overview
  * refactor: fix logic description comment
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(autoware_ground_segmentation): scan ground filter refactoring (`#9061 <https://github.com/autowarefoundation/autoware.universe/issues/9061>`_)
  * chore: Add comment classification logic for point cloud grid scan
  * chore: renamed horizontal angle to azimuth angle
  * chore: rename offset to data_index
  * chore: rename ground_cluster to centroid_bin
  chore: Refactor recheckGroundCluster function in scan_ground_filter
  * chore: rename too short variables
  * refactor: set input to be const
  * refactor: update functions to be const
  * chore: reorder params
  * refactor: Add ScanGroundGrid class for managing grid data
  * refactor: Update grid parameters and calculations in ScanGroundGrid class
  * refactor: remove unused methods
  * refactor: classification description
  * refactor: initialize members in ScanGroundGrid class
  * refactor: remove unused value
  * chore: reduce scope
  * refactor: align structure between convertPointcloud and convertPointcloudGridScan
  ---------
* feat(ground_segmentation): add time_keeper (`#8585 <https://github.com/autowarefoundation/autoware.universe/issues/8585>`_)
  * add time_keeper
  * add timekeeper option
  * add autoware_universe_utils
  * fix topic name
  * add scope and timekeeper
  * remove debug code
  * remove some timekeeper and mod block comment
  ---------
* fix(autoware_pointcloud_preprocessor): static TF listener as Filter option (`#8678 <https://github.com/autowarefoundation/autoware.universe/issues/8678>`_)
* fix(ground-segmentation): missing ament_index_cpp dependency (`#8587 <https://github.com/autowarefoundation/autoware.universe/issues/8587>`_)
* fix(autoware_ground_segmentation): fix unusedFunction (`#8566 <https://github.com/autowarefoundation/autoware.universe/issues/8566>`_)
  fix:unusedFunction
* fix(ground_segmentation): missing default parameters ERROR (`#8538 <https://github.com/autowarefoundation/autoware.universe/issues/8538>`_)
  fix(ground_segmentation): remove unused params
* fix(autoware_ground_segmentation): fix unreadVariable (`#8353 <https://github.com/autowarefoundation/autoware.universe/issues/8353>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* perf(autoware_pointcloud_preprocessor): lazy & managed TF listeners (`#8174 <https://github.com/autowarefoundation/autoware.universe/issues/8174>`_)
  * perf(autoware_pointcloud_preprocessor): lazy & managed TF listeners
  * fix(autoware_pointcloud_preprocessor): param names & reverse frames transform logic
  * fix(autoware_ground_segmentation): add missing TF listener
  * feat(autoware_ground_segmentation): change to static TF buffer
  * refactor(autoware_pointcloud_preprocessor): move StaticTransformListener to universe utils
  * perf(autoware_universe_utils): skip redundant transform
  * fix(autoware_universe_utils): change checks order
  * doc(autoware_universe_utils): add docstring
  ---------
* fix(autoware_ground_segmentation): fix uninitMemberVar (`#8336 <https://github.com/autowarefoundation/autoware.universe/issues/8336>`_)
  fix:uninitMemberVar
* fix(autoware_ground_segmentation): fix functionConst (`#8291 <https://github.com/autowarefoundation/autoware.universe/issues/8291>`_)
  fix:functionConst
* refactor(ground_segmentation)!: add package name prefix of autoware\_ (`#8135 <https://github.com/autowarefoundation/autoware.universe/issues/8135>`_)
  * refactor(ground_segmentation): add package name prefix of autoware\_
  * fix: update prefix cmake
  ---------
* Contributors: Amadeusz Szymko, Masaki Baba, Rein Appeldoorn, Taekjin LEE, Yutaka Kondo, badai nguyen, kobayu858

0.26.0 (2024-04-03)
-------------------
