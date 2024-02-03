// file:    Quad.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

namespace tbasque::Primitive
{
  struct Quad2D
  {
    using Elements = std::tuple<glm::vec2, glm::vec2, glm::vec2, glm::vec2>;

    Quad2D() = default;
    Quad2D(Elements const&);
    Quad2D operator+(glm::vec2 const&);

    Elements elements;
  };

  //  Global helper functions
  Quad2D MakeQuad2D(float _xMin, float _xMax, float _yMin, float _yMax);
  Quad2D MakeQuad2D(glm::vec2 _size);
  Quad2D MakeQuad2D(glm::vec2 _center, glm::vec2 _size);

  struct Quad3D
  {
    using Elements = std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3>;

    Quad3D() = default;
    Quad3D(Elements const&, glm::vec3 const&);
    Quad3D(Quad2D const&);

    Quad3D operator+(glm::vec3 const&);
    Quad3D operator*(glm::mat3 const&);
    Quad3D Flip() const;

    Elements elements;
    glm::vec3 normal{};
  };

  Quad3D MakeQuad3D(glm::vec2 _size, glm::vec3 _normal);
  Quad3D MakeQuad3D(glm::vec3 _offset, glm::vec2 _size, glm::vec3 _normal);
}
