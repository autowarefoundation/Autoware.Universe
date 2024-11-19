^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package reaction_analyzer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* docs(reaction_analyzer): update bag files and the README (`#8633 <https://github.com/autowarefoundation/autoware.universe/issues/8633>`_)
  * docs(reaction_analyzer): update bag files and the README
* fix(reaction_analyzer): fix include hierarchy of tf2_eigen (`#8663 <https://github.com/autowarefoundation/autoware.universe/issues/8663>`_)
  Fixed include hierarchy of tf2_eigen
* fix(reaction_analyzer): fix variableScope (`#8450 <https://github.com/autowarefoundation/autoware.universe/issues/8450>`_)
  * fix:variableScope
  * fix:clang format
  ---------
* fix(reaction_analyzer): fix constVariableReference (`#8063 <https://github.com/autowarefoundation/autoware.universe/issues/8063>`_)
  * fix:constVariableReference
  * fix:constVariableReference
  * fix:constVariableReference
  * fix:suppression constVariableReference
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for tools (`#7250 <https://github.com/autowarefoundation/autoware.universe/issues/7250>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat(reaction_analyzer): add reaction anaylzer tool to measure end-to-end delay in sudden obstacle braking response (`#5954 <https://github.com/autowarefoundation/autoware.universe/issues/5954>`_)
  * feat(reaction_analyzer): add reaction anaylzer tool to measure end-to-end delay in sudden obstacle braking response
  * feat: implement message_filters package, clean up
  * feat: update style and readme
  * feat: add predicted path for the PredictedObject and add publish_only_pointcloud_with_object
  * feat: add wrong initialize localization protection, improve code readability
  * feat: launch occupancy_grid_map from reaction analyzer's own launch file
  * feat: update
  * feat: change function names
  * feat: update
  * feat: improve style, change csv output stringstream
  * fix: ci/cd
  * feat: update for new sensor setup, fix bug, optimize code, show pipeline latency, update readme
  * fix: container die problem
  * feat: update stats, check path param, add marker, warn user for wrong reaction_chain
  ---------
* Contributors: Batuhan Beytekin, Berkay Karaman, Kosuke Takeuchi, Ryohsuke Mitsudome, SakodaShintaro, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
