// file:    IService.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Interfaces/IService.h>

namespace tbasque
{
  bool IService::ShouldTerminate()
  {
    //  if the services wishes to stop running, terminate the process
    return not isRunning_;
  }
}