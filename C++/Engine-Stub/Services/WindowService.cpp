// file:    WindowService.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Services/WindowService.h>
#include <Services/MessageService.h>
#include <Services/SceneLogic.h>
#include <Core/EResult.h>
#include <Core/InputEvent.h>
#include <Graphics/Common.h>

namespace tbasque
{
  //  used to help keep track of GLFW/Glad initialization
  uint WindowService::windowCount_ = 0;

  WindowService::WindowService(DependencyList&& _dependencies)
    : DependencyList(std::forward<DependencyList>(_dependencies))
    , size_{ Graphics::Common::INIT_WIDTH, Graphics::Common::INIT_HEIGHT }
  {
    incrementWindowCount();
    //  creates a core OpenGL context with version 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow_ = glfwCreateWindow(size_.x, size_.y, "Engine Stub", nullptr, nullptr);
    this->Bind();

    //  hijack the UserPointer to store the instance of this class,
    //  we can cast the pointer later to use the delegate in this instance
    glfwSetWindowUserPointer(pWindow_, this);
    glfwSetFramebufferSizeCallback(pWindow_, 
      +[](GLFWwindow* _window, int _width, int _height)
      {
        //  GLFW callbacks are C-style, meaning we cannot bind an instance
        //  to the function. Instead, we can make use of the user pointer
        //  to store a pointer to this class
        auto pWindowService = static_cast<WindowService*>(
          glfwGetWindowUserPointer(_window));
        pWindowService->SetSize(glm::ivec2{ _width, _height });
        pWindowService->ResizeDelegate(glm::ivec2{ _width, _height });
      }
    );
    glfwSetKeyCallback(pWindow_, 
      [](GLFWwindow*, int key, int scancode, int action, int mod) 
      {
        ButtonEvent info = { key, mod };
        switch (action) {
          case GLFW_PRESS: { Send_Message(Hash("ButtonPress"), MessageService::AllObservers, &info); }
          break;

          case GLFW_RELEASE: { Send_Message(Hash("ButtonRelease"), MessageService::AllObservers, &info); }
          break;

          default: {}
          break;
        }
      }
    );
    glfwSetMouseButtonCallback(pWindow_, 
      [](GLFWwindow* window, int button, int action, int mod) 
      {
          ButtonEvent info = { button, mod };
          switch (action) {
              case GLFW_PRESS: { Send_Message(Hash("ButtonPress"), MessageService::AllObservers, &info); }
              break;

              case GLFW_RELEASE: { Send_Message(Hash("ButtonRelease"), MessageService::AllObservers, &info); }
              break;

              default: {}
              break;
          }
      }
    );
    glfwSetCursorEnterCallback(pWindow_, 
      [](GLFWwindow* window, int is_hovering_window) 
      {
          Send_Message(Hash("MouseEnterWindow"), MessageService::AllObservers, is_hovering_window ? &is_hovering_window : nullptr); // NOTE: Payload acts as bool =]
      }
    );
    glfwSetScrollCallback(pWindow_, 
      [](GLFWwindow* window, double d_x, double d_y) 
      {
          ScrollEvent info = { d_x, d_y };
          Send_Message(Hash("Scroll"), MessageService::AllObservers, &info);
      }
    );

    //  hook up OpenGL functions for RenderingService
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  }

  WindowService::~WindowService()
  {
    //  we don't want to delete our window if we are in the moved-from state
    if (pWindow_ == nullptr) return;

    //  unsure if glfw cleans this up or not, but just to be on the safe side
    //  I am also removing the callback
    glfwSetFramebufferSizeCallback(pWindow_, nullptr);
    glfwSetKeyCallback(pWindow_, nullptr);
    glfwSetMouseButtonCallback(pWindow_, nullptr);
    glfwSetCursorEnterCallback(pWindow_, nullptr);
    glfwSetScrollCallback(pWindow_, nullptr);

    glfwDestroyWindow(pWindow_);
    //  let Window class know a window has been destroyed, if this is final
    //  window, terminates GLFW
    WindowService::decrementWindowCount();
  }

  WindowService::WindowService(WindowService&& _other) noexcept
    : DependencyList(std::move(_other))
  {
    std::swap(this->pWindow_, _other.pWindow_);
    std::swap(this->size_, _other.size_);
  }

  void WindowService::SetSize(glm::ivec2 const& _size)
  {
    size_ = _size;
  }

  glm::ivec2 WindowService::GetSize() const
  {
    return size_;
  }

  float WindowService::GetAspectRatio() const
  {
    return static_cast<float>(size_.x) / size_.y;
  }

  void WindowService::Bind() const
  {
    glfwMakeContextCurrent(pWindow_);
  }

  void WindowService::Unbind() const
  {
    glfwMakeContextCurrent(nullptr);
  }

  void WindowService::FrameBegin(float)
  {
    //  Common logic to set-up the next frames' input and FBO
    glfwSwapBuffers(pWindow_);
    glfwPollEvents();

    if (glfwWindowShouldClose(pWindow_))
    {
      isRunning_ = false;
      return;
    }

    CursorEvent info;
    glfwGetCursorPos(pWindow_, &info.Pos.x, &info.Pos.y);
    info.Delta = info.Pos - cursor_;
    cursor_ = info.Pos;
    if (glm::abs(info.Delta.x) >= DBL_EPSILON || glm::abs(info.Delta.y) >= DBL_EPSILON) 
    {
      Send_Message(Hash("MouseMove"), MessageService::AllObservers, &info);
    }
  }

  GLFWwindow* WindowService::GetInternalPointer() const
  {
    return pWindow_;
  }

  void WindowService::incrementWindowCount()
  {
    if (windowCount_ == 0)
    {
      auto result = glfwInit();
    }

    ++windowCount_;
  }

  void WindowService::decrementWindowCount()
  {
    --windowCount_;

    if (windowCount_ == 0)
    {
      glfwTerminate();
    }
  }
}
