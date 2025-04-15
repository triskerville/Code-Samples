// file:    DeltaTime.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Core/DeltaTime.h>

namespace triskerville
{
  DeltaTime::DeltaTime(float& _dt)
    : dt_(_dt)
    , begin_(std::chrono::high_resolution_clock::now())
  {
  }

  DeltaTime::~DeltaTime()
  {
    //  update dt_ to be the time elapsed since this object was created
    auto end = std::chrono::high_resolution_clock::now();
    dt_ = std::chrono::duration<float>(end - begin_).count();
  }
}
