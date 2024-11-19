^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_rtc_interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(rtc_interface): update cooperateStatus state transition (`#8883 <https://github.com/autowarefoundation/autoware.universe/issues/8883>`_)
  fix state transition for failure/success
* feat(rtc_interface, lane_change): check state transition for cooperate status (`#8855 <https://github.com/autowarefoundation/autoware.universe/issues/8855>`_)
  * update rtc state transition
  * remove transition from failuer and succeeded
  * fix
  * check initial state for cooperate status
  * change rtc cooperate status according to module status
  ---------
* feat(rtc_inteface): add function to check force deactivate (`#8221 <https://github.com/autowarefoundation/autoware.universe/issues/8221>`_)
  add function to check for deactivation
* fix(autoware_rtc_interface): fix constParameterReference (`#8042 <https://github.com/autowarefoundation/autoware.universe/issues/8042>`_)
  * fix:constParameterReference
  * fix: clang format
  * fix:constParameterReference
  ---------
* fix(rtc_interface): fix build error (`#8111 <https://github.com/autowarefoundation/autoware.universe/issues/8111>`_)
  * fix
  * fix format
  ---------
* fix(bpp, rtc_interface): fix state transition (`#7743 <https://github.com/autowarefoundation/autoware.universe/issues/7743>`_)
  * fix(rtc_interface): check rtc state
  * fix(bpp_interface): check rtc state
  * feat(rtc_interface): print
  ---------
* feat(static_obstacle_avoidance): enable force execution under unsafe conditions (`#8094 <https://github.com/autowarefoundation/autoware.universe/issues/8094>`_)
  * add force execution for static obstacle avoidance
  * fix
  * erase unused function in RTC interface
  * refactor with lamda function
  * fix rtc_interface
  * add warn throtthle and move code block
  * fix
  ---------
* docs(planning): fix wrong link (`#7751 <https://github.com/autowarefoundation/autoware.universe/issues/7751>`_)
  * fix page link
  * fix out of lane link
  * fix
  * fix cost map generator link
  ---------
* docs(rtc_replayer): fix wrong link (`#7714 <https://github.com/autowarefoundation/autoware.universe/issues/7714>`_)
  * fix link for rtc_replayer
  * delete RTC replayer header
  * fix
  ---------
* refactor(rtc_interface)!: rename to include/autoware/{package_name} (`#7531 <https://github.com/autowarefoundation/autoware.universe/issues/7531>`_)
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
* refactor(rtc_interface)!: prefix package and namespace with autoware (`#7321 <https://github.com/autowarefoundation/autoware.universe/issues/7321>`_)
  refactor(rtc_interface): add autoware prefix
* Contributors: Fumiya Watanabe, Go Sakayori, Kosuke Takeuchi, Satoshi OTA, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
