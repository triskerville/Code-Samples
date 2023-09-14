//>=------------------------------------------------------------------------=<//
// file:    Shader.cpp
// author:  Tristan Baskerville
// course:  CS350
// brief:   
//   This file contains the implementation for the Shader class, which is meant
//   to handle parsing shader files and converting them to OpenGL objects.
//
// Copyright © 2021 DigiPen, All rights reserved.
//>=------------------------------------------------------------------------=<//
#include <stdafx.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture/Texture2D.h>

namespace tbasque::Graphics
{
  //  mappings of supported file extensions to their respective OpenGL values
  const std::unordered_map<std::string, int> Shader::extension_map_
  {
    { "vert", GL_VERTEX_SHADER },
    { "frag", GL_FRAGMENT_SHADER },
    { "geom", GL_GEOMETRY_SHADER },
    { "comp", GL_COMPUTE_SHADER },
    { "tese", GL_TESS_EVALUATION_SHADER },
    { "tesc", GL_TESS_CONTROL_SHADER }
  };

  Shader::Shader(std::initializer_list<std::string> _paths)
    : filepaths_(_paths)
  {
    handle_ = glCreateProgram();

    std::deque<unsigned> shader_ids{};
    auto compile = [&](const std::string& _filename)
    {
      //  start by first converting the file contents into a string,
      //  then determining the type of shader based on the file extension
      unsigned shader = compileShader(
        extractFileContents(_filename),
        getShaderType(_filename)
      );
      //  if any of the above helper functions fail, we cannot use the file,
      //  thus only adding successful shader files
      if (shader != invalid_program_) shader_ids.push_back(shader);
    };
    //  iterate over the container and compile the shaders
    std::for_each(filepaths_.begin(), filepaths_.end(), compile);
    //  iterate and attach shaders
    auto attach_shaders = [&](unsigned sid) { glAttachShader(handle_, sid); };
    std::for_each(shader_ids.begin(), shader_ids.end(), attach_shaders);
    //  this attempts to link all the shader programs and checks for any errors
    linkProgram(handle_);
    //  iterate over shaders and delete each
    auto delete_shaders = [](unsigned sid) { glDeleteShader(sid); };
    std::for_each(shader_ids.begin(), shader_ids.end(), delete_shaders);
  }

  Shader::Shader(Shader&& _other) noexcept
    : IRenderable(std::move(_other))
    , filepaths_(std::move(_other.filepaths_))
  {
  }

  Shader::~Shader()
  {
    glDeleteProgram(handle_);
    handle_ = 0u;
  }

  void Shader::Bind() const
  {
    glUseProgram(handle_);
  }

  void Shader::Unbind() const
  {
    //  technically this function can be static (or even completely omitted)
    glUseProgram(0);
  }

  void Shader::SetUniform(int _value, std::string const& _name)
  {
    glProgramUniform1i(handle_, GetUniformLocation(_name), _value);
  }

  void Shader::SetUniform(unsigned _value, std::string const& _name)
  {
    glProgramUniform1ui(handle_, GetUniformLocation(_name), _value);
  }

  void Shader::SetUniform(float _value, std::string const& _name)
  {
    glProgramUniform1f(handle_, GetUniformLocation(_name), _value);
  }

  void Shader::SetUniform(glm::vec2 _vec, std::string const& _name)
  {
    glProgramUniform2fv(handle_, GetUniformLocation(_name), 1,
                        &_vec[0]);
  }

  void Shader::SetUniform(glm::vec3 _vec, std::string const& _name)
  {
    glProgramUniform3fv(handle_, GetUniformLocation(_name), 1,
                        &_vec[0]);
  }

  void Shader::SetUniform(glm::mat4 const& _uniform, std::string const& _name)
  {
    glProgramUniformMatrix4fv(handle_, GetUniformLocation(_name),
                              1, GL_FALSE, &_uniform[0][0]);
  }

  void Shader::SetUniform(int _index, Texture2D const& _texture)
  {
    SetUniform(_index, _texture.GetName());
    _texture.Bind(_index);
  }

  unsigned Shader::compileShader(std::string _data, int _type)
  {
    //  if the file could not be read, or the extension was not recognized,
    //  we cannot use the shader object
    if (_type == invalid_type_ || _data.empty()) return invalid_program_;

    //  get a handler to shader object
    unsigned id = glCreateShader(_type);
    const char* data_ptr = _data.c_str();
    //  compile the data, shaderSource takes input as 2d arr
    glShaderSource(id, 1, &data_ptr, NULL);
    glCompileShader(id);

    //  verify that no errors were encountered
    printErrors(id, Context::Compile);

    return id;
  }

  void Shader::printErrors(unsigned _id, Context _context)
  {
    int success{ 1 }; // return value from OGL functions
    char log[log_size_]; // log to place debug information on shaders

    //  update the log to describe why it failed
    switch (_context)
    {
      case Context::Compile:
        //  verify a shader was successfully compiled correctly
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        break;
      case Context::Link:
        //  verify a program was linked correctly
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        break;
      default:
      {
        //  idk how you got here fam
        assert(false);
      }
    }

    if (!success)
    {
      glGetProgramInfoLog(_id, log_size_, nullptr, log);
      std::cerr << "Shader failed to compile/link:" << log << std::endl;
    }
  }

  std::string Shader::extractFileContents(const std::string& _path)
  {
    //  file denoted by path contains the shader code we wish to retrieve
    std::ifstream file(_path);

    //  if the file cannot be opened, we don't have any data to extract
    if (file.is_open())
    {
      //  get the entire file and place it into data
      std::string data;
      std::getline(file, data, std::string::traits_type::to_char_type(
        std::string::traits_type::eof()));
      return data;
    }
    else std::cerr << "Could not open file: " << _path
      << ". Might not exist." << std::endl;

    //  file path was likely incorrect, or the file did not exist
    return { "" };
  }

  int Shader::getShaderType(const std::string& _filename)
  {
    //  matches specific file extensions to know what was passed in w/o the
    //  need for additional arguments to denote the type of shader
    static const std::regex type_check(R"(\.(vert|frag|geom|comp|tese|tesc)$)",
                                       std::regex_constants::icase);

    int type = invalid_type_; // variable we are returning
    std::smatch sm; // capture match into sm
    if (std::regex_search(_filename, sm, type_check))
      //  set type to the captured file extension. group 0 is entire match,
      //  group 1 is extension w/o the dot
      type = extension_map_.at(sm[1].str());
    else std::cerr << "File extension not supported." << std::endl;

    //  type might be invalid here
    return type;
  }

  void Shader::linkProgram(unsigned _id)
  {
    glLinkProgram(_id);
    printErrors(_id, Context::Link);
  }

  int Shader::GetUniformLocation(std::string const& _uniformName)
  {
    //  if the uniform name does not exist in our map, emplace it first
    if (uniformLocationMap_.find(_uniformName) == uniformLocationMap_.end())
    {
      int location = glGetUniformLocation(handle_, _uniformName.c_str());
      uniformLocationMap_.emplace(_uniformName, location);
    }

    return uniformLocationMap_.at(_uniformName);
  }
}
