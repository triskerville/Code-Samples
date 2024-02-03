// file:    IRenderable.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

#include <Interfaces/IBindable.h>
#include <Interfaces/INoCopy.h>

namespace tbasque
{
  /// Defines an interface for an object that is typically passed to 
  /// a rendering API such as OpenGL. We use virtual inheritance to avoid
  /// cases of diamond inheritance. ALL renderables should avoid copies,
  /// and allow for binding/unbinding
  ///
  /// \author Tristan Baskerville
  /// \date 12/16/2021
  class IRenderable : public virtual INoCopy
  {
  public:
    IRenderable() = default;
    IRenderable(IRenderable&&) noexcept;
    //  every derivable class needs this bad boi
    virtual ~IRenderable() = default;
    //  shorthand to get internal OpenGL handle
    uint GetHandle() const;
  protected:
    //  internal handle used for render calls
    uint handle_{ 0 };
  };
}
