// file:    stdafx.h
// author:  Tristan Baskerville
// brief:   Precompiled header, compiles any STL or Third Party includes before
//          compiling internal source files.
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

//  Windows.h defines a macro also found in glfw3.h. due to Windows.h not
//  having a redefinition guard, it must be included before glfw3.h
#include <Windows.h>

//>=--- STL Includes ---=<//
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <memory>
#include <iterator>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <deque>
#include <utility>
#include <functional>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <typeindex>
#include <chrono>
#include <regex>
#include <tuple>
#include <random>

//>=--- Third Party Includes ---=<//
#include <fmod/fmod.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_sprintf.h>

#include <Core/Hash.h>

//>=--- Alias and Constexpr Defines ---=<//
using uint = unsigned int;
using byte = unsigned char;

template <typename T>
using SPointer = std::shared_ptr<T>;
template <typename T>
using UPointer = std::unique_ptr<T>;

template <typename T>
struct Axis
{
  using vec = glm::vec<3, T, glm::qualifier::defaultp>;
  static constexpr vec   Right{  1,  0,  0 };
  static constexpr vec    Left{ -1,  0,  0 };
  static constexpr vec      Up{  0,  1,  0 };
  static constexpr vec    Down{  0, -1,  0 };
  static constexpr vec Forward{  0,  0,  1 };
  static constexpr vec    Back{  0,  0, -1 };

  static constexpr inline std::array<vec, 6> Each = { 
    Right, Left, Up, Down, Forward, Back 
  };
};

#define BIT(bit)            (1ULL << (bit))
#define SET_BIT(num, bit)   ((num) |= BIT(bit)) 
#define CLEAR_BIT(num, bit) ((num) &= ~BIT(bit))
#define FLIP_BIT(num, bit)  ((num) ^= BIT(bit)) 
#define CHECK_BIT(num, bit) (!!((num) & BIT(bit)))
