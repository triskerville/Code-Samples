// file:    IBindable.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace tbasque
{
  /// defines an interface where the only desired functions are Bind/Unbind,
  /// which is a commonly used pattern in this framework
  ///
  /// \author Tristan Baskerville
  /// \date 10/16/2021
  class IBindable
  {
  public:
    virtual ~IBindable() = default;

    virtual void Bind() const = 0;
    virtual void Unbind()const  = 0;
  };
}
