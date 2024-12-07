^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_pose_initializer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(component_interface_specs): prefix package and namespace with autoware (`#9094 <https://github.com/autowarefoundation/autoware.universe/issues/9094>`_)
* feat(pose_initializer): check error initial pose and gnss pose, output diagnostics (`#8947 <https://github.com/autowarefoundation/autoware.universe/issues/8947>`_)
  * check initial pose error use GNSS pose
  * add pose_error_check_enabled parameter
  * fixed key value name
  * rename diagnostics key value
  * update README
  * fixed schema json
  * fixed type and default in schema json
  * rename key value
  ---------
* refactor(localization_util)!: prefix package and namespace with autoware (`#8922 <https://github.com/autowarefoundation/autoware.universe/issues/8922>`_)
  add autoware prefix to localization_util
* refactor(pose_initializer)!: prefix package and namespace with autoware (`#8701 <https://github.com/autowarefoundation/autoware.universe/issues/8701>`_)
  * add autoware\_ prefix
  * fix link
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* Contributors: Esteve Fernandez, Masaki Baba, RyuYamamoto, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
