^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_shift_decider
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control)!: refactor directory structures of the control checkers (`#7524 <https://github.com/autowarefoundation/autoware.universe/issues/7524>`_)
  * aeb
  * control_validator
  * lane_departure_checker
  * shift_decider
  * fix
  ---------
* feat(shift_decider): use polling subscriber (`#7345 <https://github.com/autowarefoundation/autoware.universe/issues/7345>`_)
  RT1-6697 use polling subscriber
* refactor(shift_decider): prefix package and namespace with autoware\_ (`#7310 <https://github.com/autowarefoundation/autoware.universe/issues/7310>`_)
  * RT1-6684 add autoware prefix and namespace
  * RT1-6684 Revert svg
  This reverts commit 4e0569e4796ab432c734905fb7f2106779575e29.
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* Contributors: Kosuke Takeuchi, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zulfaqar Azmi

0.26.0 (2024-04-03)
-------------------
