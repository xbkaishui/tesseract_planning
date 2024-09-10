/**
 * @file ompl_motion_planner.h
 * @brief Tesseract OMPL motion planner.
 *
 * @author Levi Armstrong
 * @date April 18, 2018
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2017, Southwest Research Institute
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
#ifndef TESSERACT_MOTION_PLANNERS_OMPL_MOTION_PLANNER_H
#define TESSERACT_MOTION_PLANNERS_OMPL_MOTION_PLANNER_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <functional>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_motion_planners/core/planner.h>
#include <tesseract_motion_planners/ompl/profile/ompl_profile.h>

namespace ompl::tools
{
class ParallelPlan;
}  // namespace ompl::tools

namespace tesseract_planning
{
struct OMPLProblemConfig
{
  OMPLProblem::Ptr problem;
  boost::uuids::uuid start_uuid{};
  boost::uuids::uuid end_uuid{};
};

/**
 * @brief This planner is intended to provide an easy to use interface to OMPL for freespace planning. It is made to
 * take a start and end point and automate the generation of the OMPL problem.
 */
class OMPLMotionPlanner : public MotionPlanner
{
public:
  /** @brief Construct a planner */
  OMPLMotionPlanner(std::string name);

  /**
   * @brief Sets up the OMPL problem then solves. It is intended to simplify setting up
   * and solving freespace motion problems.
   *
   * This planner leverages OMPL ParallelPlan which supports being called multiple times. So you are able to
   * setConfiguration and keep calling solve and with each solve it will continue to build the existing planner graph.
   * If you want to start with a clean graph you must call setConfiguration() before calling solve again.
   *
   * This planner (and the associated config passed to the setConfiguration) does not expose all of the available
   * configuration data in OMPL. This is done to simplify the interface. However, many problems may require more
   * specific setups. In that case, the source code for this planner may be used as an example.
   *
   * Note: This does not use the request information because everything is provided by config parameter
   *
   * @param response The results of OMPL.
   * @param check_type The type of validation check to be performed on the planned trajectory
   * @param verbose Flag for printing more detailed planning information
   * @return true if valid solution was found
   */
  PlannerResponse solve(const PlannerRequest& request) const override;

  bool terminate() override;

  void clear() override;

  MotionPlanner::Ptr clone() const override;

  virtual std::vector<OMPLProblemConfig> createProblems(const PlannerRequest& request) const;

protected:
  /** @brief OMPL Parallel planner */
  std::shared_ptr<ompl::tools::ParallelPlan> parallel_plan_;

  OMPLProblemConfig createSubProblem(const PlannerRequest& request,
                                     const tesseract_common::ManipulatorInfo& composite_mi,
                                     const tesseract_kinematics::JointGroup::ConstPtr& manip,
                                     const MoveInstructionPoly& start_instruction,
                                     const MoveInstructionPoly& end_instruction,
                                     int n_output_states,
                                     int index) const;
};

}  // namespace tesseract_planning

#endif  // TESSERACT_MOTION_PLANNERS_OMPL_MOTION_PLANNER_H
