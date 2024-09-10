/**
 * @file pick_and_place_example.h
 * @brief An example of a robot picking up a box and placing it on a shelf.
 *
 * @author Mathew Powelson
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
#ifndef TESSERACT_EXAMPLES_PICK_AND_PLACE_EXAMPLE_H
#define TESSERACT_EXAMPLES_PICK_AND_PLACE_EXAMPLE_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <array>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_common/types.h>
#include <tesseract_examples/example.h>

namespace tesseract_examples
{
/**
 * @brief An example of a robot picking up a box and placing it on a shelf leveraging
 * tesseract and trajopt to generate the motion trajectory.
 */
class PickAndPlaceExample : public Example
{
public:
  PickAndPlaceExample(tesseract_environment::Environment::Ptr env,
                      tesseract_visualization::Visualization::Ptr plotter = nullptr,
                      double box_size = 0.2,
                      std::array<double, 2> box_position = { 0.15, 0.4 });
  ~PickAndPlaceExample() override = default;
  PickAndPlaceExample(const PickAndPlaceExample&) = default;
  PickAndPlaceExample& operator=(const PickAndPlaceExample&) = default;
  PickAndPlaceExample(PickAndPlaceExample&&) = default;
  PickAndPlaceExample& operator=(PickAndPlaceExample&&) = default;

  bool run() override final;

private:
  double box_size_;
  std::array<double, 2> box_position_;
  static tesseract_environment::Command::Ptr addBox(double box_x, double box_y, double box_side);
};

}  // namespace tesseract_examples

#endif  // TESSERACT_EXAMPLES_PICK_AND_PLACE_EXAMPLE_H
