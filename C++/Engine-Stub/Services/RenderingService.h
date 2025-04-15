// file:    RenderingService.h
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#pragma once

#include <triskerville_fwd.h>
#include <Interfaces/IService.h>
#include <Interfaces/IDependencyList.h>
#include <Graphics/Platform/Renderer.h>

namespace triskerville
{
  struct SceneWindow
  {
    glm::vec2  Pos{ 0.f };
    glm::vec2 Size{ 0.f };
  };

  class RenderingService :
    public IService,
    //  Attempting to construct a RenderingService instance before any of its
    //  dependencies emits a compiler error.
    public IDependencyList<MessageService, WindowService, GUIService>
  {
  public:
    //  The GPU needs this struct to be 16-byte aligned or it gets read incorrectly
    struct alignas(16u) CameraMatrices
    {
      glm::mat4 proj, view;
      glm::vec4 viewPos, viewDir;
      glm::vec2 nearFarPlanes;
    };

    RenderingService(DependencyList&&);

    void OnCameraUpdate(World::Camera const&);

    void PushEntity(SPointer<const World::Entity>);
    void PushSkybox(SPointer<const World::Entity>, 
                    SPointer<const Graphics::Cubemap>);
    void PushLocalLight(SPointer<const World::Entity>, 
                        SPointer<const World::LocalLight>);

    void RenderQuads(std::vector<Primitive::Quad3D> const&);

    SceneWindow GetSceneWindowCache() const;
    bool IsMouseHoveringScene() const;
    CameraMatrices const& GetCameraMatrices() const;

  private:
    void RenderEntity(SPointer<const World::Entity>);
    void RenderLocalLights(std::vector<Graphics::Texture2D> const&);
    void RenderSkybox();
    void RenderFSQ(std::vector<Graphics::Texture2D> const&);

    //  helper methods for common functionality
    void ReallocateFramebuffers();
    void SetViewport(glm::ivec2 const&);

    //  Application manages these calls, no public access
    void FrameBegin(float) override;
    void FrameEnd() override;

    UPointer<Graphics::Platform::Renderer> pRenderer_{ nullptr };
    std::unordered_map<
      std::string, SPointer<Graphics::Framebuffer>> framebufferMap_{ };

    glm::ivec2 viewportSize_{};

    SPointer<Graphics::Shader> quad3DShader_{ nullptr };
    SPointer<Graphics::Shader> gBufferShader_{ nullptr };

    //  TODO: clean this
    std::pair<SPointer<const World::Entity>,
      SPointer<const Graphics::Cubemap>> skybox_{ nullptr, nullptr };
    std::list<std::pair<SPointer<const World::Entity>,
      SPointer<const World::LocalLight>>> localLights_{};
    std::list<SPointer<const World::Entity>> renderList_;
    
    bool isMouseHoveringScene_{ false };
    SceneWindow sceneWinCache_;
    CameraMatrices cameraMatrices_;
  };
}
