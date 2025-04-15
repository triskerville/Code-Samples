// author:  Tristan Baskerville
// brief:   Provides forward declares for common engine objects.
//          MUCH faster to include this file than the implementation headers.
//
// Copyright © 2024 DigiPen, All rights reserved.

#pragma once

namespace triskerville
{
  enum class EResult;
  class MessageBus;

  class IService;
  template <class... T> class IDependencyList;

  class MessageService;
  class WindowService;
  class GUIService;
  class RenderingService;
  class SceneLogic;

  namespace Primitive
  {
    struct Quad2D;
    struct Quad3D;
  }

  namespace World
  {
    class Entity;
    class Camera;
  }
}
