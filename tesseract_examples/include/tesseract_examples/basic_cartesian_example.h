/**
 * @file basic_cartesian_example.h
 * @brief Basic example leveraging trajopt and tesseract for cartesian planning
 *
 * @author Levi Armstrong
 * @date July 22, 2018
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
#ifndef TESSERACT_EXAMPLES_BASIC_CARTESIAN_EXAMPLE_H
#define TESSERACT_EXAMPLES_BASIC_CARTESIAN_EXAMPLE_H

#include <tesseract_common/macros.h>

#include <tesseract_examples/example.h>

namespace tesseract_examples
{
/**
 * @brief Basic example leveraging trajopt and tesseract for cartesian planning
 */
class BasicCartesianExample : public Example
{
public:
  BasicCartesianExample(tesseract_environment::Environment::Ptr env,
                        tesseract_visualization::Visualization::Ptr plotter = nullptr,
                        bool ifopt = false,
                        bool debug = false);

  ~BasicCartesianExample() override = default;
  BasicCartesianExample(const BasicCartesianExample&) = default;
  BasicCartesianExample& operator=(const BasicCartesianExample&) = default;
  BasicCartesianExample(BasicCartesianExample&&) = default;
  BasicCartesianExample& operator=(BasicCartesianExample&&) = default;

  bool run() override final;

private:
  bool ifopt_;
  bool debug_;
  static tesseract_environment::Command::Ptr addPointCloud();
};

}  // namespace tesseract_examples

#endif  // TESSERACT_ROS_EXAMPLES_BASIC_CARTESIAN_EXAMPLE_H
