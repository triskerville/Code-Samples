// file:    Application.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Application/Application.h>
#include <Interfaces/IService.h>
#include <Core/DeltaTime.h>
#include <Core/EResult.h>

namespace tbasque
{
  Application::~Application()
  {
    serviceOrder_.clear();
    serviceCollection_.clear();
  }

  Application::Application(Application&& _other) noexcept
  {
    //  move constructor already defined, have it do the work instead
    *this = std::move(_other);
  }

  Application& Application::operator=(Application&& _other) noexcept
  {
    this->serviceOrder_ = std::move(_other.serviceOrder_);
    this->serviceCollection_ = std::move(_other.serviceCollection_);
    this->isRunning_ = _other.isRunning_;

    return *this;
  }

  void Application::Run()
  {
    float dt = 1.f / 60.f;
    while (isRunning_)
    {
      //  manages dt, no need for the time variable aside from the destructor
      //  being called, which is what updates dt
      DeltaTime time(dt);
      //  update each system and pass dt
      for (auto it = serviceOrder_.begin(); it != serviceOrder_.end(); ++it)
      {
        auto key = *it;
        auto& pService = serviceCollection_.at(key);
        pService->FrameBegin(dt);
        //  if any system reports failure, stop running immediately
        if (pService->ShouldTerminate()) return;
      }
      //  some services are still running, end frame in reverse order
      for (auto it = serviceOrder_.rbegin(); it != serviceOrder_.rend(); ++it)
      {
        auto key = *it;
        auto& pService = serviceCollection_.at(key);
        //  call frame end for any services that override it
        pService->FrameEnd();
      }
    }
  }
}