// file:    WindowService.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

#include <Interfaces/IService.h>
#include <Interfaces/IDependencyList.h>
#include <Interfaces/IBindable.h>
#include <Core/Delegate.h>

namespace tbasque
{
  class WindowService : public virtual IService,
                        public virtual IDependencyList<MessageService>,
                        private virtual IBindable
  {
  public:
    WindowService(DependencyList&&);
    ~WindowService();
    WindowService(WindowService&&) noexcept;

    //  this only sets the size variable itself, due to how callbacks work atm
    //  multicast delegates could help to alleviate this
    void SetSize(glm::ivec2 const&);
    glm::ivec2 GetSize() const;
    float GetAspectRatio() const;

    //  exposing another internal so the GUI can hook glfw calls.
    //  not ideal, but no other options here
    GLFWwindow* GetInternalPointer() const;
  private:
    static uint windowCount_;
    static void incrementWindowCount();
    static void decrementWindowCount();

    void FrameBegin(float) override;
    void Bind() const override;
    void Unbind() const override;

    Delegate<void, glm::ivec2> ResizeDelegate;

    GLFWwindow* pWindow_{ nullptr };
    glm::ivec2 size_{};
    glm::dvec2 cursor_{};
  };
}
