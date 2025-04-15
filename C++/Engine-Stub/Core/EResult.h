// file:    EResult.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace triskerville
{
  enum class EResult
  {
    Failure = 0,
    Success = 1
  };

  inline bool operator!(EResult _result)
  {
    return _result == EResult::Failure;
  }
}
