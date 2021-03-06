/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2019 TeMoto Telerobotics
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Author: Robert Valner */

#ifndef TEMOTO_COMPONENT_MANAGER__PIPE_INFO_H
#define TEMOTO_COMPONENT_MANAGER__PIPE_INFO_H

#include "temoto_core/common/temoto_log_macros.h"
#include "temoto_core/common/reliability.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <ctype.h>
#include <memory>                     // shared_ptr
#include "yaml-cpp/yaml.h"
#include <iostream>                   // TODO: remove

namespace temoto_component_manager
{

/**
 * @brief The SegmentCategory enum. TODO: DEPRECATED
 */
enum class SegmentCategory : int
{
  SENSOR,
  ALGORITHM
};

/**
 * @brief The Segment struct
 */
struct Segment
{
  std::string segment_type_;                          // Camera ... or ARtag detector ...
  std::set<std::string> required_input_topic_types_;  // The types of the topics that this segment requires
  std::set<std::string> required_output_topic_types_; // The types of the topics that this segment must publish
  std::set<std::string> required_parameters_;         // The types of parameters this segment requires

  /// add input topic type
  void addInputTopicType(std::string topic_type)
  {
    required_input_topic_types_.insert(topic_type);
  }

  /// add output topic type
  void addOutputTopicType(std::string topic_type)
  {
    required_output_topic_types_.insert(topic_type);
  }

  /// add output topic type
  void addRequiredParameter(std::string required_parameter)
  {
    required_parameters_.insert(required_parameter);
  }

  /// to string
  std::string toString() const
  {
    std::string str;
    str += "|_+_segment type: " + segment_type_ + "\n";

    // Print out the input topics
    if (!required_input_topic_types_.empty())
    {
      str += "| |_required input topic types: ";
      for (auto& topic : required_input_topic_types_)
      {
        str += topic;
        if (topic != *std::prev(required_input_topic_types_.end()))
        {
          str += ", ";
        }
      }
      str += "\n";
    }

    // Print out the output topics
    if (!required_output_topic_types_.empty())
    {
      str += "| |_required output topic types: ";
      for (auto& topic : required_output_topic_types_)
      {
        str += topic;
        if (topic != *std::prev(required_output_topic_types_.end()))
        {
          str += ", ";
        }
      }
      str += "\n";
    }

    return str;
  }
};

/**
 * @brief operator ==
 * @param f1
 * @param f2
 * @return
 */
static bool operator==(const Segment& f1, const Segment& f2)
{
  // Check the category, type and topic types
  return f1.segment_type_ == f2.segment_type_ &&
         f1.required_input_topic_types_ == f2.required_input_topic_types_ &&
         f1.required_output_topic_types_ == f2.required_output_topic_types_ &&
         f1.required_parameters_ == f2.required_parameters_;
}

/**
 * @brief operator <<
 * @param out
 * @param f
 * @return
 */
static std::ostream& operator<<(std::ostream& out, const Segment& f)
{
    out << f.toString();

    return out;
}

/**
 * @brief The PipeInfo class
 */
class PipeInfo
{
public:

  temoto_core::Reliability reliability_;

  PipeInfo(){}

  PipeInfo(const PipeInfo& pipe_info, std::string name)
  : PipeInfo(pipe_info)
  {
    name_ = name;
  }

  /*
   * Getters
   */

  /// Get type
  const std::string& getType() const
  {
    return type_;
  }

  const std::string& getName() const
  {
    return name_;
  }

  /// Get pipe
  const std::vector<Segment>& getSegments() const
  {
    return segments_;
  }

  /// Get pipe size
  unsigned int getPipeSize() const
  {
    return segments_.size();
  }

  /*
   * Setters
   */

  /// Set the pipe
  void setSegments(const std::vector<Segment>& segments)
  {
    segments_ = segments;
  }

  void setType(const std::string& type)
  {
    type_ = type;
  }

  void setName(std::string name)
  {
    name_ = name;
  }

  /// Add segment
  void addSegment(const Segment& segment)
  {
    segments_.push_back(segment);
  }

  std::string toString() const
  {
    std::string str;
    str += "type: " + std::string("TODO") + "\n";
    str += "reliability: " + std::to_string(reliability_.getReliability()) + "\n";

    for (const auto& segment : segments_)
    {
      str += segment.toString();

      if (&segment != &segments_.back())
      {
        str += "| \n";
      }
    }

    return str;
  }

  /**
   * @brief operator ==
   * @param t1
   * @param t2
   * @return
   */
  friend bool operator==(const PipeInfo& t1, const PipeInfo& t2)
  {
    return t1.type_ == t2.type_ && t1.segments_ == t2.segments_;
  }

private:
  
  std::string type_;
  std::string name_;
  std::vector<Segment> segments_;
};

/**
 * @brief 
 * 
 */
typedef std::vector<PipeInfo> PipeInfos;

/**
 * @brief PipeInfoPtr
 */
typedef std::shared_ptr<PipeInfo> PipeInfoPtr;

/**
 * @brief PipeInfoPtrs
 */
typedef std::vector<PipeInfoPtr> PipeInfoPtrs;

} // namespace temoto_component_manager

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *                      YAML PARSER FOR PIPE INFO CLASS
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace YAML
{
template <>
struct convert<temoto_component_manager::PipeInfo>
{
  static Node encode(const temoto_component_manager::PipeInfo& pipe_info)
  {
    Node method;
    std::vector<temoto_component_manager::Segment> pipe = pipe_info.getSegments();
    for (auto& segment : pipe)
    {
      // Encode the segment
      Node segment_node;
      segment_node["segment_type"] = segment.segment_type_;

      // Encode the input topic types (if this segment has any)
      if (segment.required_input_topic_types_.size() != 0)
      {
        for (auto& topic_type : segment.required_input_topic_types_)
        {
          segment_node["input_topic_types"].push_back(topic_type);
        }
      }

      // Encode the output topic types (if this segment has any)
      if (segment.required_output_topic_types_.size() != 0)
      {
        for (auto& topic_type : segment.required_output_topic_types_)
        {
          segment_node["output_topic_types"].push_back(topic_type);
        }
      }

      // Encode the output topic types (if this segment has any)
      if (segment.required_parameters_.size() != 0)
      {
        for (auto& parameter : segment.required_parameters_)
        {
          segment_node["required_parameters"].push_back(parameter);
        }
      }

      // Push the segment
      method.push_back(segment_node);
    }

    return method;
  }

  static bool decode(const Node& node, temoto_component_manager::PipeInfo& pipe_info)
  {
    // Check if the "node" is a map
    if (!node.IsMap())
    {
      return false;
    }

    // Get the pipe (sequence of segments) node
    YAML::Node segments_node = node["method"];

    // Iterate over each segment
    for (YAML::const_iterator segment_it = segments_node.begin(); segment_it != segments_node.end(); ++segment_it)
    {
      // Check if the segment is a map
      if (!segment_it->IsMap())
      {
        return false;
      }

      // Create an empty segment object and fill it
      temoto_component_manager::Segment segment;

      try
      {
        segment.segment_type_ = (*segment_it)["segment_type"].as<std::string>();
      }
      catch (YAML::InvalidNode e)
      {
        // Print out the error message
        // TODO: throw a proper error
        std::cout << "The segment node is either missing category or type\n";
        return false;
      }

      // Get the input topic types (if there are any)
      try
      {
        Node input_topics_node = (*segment_it)["input_topic_types"];
        for (YAML::const_iterator topics_it = input_topics_node.begin(); topics_it != input_topics_node.end(); ++topics_it)
        {
          segment.addInputTopicType(topics_it->as<std::string>());
        }
      }
      catch (YAML::InvalidNode e)
      {
        // TODO: REPORT OR DO SOMETHING
      }

      // Get the output topic types (if there are any)
      try
      {
        Node output_topics_node = (*segment_it)["output_topic_types"];
        for (YAML::const_iterator topics_it = output_topics_node.begin(); topics_it != output_topics_node.end(); ++topics_it)
        {
          segment.addOutputTopicType(topics_it->as<std::string>());
        }
      }
      catch (YAML::InvalidNode e)
      {
        // REPORT OR DO SOMETHING
      }

      // Get the output topic types (if there are any)
      try
      {
        Node parameters_node = (*segment_it)["required_parameters"];
        for (YAML::const_iterator parameters_it = parameters_node.begin(); parameters_it != parameters_node.end(); ++parameters_it)
        {
          segment.addRequiredParameter(parameters_it->as<std::string>());
        }
      }
      catch (YAML::InvalidNode e)
      {
        // REPORT OR DO SOMETHING
      }

      pipe_info.addSegment(segment);
    }

    return true;
  }
};
}
#endif
