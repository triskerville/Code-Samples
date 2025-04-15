// file:    IRenderable.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Interfaces/IRenderable.h>

namespace triskerville
{
  IRenderable::IRenderable(IRenderable&& _other) noexcept
    : handle_(_other.handle_)
  {
    _other.handle_ = 0;
  }

  uint IRenderable::GetHandle() const
  {
    return handle_;
  }
}
