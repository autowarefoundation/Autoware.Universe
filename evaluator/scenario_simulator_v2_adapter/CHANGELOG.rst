^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package diagnostic_converter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* Contributors: Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* fix(diagnostic_converter): move headers to a separate directory (`#5943 <https://github.com/autowarefoundation/autoware.universe/issues/5943>`_)
  * fix(diagnostic_converter): move headers to a separate directory
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(diagnostic_converter): remove unit and blank in the value (`#3151 <https://github.com/autowarefoundation/autoware.universe/issues/3151>`_)
  * feat(diagnostic_converter): remove unit and blank in the value
  * fix
  ---------
* feat(diagnostic_converter): apply regex for topic name (`#3149 <https://github.com/autowarefoundation/autoware.universe/issues/3149>`_)
* feat(diagnostic_converter): add converter to use planning_evaluator's output for scenario's condition (`#2514 <https://github.com/autowarefoundation/autoware.universe/issues/2514>`_)
  * add original diagnostic_convertor
  * add test
  * fix typo
  * delete file
  * change include
  * temp
  * delete comments
  * made launch for converter
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * add diagnostic convertor in launch
  * ci(pre-commit): autofix
  * change debug from info
  * change arg name to launch diagnostic convertor
  * add planning_evaluator launcher in simulator.launch.xml
  * fix arg wrong setting
  * style(pre-commit): autofix
  * use simulation msg in tier4_autoware_msgs
  * style(pre-commit): autofix
  * fix README
  * style(pre-commit): autofix
  * refactoring
  * style(pre-commit): autofix
  * remove unnecessary dependency
  * remove unnecessary dependency
  * move folder
  * reformat
  * style(pre-commit): autofix
  * Update evaluator/diagnostic_converter/include/converter_node.hpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/README.md
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/src/converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/test/test_converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * define diagnostic_topics as parameter
  * fix include way
  * fix include way
  * delete ament_cmake_clang_format from package.xml
  * fix test_depend
  * Update evaluator/diagnostic_converter/test/test_converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * style(pre-commit): autofix
  * Update launch/tier4_simulator_launch/launch/simulator.launch.xml
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kyoichi Sugahara, Takayuki Murooka, Vincent Richard
