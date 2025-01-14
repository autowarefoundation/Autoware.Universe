^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_camera_view_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(autoware_ad_api_specs): prefix package and namespace with autoware (`#9250 <https://github.com/autowarefoundation/autoware.universe/issues/9250>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api_specs to adapi_specs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* fix(tier4_camera_view_rviz_plugin): fix unmatchedSuppression (`#8918 <https://github.com/autowarefoundation/autoware.universe/issues/8918>`_)
  fix:unmatchedSuppression
* style: update rviz plugin icons to match the theme (`#8868 <https://github.com/autowarefoundation/autoware.universe/issues/8868>`_)
* fix(tier4_camera_view_rviz_plugin): fix unusedFunction (`#8843 <https://github.com/autowarefoundation/autoware.universe/issues/8843>`_)
  fix:unusedFunction
* fix(tier4_camera_view_rviz_plugin): fix uninitMemberVar (`#8819 <https://github.com/autowarefoundation/autoware.universe/issues/8819>`_)
  * fix:uninitMemberVar
  * fix:clang format
  ---------
* fix(tier4_camera_view_rviz_plugin): fix unusedFunction (`#8639 <https://github.com/autowarefoundation/autoware.universe/issues/8639>`_)
  * fix:unusedFunction
  * fix:clang format
  * fix:unusedFunction
  ---------
* fix: replace Ogre deprecated header (`#7606 <https://github.com/autowarefoundation/autoware.universe/issues/7606>`_)
  Fix Ogre deprecated header
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* fix(tier4_camera_view_rviz_plugin): fix funcArgNamesDifferent warnings (`#7621 <https://github.com/autowarefoundation/autoware.universe/issues/7621>`_)
* Contributors: Khalil Selyan, Ryuta Kambe, Yutaka Kondo, kobayu858, ぐるぐる

0.26.0 (2024-04-03)
-------------------
* fix(readme): add acknowledgement for material icons in tool plugins (`#6354 <https://github.com/autowarefoundation/autoware.universe/issues/6354>`_)
* style(update): autoware tools icons (`#6351 <https://github.com/autowarefoundation/autoware.universe/issues/6351>`_)
* feat(camera_view_plugin): add camera view plugin package (`#5472 <https://github.com/autowarefoundation/autoware.universe/issues/5472>`_)
  * add camera view plugin package
  * add readme for short cut
  * style(pre-commit): autofix
  * change name
  * style(pre-commit): autofix
  * fix license
  * fix license
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Owen-Liuyuxuan <uken.ryu@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Khalil Selyan, Yuxuan Liu
