// file:    INoCopy.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace triskerville
{
  /// defines a class that cannot be copied, used when a class has either an 
  /// expensive copy instruction, or doesn't make sense to copy. this class 
  /// deletes the copy constructor and only gives room for moving
  ///
  /// \author Tristan Baskerville
  /// \date 7/14/2021
  class INoCopy
  {
  public:
    //  to make the class abstract, the destructor is the easiest choice.
    //  we still need to provide a body, so we do that outside the class body.
    virtual ~INoCopy() = 0;
    INoCopy(const INoCopy&) = delete;
    INoCopy& operator=(const INoCopy&) = delete;
  protected:
    INoCopy() = default;
  };

  //  there must be a function body to allow the virtual destructor to
  //  destroy child objects; this line allows us to define the body here
  //  while keeping the destructor pure virtual.
  inline INoCopy::~INoCopy() = default;
}
