// file:    Hash.h
// author:  Tristan Baskerville, Gray Bangs
// brief:   Provides the Hash class implementation, which creates a hash from a
//          C-style string. 
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace tbasque
{
  class Hash 
  {
    //  Author: Gray Bangs
    constexpr uint64_t calc_hash(const char* c) 
    {
      if (c == nullptr || *c == '\0')
        return 0;

      //  Source:  https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
      constexpr uint64_t  base = 14695981039346656037;
      constexpr uint64_t prime = 1099511628211;
      uint64_t hash = base;
      for (; *c != '\0'; ++c) 
      {
        hash *= prime;
        hash ^= *c;
      }

      return hash;
    }

    uint64_t val_;

  public:
    constexpr explicit Hash(const char* c) : val_(calc_hash(c)) {}
    constexpr Hash(uint64_t val) : val_(val) {}
    constexpr operator uint64_t() const { return val_; }
  };
}
