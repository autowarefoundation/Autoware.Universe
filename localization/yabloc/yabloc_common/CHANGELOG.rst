^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package yabloc_common
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* fix(yabloc_common): fix unusedFunction (`#8560 <https://github.com/autowarefoundation/autoware.universe/issues/8560>`_)
  fix:unusedFunction
* perf(yabloc_common): remove unnecessary pub/sub depth for transient_local (`#8248 <https://github.com/autowarefoundation/autoware.universe/issues/8248>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* fix: replace deprecated header in Jazzy (`#7603 <https://github.com/autowarefoundation/autoware.universe/issues/7603>`_)
  * Use cv_bridge.hpp if available
  * Fix image_geometry deprecated header
  * Add comment for __has_include
  ---------
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(yabloc_common): apply static analysis (`#7481 <https://github.com/autowarefoundation/autoware.universe/issues/7481>`_)
  * refactor based on linter
  * restore unwanted change
  * remove unnecessary comment
  * style(pre-commit): autofix
  * change to const double
  * add static cast
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(yabloc): suppress no viable conversion error (`#7299 <https://github.com/autowarefoundation/autoware.universe/issues/7299>`_)
  * use tier4_autoware_utils instead of yabloc::Color
  * use static_cast to convert Color to RGBA
  * use tier4_autoware_utils instead of yabloc::Color
  * use static_cast to convert Color to RGBA
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for localization modules (`#7243 <https://github.com/autowarefoundation/autoware.universe/issues/7243>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* refactor(yabloc): use constexpr properly (`#7207 <https://github.com/autowarefoundation/autoware.universe/issues/7207>`_)
* fix(yabloc): fix bug in capturing in lambda function (`#7208 <https://github.com/autowarefoundation/autoware.universe/issues/7208>`_)
  * fix(yabloc): fix bug in capturing in lambda function
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(yabloc_common): componentize yabloc_common nodes (`#7143 <https://github.com/autowarefoundation/autoware.universe/issues/7143>`_)
  * make executables component
  * log output changes to both
  * style(pre-commit): autofix
  * add default param path
  * add glog as depend package
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(glog): add initialization check (`#6792 <https://github.com/autowarefoundation/autoware.universe/issues/6792>`_)
* Contributors: Esteve Fernandez, Kento Yabuuchi, Kosuke Takeuchi, Masaki Baba, Ryohsuke Mitsudome, Ryuta Kambe, Takamasa Horibe, Takayuki Murooka, Yutaka Kondo, kobayu858, ぐるぐる

0.26.0 (2024-04-03)
-------------------
* chore(yabloc): replace parameters by json_to_markdown in readme (`#6183 <https://github.com/autowarefoundation/autoware.universe/issues/6183>`_)
  * replace parameters by json_to_markdown
  * fix some schma path
  * fix again
  ---------
* chore(yabloc): rework parameters (`#6170 <https://github.com/autowarefoundation/autoware.universe/issues/6170>`_)
  * introduce json schema for ground_server
  * introduce json schema for ll2_decomposer
  * style(pre-commit): autofix
  * fix json in yabloc_common
  * introduce json schema for graph_segment
  * introduce json schema for segment_filter
  * fix yabloc_common schema.json
  * introduce json schema for undistort
  * style(pre-commit): autofix
  * Revert "introduce json schema for ground_server"
  This reverts commit 33d3e609d4e01919d11a86d3c955f53e529ae121.
  * Revert "introduce json schema for graph_segment"
  This reverts commit 00ae417f030324f2dcc7dfb4b867a969ae31aea7.
  * style(pre-commit): autofix
  * introduce json schema for yabloc_monitor
  * introduce json schema for yabloc_particle_filter
  * introduce json schema for yabloc_pose_initializer
  * apply pre-commit
  * fix revert conflict
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add localization & mapping maintainers (`#6085 <https://github.com/autowarefoundation/autoware.universe/issues/6085>`_)
  * Added lm maintainers
  * Add more
  * Fixed maintainer
  ---------
* build(yabloc_common): add missing libgoogle-glog-dev dependency (`#5225 <https://github.com/autowarefoundation/autoware.universe/issues/5225>`_)
  * build(yabloc_common): add missing libgoogle-glog-dev dependency
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(localization_packages): remove unused <depend> in packages.xml files (`#5171 <https://github.com/autowarefoundation/autoware.universe/issues/5171>`_)
  Co-authored-by: yamato-ando <Yamato ANDO>
* chore: add maintainer in localization and map packages (`#4501 <https://github.com/autowarefoundation/autoware.universe/issues/4501>`_)
* fix(yabloc): fix typo (`#4281 <https://github.com/autowarefoundation/autoware.universe/issues/4281>`_)
  * fix(yabloc): fix typo
  * fix Kinv and mean_pose
  * style(pre-commit): autofix
  * fix normalized term
  * fix resamping
  * style(pre-commit): autofix
  * fix reweight
  * fix typo
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(yabloc): add camera and vector map localization (`#3946 <https://github.com/autowarefoundation/autoware.universe/issues/3946>`_)
  * adopt scane_case to undistort, segment_filter
  * adopt scane_case to ground_server, ll2_decomposer
  * adopt scane_case to twist_converter, twist_estimator
  * adopt scane_case to validation packages
  * adopt scane_case tomodularized_particle_filter
  * adopt scane_case to gnss_particle_corrector
  * adopt scane_case to camera_particle_corrector
  * adopt scane_case to antishadow_corrector
  * adopt scane_case to particle_initializer
  * organize launch files
  * add twist_visualizer to validate odometry performance
  * use SE3::exp() to predict particles & modify linear noise model
  * stop to use LL2 to rectify initialpose2d
  * fix redundant computation in segment_accumulator
  * improve gnss_particle_corrector
  * fix segment_accumulator's bug
  * add doppler_converter
  * add xx2.launch.xml
  * add hsv_extractor
  * pickup other regions which have same color histogram
  * use additional region to filt line-segments
  * improve graph-segmentation
  * remove `truncate_pixel_threshold`
  * refactor graph_segmentator & segment_filter
  * add mahalanobis_distance_threshold in GNSS particle corrector
  * add extract_line_segments.hpp
  * use pcl::transformCloudWithNormals instead of  transform_cloud
  * filt accumulating segments by LL2
  * move herarchical_cost_map to common
  * apply positive feedback for accumulation
  * move transform_linesegments() to common pkg
  * refactor
  * use all projected lines for camera corrector
  * evaluate iffy linesegments
  * complete to unify ll2-assisted lsd clasification
  * add abs_cos2() which is more strict direction constraint
  * fix orientation initialization bug
  * publish doppler direction
  * TMP: add disable/enable switch for camera corrector
  * implement doppler orientation correction but it's disabled by default
  * speed up camera corrector
  * update ros params
  * implement kalman filter for ground tilt estimation
  * continuous height estimation works well?
  * estimate height cotiniously
  * use only linesegments which are at same height
  * add static_gyro_bias parameter
  * fix bug about overlay varidation
  * increse ll2 height marging in cost map generation
  * add static_gyro_bias in twist.launch.xml
  * load pcdless_init_area from ll2
  * add specified initialization area
  * add corrector_manager node to disable/enable camera_corrector
  * call service to disable camer_corrector from manager
  * load corrector disable area
  * overlay even if pose is not estiamted
  * publish camera corrector's status as string
  * add set_booL_panel for camera_corrector enable/disable
  * load bounding box from lanelet2
  * draw bounding box on cost map
  * remove at2,at1 from cost map
  * use cost_map::at() instread pf at2()
  * move cost map library from common to camera corrector
  * use logit for particle weighting but it does not work well
  * prob_to_logit() requires non-intuitive parameters
  * goodbye stupid parameters (max_raw_score & score_offset)
  * publish two scored pointclouds as debug
  * can handle unmapped areas
  * remove obsolete packages
  * update README.md
  * Update README.md
  * add image of how_to_launch
  * add node diagram in readme
  * add rviz_description.png in README
  * subscribe pose_with_cov & disconnect base_link <-> particle_pose
  * remove segment_accumulator & launch ekf_localizer from this project
  * add gnss_ekf_corrector
  * add camera_ekf_corrector package
  * subscribe ekf prediction & synch pose data
  * WIP: ready to implement UKF?
  * estimate weighted averaging as pose_estimator
  * basic algorithm is implemented but it does not work proparly
  * apply after_cov_gain\_
  * ekf corrector works a little bit appropriately
  * increase twist covariance for ekf
  * test probability theory
  * updat prob.py
  * implement de-bayesing but it loooks ugly
  * remove obsolete parameters
  * skip measurement publishing if travel distance is so short
  * use constant covariance because i dont understand what is correct
  * add submodule sample_vehicle_launch
  * TMP but it works
  * add ekf_trigger in particle_initializer.hpp
  * publish gnss markers & camera_est pubishes constant cov
  * back to pcd-less only launcher
  * add bayes_util package
  * apply de-bayesing for camera_ekf
  * some launch file update
  * organize launch files. we can choice mode from ekf/pekf/pf
  * organize particle_initializer
  * add swap_mode_adaptor WIP
  * use latest ekf in autoware & sample_vehicle
  * fix bug of swap_adalptor
  * fix FIX & FLOAT converter
  * fix septentrio doppler converter
  * move ekf packages to ekf directory
  * ignore corrector_manager
  * add standalone arg in launch files
  * update semseg_node
  * add camera_pose_initializer pkg
  * subscribe camera_info&tf and prepare semantic projection
  * project semantic image
  * create vector map image from ll2
  * create lane image from vector map
  * search the most match angle by non-zero pixels
  * camera based pose_initializer
  * move ekf packages into unstable
  * move ekf theory debugger
  * add tier4_autoware_msgs as submodule
  * move pose_initializer into initializer dir
  * add semse_msgs pkg
  * separate marker pub function
  * separate projection functions
  * add semseg_srv client
  * move sem-seg directory
  * camera pose initilizer works successfully
  * rectify covariance along the orientation
  * improve initialization parameters
  * take into account covariance of request
  * use lanelet direciton to compute init pose scores
  * semseg download model automatically
  * remove sample_vehicle_launch
  * add autoware_msgs
  * remove obsolete launch files
  * add standalone mode for direct initialization
  * fix fix_to_pose
  * update launch files
  * update rviz config
  * remove lidar_particle_corrector
  * remove Sophus from sunbmodule
  * rename submodule directory
  * update README and some sample images
  * update README.md
  * fix override_camera_frame_id bahaviors
  * fix some bugs (`#4 <https://github.com/autowarefoundation/autoware.universe/issues/4>`_)
  * fix: use initialpose from Rviz (`#6 <https://github.com/autowarefoundation/autoware.universe/issues/6>`_)
  * use initialpose from Rviz to init
  * add description about how-to-set-initialpose
  ---------
  * misc: add license (`#7 <https://github.com/autowarefoundation/autoware.universe/issues/7>`_)
  * WIP: add license description
  * add license description
  * add description about license in README
  ---------
  * add quick start demo (`#8 <https://github.com/autowarefoundation/autoware.universe/issues/8>`_)
  * refactor(launch) remove & update obsolete launch files (`#9 <https://github.com/autowarefoundation/autoware.universe/issues/9>`_)
  * delete obsolete launch files
  * update documents
  ---------
  * docs(readme): update architecture image (`#10 <https://github.com/autowarefoundation/autoware.universe/issues/10>`_)
  * replace architecture image in README
  * update some images
  ---------
  * refactor(pcdless_launc/scripts): remove unnecessary scripts (`#11 <https://github.com/autowarefoundation/autoware.universe/issues/11>`_)
  * remove not useful scripts
  * rename scripts &  add descriptions
  * little change
  * remove odaiba.rviz
  * grammer fix
  ---------
  * fix(pcdless_launch): fix a build bug
  * fix(twist_estimator): use velocity_report by default
  * fix bug
  * debugged, now works
  * update sample rosbag link (`#14 <https://github.com/autowarefoundation/autoware.universe/issues/14>`_)
  * feature(graph_segment, gnss_particle_corrector): make some features switchable (`#17 <https://github.com/autowarefoundation/autoware.universe/issues/17>`_)
  * make additional-graph-segment-pickup disablable
  * enlarge gnss_mahalanobis_distance_threshold in expressway.launch
  ---------
  * fix: minor fix for multi camera support (`#18 <https://github.com/autowarefoundation/autoware.universe/issues/18>`_)
  * fix: minor fix for multi camera support
  * update
  * update
  * fix typo
  ---------
  * refactor(retroactive_resampler): more readable (`#19 <https://github.com/autowarefoundation/autoware.universe/issues/19>`_)
  * make Hisotry class
  * use boost:adaptors::indexed()
  * add many comment in resampling()
  * does not use ConstSharedPtr
  * rename interface of resampler
  * circular_buffer is unnecessary
  ---------
  * refactor(mpf::predictor) resampling interval control in out of resampler (`#20 <https://github.com/autowarefoundation/autoware.universe/issues/20>`_)
  * resampling interval management should be done out of resample()
  * resampler class throw exeption rather than optional
  * split files for resampling_history
  * split files for experimental/suspention_adaptor
  ---------
  * refactor(mpf::predictor): just refactoring (`#21 <https://github.com/autowarefoundation/autoware.universe/issues/21>`_)
  * remove obsolete functions
  * remove test of predictor
  * remove remapping in pf.launch.xml for suspension_adapator
  * add some comments
  ---------
  * fix(twist_estimator): remove stop filter for velocity (`#23 <https://github.com/autowarefoundation/autoware.universe/issues/23>`_)
  * feat(pcdless_launch): add multi camera launcher (`#22 <https://github.com/autowarefoundation/autoware.universe/issues/22>`_)
  * feat(pcdless_launch): add multi camera launcher
  * minor fix
  ---------
  * refactor(CMakeListx.txt): just refactoring (`#24 <https://github.com/autowarefoundation/autoware.universe/issues/24>`_)
  * refactor imgproc/*/CMakeListx.txt
  * refactor initializer/*/CMakeListx.txt & add gnss_pose_initializer pkg
  * rename some files in twist/ & refactor pf/*/cmakelist
  * refactor validation/*/CMakeListx.txt
  * fix some obsolete executor name
  ---------
  * fix: rename lsd variables and files (`#26 <https://github.com/autowarefoundation/autoware.universe/issues/26>`_)
  * misc: reame pcdless to yabloc (`#25 <https://github.com/autowarefoundation/autoware.universe/issues/25>`_)
  * rename pcdless to yabloc
  * fix conflict miss
  ---------
  * visualize path (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * docs: update readme about particle filter (`#30 <https://github.com/autowarefoundation/autoware.universe/issues/30>`_)
  * update mpf/README.md
  * update gnss_corrector/README.md
  * update camera_corrector/README.md
  ---------
  * feat(segment_filter): publish images with lines and refactor (`#29 <https://github.com/autowarefoundation/autoware.universe/issues/29>`_)
  * feat(segment_filter): publish images with lines
  * update validation
  * update imgproc (reverted)
  * large change inclding refactoring
  * major update
  * revert rviz config
  * minor fix in name
  * add validation option
  * update architecture svg
  * rename validation.launch to overlay.launch
  * no throw runtime_error (unintentionaly applying format)
  ---------
  Co-authored-by: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  * catch runtime_error when particle id is invalid (`#31 <https://github.com/autowarefoundation/autoware.universe/issues/31>`_)
  * return if info is nullopt (`#32 <https://github.com/autowarefoundation/autoware.universe/issues/32>`_)
  * pose_buffer is sometimes empty (`#33 <https://github.com/autowarefoundation/autoware.universe/issues/33>`_)
  * use_yaw_of_initialpose (`#34 <https://github.com/autowarefoundation/autoware.universe/issues/34>`_)
  * feat(interface):  remove incompatible interface (`#35 <https://github.com/autowarefoundation/autoware.universe/issues/35>`_)
  * not use ublox_msg when run as autoware
  * remove twist/kalman/twist & use twist_estimator/twist_with_covariance
  * update particle_array stamp even if the time stamp seems wrong
  ---------
  * fix: suppress info/warn_stream (`#37 <https://github.com/autowarefoundation/autoware.universe/issues/37>`_)
  * does not stream undistortion time
  * improve warn stream when skip particle weighting
  * surpress frequency of  warnings during synchronized particle searching
  * fix camera_pose_initializer
  ---------
  * /switch must not be nice name (`#39 <https://github.com/autowarefoundation/autoware.universe/issues/39>`_)
  * misc(readme): update readme (`#41 <https://github.com/autowarefoundation/autoware.universe/issues/41>`_)
  * add youtube link and change thumbnail
  * improve input/output topics
  * quick start demo screen image
  * add abstruct architecture and detail architecture
  ---------
  * docs(rosdep): fix package.xml to ensure build success (`#44 <https://github.com/autowarefoundation/autoware.universe/issues/44>`_)
  * fix package.xml to success build
  * add 'rosdep install' in how-to-build
  ---------
  * add geographiclib in package.xml (`#46 <https://github.com/autowarefoundation/autoware.universe/issues/46>`_)
  * fix path search error in build stage (`#45 <https://github.com/autowarefoundation/autoware.universe/issues/45>`_)
  * fix path search error in build stage
  * fix https://github.com/tier4/YabLoc/pull/45#issuecomment-1546808419
  * Feature/remove submodule (`#47 <https://github.com/autowarefoundation/autoware.universe/issues/47>`_)
  * remove submodules
  * remove doppler converter
  ---------
  * feature: change node namespace to /localization/yabloc/** from /localization/** (`#48 <https://github.com/autowarefoundation/autoware.universe/issues/48>`_)
  * change node namespace
  * update namespace for autoware-mode
  * update namespace in multi_camera.launch
  ---------
  * removed unstable packages (`#49 <https://github.com/autowarefoundation/autoware.universe/issues/49>`_)
  * feature: add *.param.yaml to manage parameters (`#50 <https://github.com/autowarefoundation/autoware.universe/issues/50>`_)
  * make *.param.yaml in imgproc packages
  * make *.param.yaml in initializer packages
  * make *.param.yaml in map packages
  * make *.param.yaml in pf packages
  * make *.param.yaml in twist packages
  * fix expressway parameter
  * fix override_frame_id
  * remove default parameters
  * fix some remaining invalida parameters
  ---------
  * does not estimate twist (`#51 <https://github.com/autowarefoundation/autoware.universe/issues/51>`_)
  * feat(particle_initializer): merge particle_initializer into mpf (`#52 <https://github.com/autowarefoundation/autoware.universe/issues/52>`_)
  * feat(particle_initializer): merge particle_initializer to modulalized_particle_filter
  * remove particle_initializer
  * remove debug message
  * remove related parts
  * update readme
  * rename publishing topic
  ---------
  Co-authored-by: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  * fix: remove ll2_transition_area (`#54 <https://github.com/autowarefoundation/autoware.universe/issues/54>`_)
  * feature(initializer): combine some initializer packages (`#56 <https://github.com/autowarefoundation/autoware.universe/issues/56>`_)
  * combine some package about initializer
  * yabloc_pose_initializer works well
  * remove old initializer packages
  * semseg node can launch
  * fix bug
  * revert initializer mode
  ---------
  * feature(imgproc): reudce imgproc packages (`#57 <https://github.com/autowarefoundation/autoware.universe/issues/57>`_)
  * combine some imgproc packages
  * combine overlay monitors into imgproc
  ---------
  * feature(validation): remove validation packages (`#58 <https://github.com/autowarefoundation/autoware.universe/issues/58>`_)
  * remove validation packages
  * remove path visualization
  ---------
  * feature(pf): combine some packages related to particle filter (`#59 <https://github.com/autowarefoundation/autoware.universe/issues/59>`_)
  * create yabloc_particle_filter
  * combine gnss_particle_corrector
  * combine ll2_cost_map
  * combine camera_particle_corrector
  * fix launch files
  * split README & remove obsolete scripts
  * fix config path of multi_camera mode
  ---------
  * feature: combine map and twist packages (`#60 <https://github.com/autowarefoundation/autoware.universe/issues/60>`_)
  * removed some twist nodes & rename remains to yabloc_twist
  * fix launch files for yabloc_twist
  * move map packages to yabloc_common
  * WIP: I think its impossible
  * Revert "WIP: I think its impossible"
  This reverts commit 49da507bbf9abe8fcebed4d4df44ea5f4075f6d1.
  * remove map packages & fix some launch files
  ---------
  * removed obsolete packages
  * remove obsolete dot files
  * use tier4_loc_launch instead of yabloc_loc_launch
  * move launch files to each packages
  * remove yabloc_localization_launch
  * remove yabloc_launch
  * modify yabloc/README.md
  * update yabloc_common/README.md
  * update yabloc_imgproc README
  * update yabloc_particle_filter/README
  * update yabloc_pose_initializer/README
  * update README
  * use native from_bin_msg
  * use ifndef instead of pragma once in yabloc_common
  * use ifndef instead of pragma once in yabloc_imgproc & yabloc_pf
  * use ifndef instead of pragma once in yabloc_pose_initializer
  * style(pre-commit): autofix
  * use autoware_cmake & suppress build warning
  * repalce yabloc::Timer with  tier4_autoware_utils::StopWatch
  * replace 1.414 with std::sqrt(2)
  * style(pre-commit): autofix
  * removed redundant ament_cmake_auto
  * removed yabloc_common/timer.hpp
  * replaced low_pass_filter with autoware's lowpass_filter_1d
  * style(pre-commit): autofix
  * Squashed commit of the following:
  commit cb08e290cca5c00315a58a973ec068e559c9e0a9
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:30:09 2023 +0900
  removed ublox_msgs in gnss_particle_corrector
  commit c158133f184a43914ec5f929645a7869ef8d03be
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:24:19 2023 +0900
  removed obsolete yabloc_multi_camera.launch
  commit 10f578945dc257ece936ede097544bf008e5f48d
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:22:14 2023 +0900
  removed ublox_msgs in yabloc_pose_initializer
  * style(pre-commit): autofix
  * removed fix2mgrs & ublox_stamp
  * added ~/ at the top of topic name
  * removed use_sim_time in yabloc launch files
  * add architecture diagram in README
  * rename lsd_node to line_segment_detector
  * style(pre-commit): autofix
  * Update localization/yabloc/README.md
  fix typo
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * removed obsolete debug code in similar_area_searcher
  * removed suspension_adaptor which manages lifecycle of particle predictor
  * style(pre-commit): autofix
  * renamed semseg to SemanticSegmentation
  * style(pre-commit): autofix
  * fixed README.md to solve markdownlint
  * WIP: reflected cpplint's suggestion
  * reflected cpplint's suggestion
  * rename AbstParaticleFilter in config files
  * fixed typo
  * used autoware_lint_common
  * fixed miss git add
  * style(pre-commit): autofix
  * replaced lanelet_util by lanelet2_extension
  * replaced fast_math by tie4_autoware_utils
  * sort package.xml
  * renamed yabloc_imgproc with yabloc_image_processing
  * reflected some review comments
  * resolved some TODO
  * prioritize NDT if both NDT and YabLoc initializer enabled
  * changed localization_mode option names
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: kminoda <koji.minoda@tier4.jp>
  Co-authored-by: Akihiro Komori <akihiro.komori@unity3d.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kento Yabuuchi, SakodaShintaro, Yamato Ando, kminoda
