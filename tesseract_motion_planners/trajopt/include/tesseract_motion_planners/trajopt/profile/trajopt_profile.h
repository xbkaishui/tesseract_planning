/**
 * @file trajopt_profile.h
 * @brief
 *
 * @author Levi Armstrong
 * @date June 18, 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_MOTION_PLANNERS_TRAJOPT_PROFILE_H
#define TESSERACT_MOTION_PLANNERS_TRAJOPT_PROFILE_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <trajopt/problem_description.hpp>
#include <vector>
#include <memory>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/poly/move_instruction_poly.h>
#include <tesseract_command_language/poly/cartesian_waypoint_poly.h>
#include <tesseract_command_language/poly/joint_waypoint_poly.h>

namespace tesseract_planning
{
class TrajOptPlanProfile
{
public:
  using Ptr = std::shared_ptr<TrajOptPlanProfile>;
  using ConstPtr = std::shared_ptr<const TrajOptPlanProfile>;

  TrajOptPlanProfile() = default;
  virtual ~TrajOptPlanProfile() = default;
  TrajOptPlanProfile(const TrajOptPlanProfile&) = default;
  TrajOptPlanProfile& operator=(const TrajOptPlanProfile&) = default;
  TrajOptPlanProfile(TrajOptPlanProfile&&) = default;
  TrajOptPlanProfile& operator=(TrajOptPlanProfile&&) = default;

  virtual void apply(trajopt::ProblemConstructionInfo& pci,
                     const CartesianWaypointPoly& cartesian_waypoint,
                     const MoveInstructionPoly& parent_instruction,
                     const tesseract_common::ManipulatorInfo& manip_info,
                     const std::vector<std::string>& active_links,
                     int index) const = 0;

  virtual void apply(trajopt::ProblemConstructionInfo& pci,
                     const JointWaypointPoly& joint_waypoint,
                     const MoveInstructionPoly& parent_instruction,
                     const tesseract_common::ManipulatorInfo& manip_info,
                     const std::vector<std::string>& active_links,
                     int index) const = 0;

  virtual tinyxml2::XMLElement* toXML(tinyxml2::XMLDocument& doc) const = 0;
};

class TrajOptCompositeProfile
{
public:
  using Ptr = std::shared_ptr<TrajOptCompositeProfile>;
  using ConstPtr = std::shared_ptr<const TrajOptCompositeProfile>;

  TrajOptCompositeProfile() = default;
  virtual ~TrajOptCompositeProfile() = default;
  TrajOptCompositeProfile(const TrajOptCompositeProfile&) = default;
  TrajOptCompositeProfile& operator=(const TrajOptCompositeProfile&) = default;
  TrajOptCompositeProfile(TrajOptCompositeProfile&&) = default;
  TrajOptCompositeProfile& operator=(TrajOptCompositeProfile&&) = default;

  virtual void apply(trajopt::ProblemConstructionInfo& pci,
                     int start_index,
                     int end_index,
                     const tesseract_common::ManipulatorInfo& manip_info,
                     const std::vector<std::string>& active_links,
                     const std::vector<int>& fixed_indices) const = 0;

  virtual tinyxml2::XMLElement* toXML(tinyxml2::XMLDocument& doc) const = 0;
};

class TrajOptSolverProfile
{
public:
  using Ptr = std::shared_ptr<TrajOptSolverProfile>;
  using ConstPtr = std::shared_ptr<const TrajOptSolverProfile>;

  TrajOptSolverProfile() = default;
  virtual ~TrajOptSolverProfile() = default;
  TrajOptSolverProfile(const TrajOptSolverProfile&) = default;
  TrajOptSolverProfile& operator=(const TrajOptSolverProfile&) = default;
  TrajOptSolverProfile(TrajOptSolverProfile&&) = default;
  TrajOptSolverProfile& operator=(TrajOptSolverProfile&&) = default;

  virtual void apply(trajopt::ProblemConstructionInfo& pci) const = 0;

  virtual tinyxml2::XMLElement* toXML(tinyxml2::XMLDocument& doc) const = 0;
};

using TrajOptSolverProfileMap = std::unordered_map<std::string, TrajOptSolverProfile::ConstPtr>;
using TrajOptCompositeProfileMap = std::unordered_map<std::string, TrajOptCompositeProfile::ConstPtr>;
using TrajOptPlanProfileMap = std::unordered_map<std::string, TrajOptPlanProfile::ConstPtr>;
}  // namespace tesseract_planning

#endif  // TESSERACT_MOTION_PLANNERS_TRAJOPT_PROFILE_H
