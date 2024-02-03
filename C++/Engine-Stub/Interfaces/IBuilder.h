// file:    IBuilder.h
// author:  Tristan Baskerville
// brief:   Builder classes almost always use the same Build() implementation,
//          this class defines a common interface to prioritize move ops.
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace tbasque
{
  /// Interface class for buildable objects. Provides a common function to 
  /// output the object instance with a move operation.
  ///
  /// \tparam T Type of object being built.
  template <typename T>
  class IBuilder
  {
  public:
    /// Returns the object instance using a move operation so that the 
    /// object can avoid copies.
    ///
    /// \tparam T Type of object this Builder is constructing
    ///
    /// \returns  The instance stored in T as an r-value
    [[nodiscard]] T&& Build() 
    {
      return std::move(instance_); 
    }
  protected:
    T instance_{};
  };
}
