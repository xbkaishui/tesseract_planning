/**
 * @file examples.h
 * @brief Examples base class
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
#ifndef TESSERACT_ROS_EXAMPLES_EXAMPLES_H
#define TESSERACT_ROS_EXAMPLES_EXAMPLES_H

#include <tesseract_common/macros.h>

#include <tesseract_environment/environment.h>
#include <tesseract_visualization/visualization.h>

namespace tesseract_examples
{
/**
 * @brief The Example base class
 *
 * It provides a generic interface for all examples as a library which then
 * can easily be integrated as unit tests so breaking changes are caught.
 * Also it provides a few utility functions for checking rviz environment and
 * updating the rviz environment.
 */
class Example
{
public:
  Example(tesseract_environment::Environment::Ptr env, tesseract_visualization::Visualization::Ptr plotter = nullptr)
    : env_(std::move(env)), plotter_(std::move(plotter))
  {
  }

  virtual ~Example() = default;
  Example(const Example&) = default;
  Example& operator=(const Example&) = default;
  Example(Example&&) = default;
  Example& operator=(Example&&) = default;

  virtual bool run() = 0;

protected:
  /** @brief Tesseract Manager Class (Required) */
  tesseract_environment::Environment::Ptr env_;
  /** @brief Tesseract Visualization Class (Optional)*/
  tesseract_visualization::Visualization::Ptr plotter_;
};

}  // namespace tesseract_examples
#endif  // TESSERACT_ROS_EXAMPLES_EXAMPLES_H
