^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_ar_tag_based_localizer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* fix(cpplint): include what you use - localization (`#9567 <https://github.com/autowarefoundation/autoware.universe/issues/9567>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(localization_util)!: prefix package and namespace with autoware (`#8922 <https://github.com/autowarefoundation/autoware.universe/issues/8922>`_)
  add autoware prefix to localization_util
* fix(doc): landmark localizer (`#8301 <https://github.com/autowarefoundation/autoware.universe/issues/8301>`_)
  fix landmark localizer
* refactor(localization): remove unnecessary dependency in localization packages (`#8202 <https://github.com/autowarefoundation/autoware.universe/issues/8202>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* perf(autoware_ar_tag_based_localizer): remove unnecessary pub/sub depth for transient_local (`#8259 <https://github.com/autowarefoundation/autoware.universe/issues/8259>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* chore: update sample rosbags for localization modules (`#7716 <https://github.com/autowarefoundation/autoware.universe/issues/7716>`_)
  Update sample rosbags for new message types
* fix: replace deprecated header in Jazzy (`#7603 <https://github.com/autowarefoundation/autoware.universe/issues/7603>`_)
  * Use cv_bridge.hpp if available
  * Fix image_geometry deprecated header
  * Add comment for __has_include
  ---------
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(ar_tag_based_localizer): add prefix "autoware\_" to ar_tag_based_localizer (`#7483 <https://github.com/autowarefoundation/autoware.universe/issues/7483>`_)
  * Added prefix "autoware\_" to ar_tag_based_localizer
  * style(pre-commit): autofix
  * Fixed localization_launch
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Kento Yabuuchi, Kosuke Takeuchi, Masaki Baba, SakodaShintaro, TaikiYamada4, Takayuki Murooka, Yutaka Kondo, Yuxuan Liu, ぐるぐる

0.26.0 (2024-04-03)
-------------------
