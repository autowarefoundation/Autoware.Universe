// Copyright 2024 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "decision_result.hpp"

#include <string>

namespace autoware::behavior_velocity_planner
{
std::string formatDecisionResult(
  const DecisionResult & decision_result, const bool int_activated, const bool int_occ_activated)
{
  const auto rtc = "RTC: intersection activated_ = " + std::to_string(int_activated) +
                   ", intersection_occlusion activated_ = " + std::to_string(int_occ_activated) +
                   "\n";

  if (std::holds_alternative<InternalError>(decision_result)) {
    const auto & state = std::get<InternalError>(decision_result);
    return rtc + "InternalError because " + state.error;
  }
  if (std::holds_alternative<OverPassJudge>(decision_result)) {
    const auto & state = std::get<OverPassJudge>(decision_result);
    return rtc + "OverPassJudge:\nsafety_report:" + state.safety_report +
           "\nevasive_report:" + state.evasive_report;
  }
  if (std::holds_alternative<StuckStop>(decision_result)) {
    return rtc + "StuckStop";
  }
  if (std::holds_alternative<YieldStuckStop>(decision_result)) {
    const auto & state = std::get<YieldStuckStop>(decision_result);
    return rtc + "YieldStuckStop:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<NonOccludedCollisionStop>(decision_result)) {
    const auto & state = std::get<NonOccludedCollisionStop>(decision_result);
    return rtc + "NonOccludedCollisionStop:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<FirstWaitBeforeOcclusion>(decision_result)) {
    const auto & state = std::get<FirstWaitBeforeOcclusion>(decision_result);
    return rtc + "FirstWaitBeforeOcclusion:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<PeekingTowardOcclusion>(decision_result)) {
    const auto & state = std::get<PeekingTowardOcclusion>(decision_result);
    return rtc + "PeekingTowardOcclusion:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<OccludedCollisionStop>(decision_result)) {
    const auto & state = std::get<OccludedCollisionStop>(decision_result);
    return rtc + "OccludedCollisionStop:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<OccludedAbsenceTrafficLight>(decision_result)) {
    const auto & state = std::get<OccludedAbsenceTrafficLight>(decision_result);
    return rtc + "OccludedAbsenceTrafficLight:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<Safe>(decision_result)) {
    const auto & state = std::get<Safe>(decision_result);
    return rtc + "Safe:\nocclusion_report:" + state.occlusion_report;
  }
  if (std::holds_alternative<FullyPrioritized>(decision_result)) {
    const auto & state = std::get<FullyPrioritized>(decision_result);
    return rtc + "FullyPrioritized\nsafety_report:" + state.safety_report;
  }
  return rtc + "";
}

}  // namespace autoware::behavior_velocity_planner
