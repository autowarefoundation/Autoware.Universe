/*
 * UOS-ROS packages - Robot Operating System code by the University of Osnabrück
 * Copyright (C) 2014 University of Osnabrück
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 * and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * route_set_tool.cpp
 *
 * Author: Henning Deeken {hdeeken@uos.de}
 *
 */

#include "route_set_tool.hpp"

#include "rviz_common/display_context.hpp"
#include "rviz_common/view_manager.hpp"

#include <iostream>

namespace tier4_adapi_rviz_plugins
{
RouteSetTool::RouteSetTool()
{
  shortcut_key_ = 'r';
}

RouteSetTool::~RouteSetTool()
{
}

void RouteSetTool::onInitialize()
{
  setName("Route Set");

  step_length_property_ = new rviz_common::properties::FloatProperty(
    "Step Length", 0.1, "The length by with the position is updated on each step.",
    getPropertyContainer(), SLOT(setOffset()), this);

  boost_property_ = new rviz_common::properties::FloatProperty(
    "Boost Property", 0.5, "Gives the boost factor which is applied if pressing shift.",
    getPropertyContainer(), SLOT(setBoost()), this);

  fly_property_ = new rviz_common::properties::BoolProperty(
    "Fly Mode", false, "In fly mode it is possible to move along the z axis as well.",
    getPropertyContainer(), SLOT(setFlyMode()), this);

  left_hand_property_ = new rviz_common::properties::BoolProperty(
    "Left Hand Mode", false, "In left hand mode one uses the arrows to move around",
    getPropertyContainer(), SLOT(setLeftHandMode()), this);

  fallback_view_controller_property_ = new rviz_common::properties::EnumProperty(
    "Fallback ViewController", QString("tier4_adapi_rviz_plugins/RouteSet"),
    "Determines to which view controller the control switches, if the tool is deactivated.",
    getPropertyContainer(), SLOT(setFallbackViewController()), this);

  m_pos_offset = 0.1;
  m_boost = 0.5;
  m_fly_mode = false;
  m_left_hand_mode = false;

  // temporarily disabled
  // setFallbackToolProperty();
  setFallbackViewControllerProperty();
}

void RouteSetTool::setFallbackViewControllerProperty()
{
  fallback_view_controller_property_->clearOptions();
  m_view_controller_classes.clear();

  m_view_controller_classes = context_->getViewManager()->getDeclaredClassIdsFromFactory();

  for (int i = 0; i < m_view_controller_classes.size(); i++) {
    if (m_view_controller_classes[i] != QString("tier4_adapi_rviz_plugins/RouteSet")) {
      fallback_view_controller_property_->addOption(m_view_controller_classes[i], i);
      m_view_controller.push_back(context_->getViewManager()->getViewAt(i));
    }
  }

  fallback_view_controller_property_->show();
  setFallbackViewController();
}

// temporarily disabled
// void RouteSetTool::setFallbackToolProperty()
// {
//   fallback_tool_property_->clearOptions();
//   m_tools.clear();

//   m_tool_classes = context_->getToolManager()->getToolClasses();

//   for (int i = 0; i < m_tool_classes.size(); i++) {
//     if (m_tool_classes[i] != getClassId()) {
//       fallback_tool_property_->addOption(m_tool_classes[i], i);
//       m_tools.push_back(context_->getToolManager()->getTool(i));
//     }
//   }

//   fallback_tool_property_->show();
//   setFallbackTool();
// }

void RouteSetTool::activate()
{
  context_->getViewManager()->setCurrentViewControllerType(
    QString("tier4_adapi_rviz_plugins/RouteSet"));
  context_->getViewManager()->getCurrent()->reset();

  // temporarily disabled
  // setFallbackToolProperty();
  setFallbackViewControllerProperty();
}

void RouteSetTool::deactivate()
{
}

int RouteSetTool::processKeyEvent(QKeyEvent * event, rviz_common::RenderPanel * panel)
{
  return 0;
}

int RouteSetTool::processMouseEvent(rviz_common::ViewportMouseEvent & event)
{
  if (event.panel->getViewController()) {
    event.panel->getViewController()->handleMouseEvent(event);
    setCursor(event.panel->getViewController()->getCursor());
  }
  return 0;
}

}  // namespace tier4_adapi_rviz_plugins

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(tier4_adapi_rviz_plugins::RouteSetTool, rviz_common::Tool)
