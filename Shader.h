//>=------------------------------------------------------------------------=<//
// file:    Shader.h
// author:  Tristan Baskerville
// course:  CS350
// brief:   
//   This file contains the declaration for the Shader class.
//
// Copyright © 2021 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//
#ifndef SHADER_H
#define SHADER_H

#include <tbasque_fwd.h>
#include <Interfaces/IRenderable.h>

namespace tbasque::Graphics
{
  class Shader : public virtual IRenderable, public virtual IBindable
  {
  public:
    Shader(std::initializer_list<std::string>);
    //  for some reason, marking this as default does not work
    Shader(Shader&&) noexcept;
    ~Shader();

    void Bind() const override;
    void Unbind() const override;

    void SetUniform(int, std::string const&);
    void SetUniform(unsigned, std::string const&);
    void SetUniform(float, std::string const&);

    void SetUniform(glm::vec2, std::string const&);
    void SetUniform(glm::vec3, std::string const&);
    void SetUniform(glm::mat4 const&, std::string const&);

    void SetUniform(int, Texture2D const&);
  private:
    enum class Context { Compile, Link };
    ///////////////////////////////////////////
    // Static Helper functions and variables //
    ///////////////////////////////////////////
    static constexpr unsigned invalid_program_{ static_cast<uint>(-1) };
    static constexpr      int log_size_{ 1024 };
    static constexpr      int invalid_type_{ -1 };
    static const std::unordered_map<std::string, int> extension_map_;
   
    static unsigned compileShader(std::string, int);
    static void printErrors(uint, Context);
    static std::string extractFileContents(const std::string&);
    static int getShaderType(const std::string&);
    static void linkProgram(uint);

    int GetUniformLocation(std::string const&);

    //////////////////////
    // Member variables //
    //////////////////////
    std::vector<std::string> filepaths_{};
    std::unordered_map<std::string, int> uniformLocationMap_{};
  };
}

#endif
