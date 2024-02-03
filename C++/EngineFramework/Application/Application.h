// file:    Application.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.
#pragma once

#include <tbasque_fwd.h>
#include <Interfaces/INoCopy.h>
#include <Interfaces/IBuilder.h>

namespace tbasque
{
  class Application : public virtual INoCopy
  {
  public:
    class Builder;

    Application() = default;
    ~Application();

    Application(Application&&) noexcept;
    Application& operator=(Application&&) noexcept;

    void Run();
  private:
    //  because we lose ordering with an unordered_map,
    //  we preserve it here by appending the key to this collection
    std::vector<std::type_index> serviceOrder_{};
    //  because we are storing pointers to the base class,
    //  we use the derived type as the key
    std::unordered_map<
      std::type_index, SPointer<IService>> serviceCollection_{};
    bool isRunning_{ true };
  };

  class Application::Builder : public IBuilder<Application>
  {
  public:
    template <typename T>
    Builder& AppendService();
  private:
    template <typename T>
    void SetDependencyService(SPointer<T>& _pService);
  };
}

#include <Application/Application.inl>
