// file:    IService.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

//  include forward declare for EResult
#include <tbasque_fwd.h>
#include <Interfaces/INoCopy.h>

namespace tbasque
{
  class IService : public virtual INoCopy
  {
  public:
    //  allows for IService to be managed by a parent class
    friend class Application;
    //  polymorphic classes must always use a virtual destructor
    virtual ~IService() = default;
  protected:
    //  We do not want outside classes to touch these functions,
    //  so we tuck them away to only allow Application to access them.
    virtual void FrameBegin(float) = 0;
    virtual void FrameEnd() {};
    //  Determines if a service needs to stop running
    bool ShouldTerminate();
    bool isRunning_{ true };
  };
}
