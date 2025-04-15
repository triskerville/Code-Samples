// file:    Application.h
// author:  Tristan Baskerville
// brief:   Application class that installs IServices while building, and
//          manages the lifetime of these services until the application closes.
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

#include <triskerville_fwd.h>
#include <Interfaces/INoCopy.h>
#include <Interfaces/IBuilder.h>

namespace triskerville
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
    
    //  Services run first to last based on their order of installation.
    //  type_index vector is used to remember that ordering.
    std::unordered_map<std::type_index, SPointer<IService>> serviceCollection_{};
    std::vector<std::type_index> serviceOrder_{};
    
    bool isRunning_{ true };
  };

  class Application::Builder : public IBuilder<Application>
  {
  public:
    template <typename S>
    inline Builder& InstallService();

  private:
    template <typename S>
    inline void setDependencyPointer(SPointer<S>& _pServiceRef);
  };
}

template<typename S>
inline auto Application::Builder::InstallService() -> Builder&
{
  static_assert(std::is_base_of<IService, S>::value, "Type passed to InstallService must derive IService.");
  
  typename S::DependencyList::Tuple dependencyTuple{};

  //  call helper function for each element in our Tuple
  std::apply(
    [this](auto&&... pService) { (setDependencyPointer(pService), ...); },
    dependencyTuple
  );

  //  construct our service. pass along the dependency list we just created
  auto pService = std::make_shared<S>(std::move(dependencyTuple));


  //  order MUST be filled along with collection, as it is used to
  //  ensure services are called in the order they are created
  instance_.serviceOrder_.emplace_back(typeid(S));
  instance_.serviceCollection_.emplace(typeid(S), pService);

  return *this;
}

template<typename S>
inline void Application::Builder::setDependencyPointer(SPointer<S>& _pServiceRef)
{
  //  find the instance of our dependency that Application manages.
  //  fails fast if no instance exists (missing dependency).
  auto service = instance_.serviceCollection_.at(typeid(S));
  //  forward this service to the dependee service, to be retrieved later
  _pServiceRef = std::dynamic_pointer_cast<S>(service);
}

