// file:    Transform.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

#include <Interfaces/IBuilder.h>

namespace triskerville::World
{
  class Transform
  {
  public:
    class Builder;
    glm::mat4 GetMatrix() const;
    glm::vec3 GetTranslation() const;
    glm::vec3 GetScale() const;
    glm::vec3 GetRotation() const;
    glm::vec3 GetForward() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetRight() const;
    void SetTranslation(glm::vec3 const&);
    void SetScale(glm::vec3 const&);
    void SetRotation(glm::vec3 const&);
  private:
    glm::vec3 translation_{};
    glm::vec3 scale_{ 1.f };
    glm::vec3 rotation_{};
  };

  class Transform::Builder : public IBuilder<Transform>
  {
  public:
    Builder& WithTranslation(glm::vec3 const&);
    Builder& WithScale(glm::vec3 const&);
    Builder& WithRotation(glm::vec3 const&);
  };
}
