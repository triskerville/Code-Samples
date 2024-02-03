// file:    Quad.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Primitive/Quad.h>

namespace tbasque::Primitive
{
  Quad2D::Quad2D(Elements const& _e)
    : elements(_e)
  {
  }

  Quad2D Quad2D::operator+(glm::vec2 const& _offset)
  {
    return std::make_tuple(
      std::get<0>(elements) + _offset, std::get<1>(elements) + _offset,
      std::get<2>(elements) + _offset, std::get<3>(elements) + _offset);
  }

  Quad2D MakeQuad2D(float _xMin, float _xMax, float _yMin, float _yMax)
  {
    return std::make_tuple(
      //  top left, bottom left, bottom right, top right
      glm::vec2{ _xMin, _yMax }, glm::vec2{ _xMin, _yMin },
      glm::vec2{ _xMax, _yMin }, glm::vec2{ _xMax, _yMax });
  }

  Quad2D MakeQuad2D(glm::vec2 _size)
  {
    auto half = _size / 2.f; // convert full size vector to half vector
    return std::make_tuple(
      //  top left, bottom left
      glm::vec2(-half.x, +half.y), glm::vec2(-half.x, -half.y),
      // bottom right, top right
      glm::vec2(+half.x, -half.y), glm::vec2(+half.x, +half.y));
  }

  Quad2D MakeQuad2D(glm::vec2 _center, glm::vec2 _size)
  {
    return MakeQuad2D(_size) + _center;
  }


  Quad3D::Quad3D(Elements const& _elements, glm::vec3 const& _normal)
    : elements(_elements)
    , normal(_normal)
  {
  }

  Quad3D::Quad3D(Quad2D const& _quad2D)
  {
    elements = std::make_tuple(
      glm::vec3(std::get<0>(_quad2D.elements), 0.f),
      glm::vec3(std::get<1>(_quad2D.elements), 0.f),
      glm::vec3(std::get<2>(_quad2D.elements), 0.f),
      glm::vec3(std::get<3>(_quad2D.elements), 0.f));

    normal = glm::vec3(0.f, 0.f, 1.f);
  }

  Quad3D Quad3D::operator+(glm::vec3 const& _offset)
  {
    return Quad3D(std::make_tuple(
      std::get<0>(elements) + _offset, std::get<1>(elements) + _offset,
      std::get<2>(elements) + _offset, std::get<3>(elements) + _offset),
      normal);
  }

  Quad3D Quad3D::operator*(glm::mat3 const& _matrix)
  {
    glm::mat4x3 quad(std::get<0>(elements), std::get<1>(elements),
                     std::get<2>(elements), std::get<3>(elements));
   
    auto result = _matrix * quad;
    return Quad3D(std::make_tuple(result[0], result[1], result[2], result[3]),
                  _matrix * normal);
  }

  Quad3D Quad3D::Flip() const
  {
    auto newNormal = -normal;
    return Quad3D(std::make_tuple(
      std::get<3>(elements), std::get<2>(elements), 
      std::get<1>(elements), std::get<0>(elements)), newNormal);
  }


  Quad3D MakeQuad3D(glm::vec2 _size, glm::vec3 _normal)
  {
    //  convert a quad aligned at origin to use vec3's
    auto quad3D = Quad3D(MakeQuad2D(_size));
    //  rotate the quad to align normal of 2d quad to given normal
    auto angle = glm::acos(glm::dot(quad3D.normal, _normal));
    auto axis = glm::cross(quad3D.normal, _normal);
    if (glm::dot(quad3D.normal, _normal) == -1.f) return quad3D.Flip();
    //  if the input normal is equivalent to our starting normal, just bail
    if (axis == glm::vec3(0.)) return quad3D;
    return quad3D * glm::rotate(glm::mat4(1.f), angle, axis);
  }

  Quad3D MakeQuad3D(glm::vec3 _offset, glm::vec2 _size, glm::vec3 _normal)
  {
    return MakeQuad3D(_size, _normal) + _offset;
  }
}
