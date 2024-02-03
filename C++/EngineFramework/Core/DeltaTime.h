// file:    DeltaTime.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace tbasque
{
  class DeltaTime
  {
  public:
    DeltaTime(float&);
    ~DeltaTime();
  private:
    float& dt_;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
  };
}
