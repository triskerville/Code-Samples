// file:    IDependencyList.h
// author:  Tristan Baskerville
// brief:   DependencyList implementation. Provides dependent IServices as a 
//          tuple of smart pointers. Constructing a dependent service before 
//          its dependency emits a compiler error.
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace triskerville
{
  template <class... Service>
  class IDependencyList
  {
  public:
    //  gather and store dependent services for future retrievals.
    //  internal tuple can only be read from after initial construction, no writes. 
    using Tuple = std::tuple<std::shared_ptr<Service>...>;

    //  shorthand so we don't have to pass around template params
    using DependencyList = IDependencyList<Service...>;
    static constexpr uint DependencyCount = sizeof...(Service);

    IDependencyList(Tuple&& t) : pServicesTuple_(std::forward<Tuple>(t)) {}
    IDependencyList(DependencyList&&) = default;
    virtual ~IDependencyList() = default;

  protected:
    template <typename S>
    inline std::shared_ptr<S> GetService()
    {
      //  Uses std::get to retrieve our desired system from the internal tuple
      //  if the type S does not exist in the tuple, this does not compile.
      return std::get<std::shared_ptr<S>>(pServicesTuple_);
    }

    template <typename S>
    inline std::shared_ptr<S const> GetService() const
    {
      //  Same as above method but instead returns a const shared pointer. 
      return std::get<std::shared_ptr<S>>(pServicesTuple_);
    }

  private:
    //  Dependencies never change, so it's private to prohibit modification.
    Tuple pServicesTuple_;
  };
}
