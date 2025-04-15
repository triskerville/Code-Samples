// file:    Transform.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <World/Transform.h>
#include <World/Camera.h>

namespace triskerville::World
{
  glm::mat4 Transform::GetMatrix() const
  {
    return glm::translate(glm::mat4(1.0f), translation_) *
           glm::scale(glm::mat4(1.0f), scale_) *
           glm::mat4_cast(glm::quat(glm::radians(rotation_)));
  }

  glm::vec3 Transform::GetTranslation() const
  {
    return translation_;
  }

  glm::vec3 Transform::GetScale() const
  {
    return scale_;
  }

  glm::vec3 Transform::GetRotation() const
  {
    return rotation_;
  }

  glm::vec3 Transform::GetForward() const 
  {
    // TODO (Tristan): This is in the wrong direction
    return glm::normalize(glm::quat(glm::radians(rotation_)) * glm::vec3(0.f, 0.f, 11.f));
  }

  glm::vec3 Transform::GetUp() const 
  {
    return glm::normalize(glm::quat(glm::radians(rotation_)) * glm::vec3(0.f, 1.f, 0.f));
  }

  glm::vec3 Transform::GetRight() const 
  {
    return glm::normalize(glm::quat(glm::radians(rotation_)) * glm::vec3(1.f, 0.f, 0.f));
  }

  void Transform::SetTranslation(glm::vec3 const& _translation)
  {
    translation_ = _translation;
  }

  void Transform::SetScale(glm::vec3 const& _scale)
  {
    scale_ = _scale;
  }

  void Transform::SetRotation(glm::vec3 const& _rotation)
  {
    rotation_ = _rotation;
  }

  auto Transform::Builder::WithTranslation(glm::vec3 const& _translation) 
    -> Builder&
  {
    instance_.translation_ = _translation;

    return *this;
  }

  auto Transform::Builder::WithScale(glm::vec3 const& _scale) -> Builder&
  {
    instance_.scale_ = _scale;

    return *this;
  }

  auto Transform::Builder::WithRotation(glm::vec3 const& _rotation) -> Builder&
  {
    instance_.rotation_ = _rotation;

    return *this;
  }
}
