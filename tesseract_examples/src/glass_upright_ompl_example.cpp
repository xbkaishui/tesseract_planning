/**
 * @file glass_upright_ompl_example.cpp
 * @brief Glass upright OMPL example implementation
 *
 * @author Levi Armstrong
 * @date March 16, 2020
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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <console_bridge/console.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_examples/glass_upright_ompl_example.h>
#include <tesseract_environment/core/utils.h>
#include <trajopt/plot_callback.hpp>
#include <trajopt/file_write_callback.hpp>
#include <trajopt/problem_description.hpp>
#include <trajopt_common/config.hpp>
#include <trajopt_common/logging.hpp>
#include <tesseract_motion_planners/ompl/config/ompl_planner_freespace_config.h>
#include <tesseract_motion_planners/ompl/ompl_motion_planner.h>
#include <tesseract_motion_planners/trajopt/config/utils.h>
#include <tesseract_motion_planners/ompl/config/ompl_planner_constrained_config.h>
#include <tesseract_visualization/markers/toolpath_marker.h>
#include <tesseract_task_composer/planning_task_composer_problem.h>
#include <tesseract_task_composer/task_composer_input.h>

#include <tesseract_task_composer/nodes/freespace_motion_pipeline_task.h>
#include <tesseract_task_composer/taskflow/taskflow_task_composer_executor.h>

using namespace trajopt;
using namespace tesseract;
using namespace tesseract_environment;
using namespace tesseract_scene_graph;
using namespace tesseract_collision;
using namespace tesseract_visualization;
using tesseract_common::ManipulatorInfo;

namespace tesseract_ros_examples
{
class TrajOptGlassUprightConstraint : public ompl::base::Constraint
{
public:
  TrajOptGlassUprightConstraint(tesseract::Tesseract::Ptr tesseract,
                                tesseract_kinematics::ForwardKinematics::Ptr fwd_kin,
                                std::string manipulator,
                                std::string tcp_link,
                                tesseract_rosutils::ROSPlottingPtr plotter)
    : ompl::base::Constraint(fwd_kin->numJoints(), 1)
    , tesseract_(std::move(tesseract))
    , fwd_kin_(std::move(fwd_kin))
    , manipulator_(std::move(manipulator))
    , tcp_link_(std::move(tcp_link))
    , plotter_(plotter)
  {
    trajopt_common::gLogLevel = trajopt_common::LevelError;
  }

  ~TrajOptGlassUprightConstraint() override = default;

  void function(const Eigen::Ref<const Eigen::VectorXd>& /*x*/, Eigen::Ref<Eigen::VectorXd> /*out*/) const override
  {
    assert(false);
  }

  virtual bool project(Eigen::Ref<Eigen::VectorXd> x) const override
  {
    trajopt::ProblemConstructionInfo pci(tesseract_);
    pci.basic_info.n_steps = 1;
    pci.basic_info.manip = manipulator_;
    pci.basic_info.start_fixed = false;
    pci.basic_info.use_time = false;

    // Create Kinematic Object
    pci.kin = pci.getManipulator(pci.basic_info.manip);

    pci.init_info.type = trajopt::InitInfo::GIVEN_TRAJ;
    pci.init_info.data = x.transpose();

    pci.opt_info.trust_expand_ratio = 1.1;
    pci.opt_info.trust_expand_ratio = 0.9;
    pci.opt_info.trust_box_size = 1e-2;

    auto collision = std::make_shared<trajopt::CollisionTermInfo>();
    collision->name = "collision";
    collision->term_type = trajopt::TT_CNT;
    collision->evaluator_type = trajopt::CollisionEvaluatorType::SINGLE_TIMESTEP;
    collision->first_step = 0;
    collision->last_step = 0;
    collision->info = trajopt::createSafetyMarginDataVector(pci.basic_info.n_steps, 0.015, 1);
    for (auto& info : collision->info)
    {
      info->setPairSafetyMarginData("base_link", "link_5", 0.05, 1);
      info->setPairSafetyMarginData("link_3", "link_5", 0.01, 1);
      info->setPairSafetyMarginData("link_3", "link_6", 0.01, 1);
    }

    pci.cnt_infos.push_back(collision);

    auto fn = [this](const Eigen::VectorXd& jv) {
      Eigen::Isometry3d pose;
      fwd_kin_->calcFwdKin(pose, jv);

      Eigen::Vector3d z_axis = pose.matrix().col(2).template head<3>().normalized();

      Eigen::Vector3d normal = -1.0 * Eigen::Vector3d::UnitZ();
      Eigen::VectorXd out(1);
      out(0) = std::atan2(z_axis.cross(normal).norm(), z_axis.dot(normal));
      return out;
    };

    trajopt::TermInfo::Ptr ti = tesseract_motion_planners::createUserDefinedTermInfo(1, fn, nullptr, "user_defined");

    // Update the term info with the (possibly) new start and end state indices for which to apply this cost
    Eigen::VectorXd coeff = Eigen::VectorXd::Ones(1);
    std::shared_ptr<trajopt::UserDefinedTermInfo> ef = std::static_pointer_cast<trajopt::UserDefinedTermInfo>(ti);
    ef->term_type = trajopt::TT_CNT;
    ef->constraint_type = sco::ConstraintType::EQ;
    ef->coeff = coeff;
    ef->first_step = 0;
    ef->last_step = 0;
    pci.cnt_infos.push_back(ef);

    trajopt::TrajOptProb::Ptr prob = trajopt::ConstructProblem(pci);

    sco::BasicTrustRegionSQP opt(prob);

    opt.initialize(trajopt::trajToDblVec(prob->GetInitTraj()));
    sco::OptStatus status = opt.optimize();

    if (status != sco::OptStatus::OPT_CONVERGED)
      return false;

    x = trajopt::getTraj(opt.x(), prob->GetVars()).transpose();
    if (plotter_)
      plotter_->plotTrajectory(prob->GetKin()->getJointNames(), x.transpose());
    return true;
  }

  bool isSatisfied(const Eigen::Ref<const Eigen::VectorXd>& /*x*/) const override
  {
    assert(false);
    return false;
  }

  double distance(const Eigen::Ref<const Eigen::VectorXd>& /*x*/) const override
  {
    assert(false);
    return 0;
  }

  void jacobian(const Eigen::Ref<const Eigen::VectorXd>& /*x*/, Eigen::Ref<Eigen::MatrixXd> /*out*/) const override
  {
    assert(false);
  }

private:
  tesseract::Tesseract::Ptr tesseract_;
  tesseract_kinematics::ForwardKinematics::Ptr fwd_kin_;
  std::string manipulator_;
  std::string tcp_link_;
  tesseract_rosutils::ROSPlottingPtr plotter_;
};

class GlassUprightConstraint : public ompl::base::Constraint
{
public:
  GlassUprightConstraint(const Eigen::Vector3d& normal, tesseract_kinematics::ForwardKinematics::Ptr fwd_kin)
    : ompl::base::Constraint(fwd_kin->numJoints(), 1), normal_(normal.normalized()), fwd_kin_(std::move(fwd_kin))
  {
  }

  ~GlassUprightConstraint() override = default;

  void function(const Eigen::Ref<const Eigen::VectorXd>& x, Eigen::Ref<Eigen::VectorXd> out) const override
  {
    Eigen::Isometry3d pose;
    fwd_kin_->calcFwdKin(pose, x);

    Eigen::Vector3d z_axis = pose.matrix().col(2).template head<3>().normalized();

    out[0] = std::atan2(z_axis.cross(normal_).norm(), z_axis.dot(normal_));
  }

private:
  Eigen::Vector3d normal_;
  tesseract_kinematics::ForwardKinematics::Ptr fwd_kin_;
};

GlassUprightOMPLExample::GlassUprightOMPLExample(tesseract_environment::Environment::Ptr env,
                                                 tesseract_visualization::Visualization::Ptr plotter,
                                                 double range,
                                                 double planning_time,
                                                 bool use_trajopt_constraint)
  : Example(std::move(env), std::move(plotter))
  , range_(range)
  , planning_time_(planning_time)
  , use_trajopt_constraint_(use_trajopt_constraint)
{
}

bool GlassUprightOMPLExample::run()
{
  if (plotter_ != nullptr)
    plotter_->waitForConnection();

  // Add sphere to environment
  Link link_sphere("sphere_attached");

  Visual::Ptr visual = std::make_shared<Visual>();
  visual->origin = Eigen::Isometry3d::Identity();
  visual->origin.translation() = Eigen::Vector3d(0.5, 0, 0.55);
  visual->geometry = std::make_shared<tesseract_geometry::Sphere>(0.15);
  link_sphere.visual.push_back(visual);

  Collision::Ptr collision = std::make_shared<Collision>();
  collision->origin = visual->origin;
  collision->geometry = visual->geometry;
  link_sphere.collision.push_back(collision);

  Joint joint_sphere("joint_sphere_attached");
  joint_sphere.parent_link_name = "base_link";
  joint_sphere.child_link_name = link_sphere.getName();
  joint_sphere.type = JointType::FIXED;

  tesseract_->getEnvironment()->addLink(link_sphere, joint_sphere);

  if (rviz_)
  {
    // Now update rviz environment
    if (!sendRvizChanges(0))
      return false;
  }

  // Set the robot initial state
  auto kin = tesseract_->getFwdKinematicsManagerConst()->getFwdKinematicSolver("manipulator");
  std::vector<double> swp = { -0.4, 0.2762, 0.0, -1.3348, 0.0, 1.4959, 0.0 };
  std::vector<double> ewp = { 0.4, 0.2762, 0.0, -1.3348, 0.0, 1.4959, 0.0 };

  tesseract_->getEnvironment()->setState(kin->getJointNames(), swp);

  //  plotter->plotScene();

  // Set Log Level
  trajopt_common::gLogLevel = trajopt_common::LevelError;

  // Setup Problem
  tesseract_motion_planners::OMPLMotionPlanner ompl_planner;

  auto ompl_config =
      std::make_shared<tesseract_motion_planners::OMPLPlannerConstrainedConfig>(tesseract_, "manipulator");

  ompl_config->start_waypoint = std::make_shared<tesseract_motion_planners::JointWaypoint>(swp, kin->getJointNames());
  ompl_config->end_waypoint = std::make_shared<tesseract_motion_planners::JointWaypoint>(ewp, kin->getJointNames());
  ompl_config->collision_safety_margin = 0.01;
  ompl_config->planning_time = planning_time_;
  ompl_config->max_solutions = 2;
  ompl_config->longest_valid_segment_fraction = 0.01;

  ompl_config->collision_continuous = false;
  ompl_config->collision_check = false;
  ompl_config->simplify = false;
  ompl_config->n_output_states = 50;

  if (use_trajopt_constraint_)
  {
    if (plotting_)
      ompl_config->constraint =
          std::make_shared<TrajOptGlassUprightConstraint>(tesseract_, kin, "manipulator", "tool0", plotter);
    else
      ompl_config->constraint =
          std::make_shared<TrajOptGlassUprightConstraint>(tesseract_, kin, "manipulator", "tool0", nullptr);
  }
  else
  {
    Eigen::Vector3d normal = -1.0 * Eigen::Vector3d::UnitZ();
    ompl_config->constraint = std::make_shared<GlassUprightConstraint>(normal, kin);
  }

  for (int i = 0; i < 4; ++i)
  {
    auto rrtconnect_planner = std::make_shared<tesseract_motion_planners::ESTConfigurator>();
    rrtconnect_planner->range = range_;
    ompl_config->planners.push_back(rrtconnect_planner);
  }

  // Set the planner configuration
  ompl_planner.setConfiguration(ompl_config);

  // Solve Trajectory
  CONSOLE_BRIDGE_logInform("glass upright plan OMPL example");

  ros::Time tStart = ros::Time::now();
  tesseract_motion_planners::PlannerResponse ompl_planning_responseb = ompl_planner.solve(ompl_planning_request);
  CONSOLE_BRIDGE_logError("planning time: %.3f", (ros::Time::now() - tStart).toSec());

  double d = 0;
  tesseract_common::TrajArray traj = ompl_planning_response.joint_trajectory.trajectory;
  for (unsigned i = 1; i < traj.rows(); ++i)
  {
    for (unsigned j = 0; j < traj.cols(); ++j)
    {
      d += std::abs(traj(i, j) - traj(i - 1, j));
    }
  }
  CONSOLE_BRIDGE_logError("trajectory norm: %.3f", d);

  if (plotting_)
    plotter->clear();

  if (status)
  {
    auto env = tesseract_->getEnvironmentConst();
    std::vector<ContactResultMap> collisions;
    tesseract_environment::StateSolver::Ptr state_solver = env->getStateSolver();
    ContinuousContactManager::Ptr manager = env->getContinuousContactManager();
    AdjacencyMap::Ptr adjacency_map = std::make_shared<tesseract_environment::AdjacencyMap>(
        env->getSceneGraph(), env->getActiveLinkNames(), env->getCurrentState()->link_transforms);

    manager->setActiveCollisionObjects(adjacency_map->getActiveLinkNames());
    manager->setContactDistanceThreshold(0);
    bool found = checkTrajectory(collisions, *manager, *state_solver, kin->getJointNames(), traj);

    CONSOLE_BRIDGE_logInform((found) ? ("Final trajectory is in collision") : ("Final trajectory is collision free"));

    plotter->plotTrajectory(kin->getJointNames(), traj);
  }
  else
  {
    CONSOLE_BRIDGE_logError("Failed to find a valid trajector: %s", status.message().c_str());
  }

  return response.interface->isSuccessful();
}
}  // namespace tesseract_ros_examples
