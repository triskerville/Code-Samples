//>=------------------------------------------------------------------------=<//
// file:    IDependencyList.h
// author:  Tristan Baskerville
// brief:   Provides dependent IServices as a Tuple of shared pointers.
//          Constructing a dependent service before its dependency emits
//          a compiler error.
//
// Copyright © 2022 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//
#ifndef IDEPENDENCY_LIST_H
#define IDEPENDENCY_LIST_H

namespace tbasque
{
  template <class... T>
  class IDependencyList
  {
  public:
    //  no modification to the tuple allowed after construction, so this enables
    //  outside classes to know what kind of tuple to pass to the constructor
    using Tuple = std::tuple<std::shared_ptr<T>...>;
    //  shorthand so we don't have to pass around template params
    using DependencyList = IDependencyList<T...>;
    //  shorthand for getting the total NUMBER of dependencies
    static constexpr uint DependencyCount = sizeof...(T);

    IDependencyList(Tuple&& t) : pServices_(std::forward<Tuple>(t)) {}
    IDependencyList(DependencyList&&) = default;

  protected:
    template <typename S>
    std::shared_ptr<S> GetService()
    {
      //  Uses std::get to retrieve our desired system from the internal tuple
      //  if the type S does not exist in the tuple, this does not compile.
      return std::get<std::shared_ptr<S>>(pServices_);
    }

    template <typename S>
    std::shared_ptr<S const> GetService() const
    {
      //  Same as above method but instead returned as a const shared pointer. 
      return std::get<std::shared_ptr<S>>(pServices_);
    }
  private:
    //  Stores pointers to dependent systems for GetService
    Tuple pServices_;
  };
}

#endif