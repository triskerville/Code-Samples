// file:    Application.inl
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

namespace tbasque
{
  template<typename T>
  inline auto Application::Builder::AppendService() -> Builder&
  {
    static_assert(std::is_base_of<IService, T>::value,
                  "Service T must derive from IService.");
    //  we cannot modify tuple after building DependencyList,
    //  so we start with the internal tuple first
    typename T::DependencyList::Tuple dependencyTuple{};
    //  call helper function for each element in our Tuple
    std::apply([this](auto&&... args) { (SetDependencyService(args), ...); }
      , dependencyTuple);
    //  construct our service and pass along the dependency list we created
    auto pService = std::make_shared<T>(std::move(dependencyTuple));
    //  order MUST be filled along with collection, as it is used to
    //  ensure services are called in the order they are created
    instance_.serviceOrder_.emplace_back(typeid(T));
    instance_.serviceCollection_.emplace(typeid(T), pService);

    return *this;
  }

  template<typename T>
  inline void Application::Builder::SetDependencyService(
    SPointer<T>& _pService)
  {
    //  get the instance of the dependency service from the collection of
    //  services the application manages
    auto service = instance_.serviceCollection_.at(typeid(T));
    //  forward this service to the dependent service, to be retrieved later
    _pService = std::dynamic_pointer_cast<T>(service);
  }
}