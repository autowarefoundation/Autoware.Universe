^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_collision_detector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(collision_detector): skip process when odometry is not published (`#9308 <https://github.com/autowarefoundation/autoware.universe/issues/9308>`_)
  * subscribe odometry
  * fix precommit
  * remove unnecessary log info
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(collision_detector): use polling subscriber (`#9213 <https://github.com/autowarefoundation/autoware.universe/issues/9213>`_)
  use polling subscriber
* Contributors: Esteve Fernandez, Go Sakayori, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* chore(collision_detector): add maintainer  (`#9184 <https://github.com/autowarefoundation/autoware.universe/issues/9184>`_)
  add maintainer
* feat(collision_detector): add autoware_collision_detector (`#9157 <https://github.com/autowarefoundation/autoware.universe/issues/9157>`_)
  * add new package autoware_collision_detector
  * update to latest
  * fix
  * fix reference
  * modify maintainer
  * change definiton from filtering to exclude
  * change description for parameters
  ---------
* Contributors: Go Sakayori, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
