// file:    IBuilder.inl
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

namespace tbasque
{
  /// Returns the instance relative to the type of Builder instantiated. 
  /// Uses a move operation so that the object can avoid copies.
  ///
  /// \tparam T Type of object this Builder is constructing
  ///
  /// \returns  The instance stored in T as an r-value
  template<typename T>
  inline T&& IBuilder<T>::Build()
  {
    return std::move(instance_);
  }
}