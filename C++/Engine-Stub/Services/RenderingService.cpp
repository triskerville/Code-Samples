// file:    RenderingService.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>
#include <Services/RenderingService.h>
#include <Services/MessageService.h>
#include <Services/WindowService.h>
#include <Services/GUIService.h>

#include <Core/EResult.h>
#include <Core/InputEvent.h>

#include <Graphics/Common.h>
#include <Graphics/VertexArray.h>
#include <Graphics/Buffer.h>
#include <Graphics/Framebuffer.h>
#include <Graphics/Texture/Texture2D.h>
#include <Graphics/Shader.h>
#include <Graphics/Mesh.h>
#include <Graphics/ObjectData.h>
#include <Graphics/Texture/Cubemap.h>

#include <Primitive/Quad.h>

#include <World/Entity.h>
#include <World/Camera.h>
#include <World/LocalLight.h>

namespace tbasque
{
  static void onCameraUpdate(MessageService::Observer, Hash, void*);

  RenderingService::RenderingService(DependencyList&& _dependencies)
    : DependencyList(std::forward<DependencyList>(_dependencies))
  {
    pRenderer_ = Graphics::Platform::CreateRenderer();

    auto pWindowService = this->GetService<WindowService>();

    SetViewport(pWindowService->GetSize());
    pWindowService->ResizeDelegate = std::function(
      [this](glm::ivec2 _size)
      {
        SetViewport(_size);
      });

    quad3DShader_ = std::make_shared<Graphics::Shader>(
      Graphics::Shader
      {
        "./Input/Shaders/points.vert",
        "./Input/Shaders/points.frag"
      }
    );
    gBufferShader_ = std::make_shared<Graphics::Shader>(
      Graphics::Shader
      {
        "./Input/Shaders/gbuffer.vert",
        "./Input/Shaders/gbuffer.frag"
      }
    );

    auto pMessageService = GetService<MessageService>();
    pMessageService->Register(this, Hash("CameraUpdate"), onCameraUpdate);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  }

  void RenderingService::OnCameraUpdate(World::Camera const& _camera)
  {
    cameraMatrices_ =
    {
      _camera.GetProjection(), _camera.GetView(),
      { _camera.GetEye(), 1.f }, { _camera.GetForward(), 0.f },
      { _camera.NearPlane, _camera.FarPlane }
    };
    
    auto ubo = pRenderer_->GetUniformBuffer("CameraMatrices");
    ubo->SetData(&cameraMatrices_, 1);
  }

  void RenderingService::FrameBegin(float)
  {
    //  just make sure this gets emptied every frame
    for (auto& [name, pFramebuffer] : framebufferMap_)
      pFramebuffer->Clear();
  }

  void RenderingService::FrameEnd()
  {
    auto fboDeferred = framebufferMap_.at("gbuffer");
    //  deferred rendering pass for world entities
    fboDeferred->Bind();
    while (!renderList_.empty())
    {
      RenderEntity(renderList_.front());
      renderList_.pop_front();
    }
    fboDeferred->Unbind();

    auto& textures = fboDeferred->GetTextures();
    auto fboScene = framebufferMap_.at("scene");
    fboScene->Bind();
    RenderFSQ(textures);
    fboScene->CopyDepth(fboDeferred);
    RenderLocalLights(textures);
    RenderSkybox();
    fboScene->Unbind();

    //  TODO (Tristan): Move GUI stuff outside
    {
      auto gbuffer = Editor::Window::Builder{ "G-Buffer" }
        .WithFlag(Editor::WindowFlags::NoScroll)
        .Build();

      if (gbuffer.Bind())
      {
        for (auto& texture : textures)
        {
          gbuffer.Image(texture);
        }
      }
      gbuffer.Unbind();

      auto sceneWindow = Editor::Window::Builder{ "Scene" }
        .WithFlag(Editor::WindowFlags::AutoResize)
        .WithFlag(Editor::WindowFlags::NoScroll)
        .Build();

      if (sceneWindow.Bind())
      {
        using namespace Editor;
        sceneWindow.Image(fboScene->GetTextures()[0]);

        sceneWinCache_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectSize() };
        isMouseHoveringScene_ = ImGui::IsItemHovered() ? TRUE : FALSE;
      }
      sceneWindow.Unbind();
    }
  }

  //  we are going to store the pointer and render this later
  void RenderingService::PushEntity(SPointer<const World::Entity> _pEntity)
  {
    renderList_.push_back(_pEntity);
  }

  void RenderingService::PushSkybox(
    SPointer<const World::Entity> _pEntity,
    SPointer<const Graphics::Cubemap> _pCubemap)
  {
    skybox_ = std::make_pair(_pEntity, _pCubemap);
  }

  void RenderingService::PushLocalLight(
    SPointer<const World::Entity> _pEntity, 
    SPointer<const World::LocalLight> _pLocalLight)
  {
    localLights_.push_back(std::make_pair(_pEntity, _pLocalLight));
  }

  void RenderingService::RenderQuads(
    std::vector<Primitive::Quad3D> const& _quads)
  {
    framebufferMap_.at("gbuffer")->Bind();
    quad3DShader_->Bind();

    pRenderer_->RenderToWorld(_quads);

    quad3DShader_->Unbind();
    framebufferMap_.at("gbuffer")->Unbind();
  }

  SceneWindow RenderingService::GetSceneWindowCache() const
  {
    return sceneWinCache_;
  }

  bool RenderingService::IsMouseHoveringScene() const
  {
    return isMouseHoveringScene_;
  }

  auto RenderingService::GetCameraMatrices() const -> CameraMatrices const&
  {
    return cameraMatrices_;
  }

  void RenderingService::RenderEntity(SPointer<const World::Entity> _pEntity)
  {
    _pEntity->pShader_->Bind();
    _pEntity->pShader_->SetUniform(_pEntity->transform_.GetMatrix(), "model");

    pRenderer_->RenderToWorld(_pEntity->pMesh_);

    _pEntity->pShader_->Unbind();
  }

  void RenderingService::RenderLocalLights(
    std::vector<Graphics::Texture2D> const& _textures)
  {
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    while (!localLights_.empty())
    {
      SPointer<const World::Entity> pEntity;
      SPointer<const World::LocalLight> pLocalLight;
      std::tie(pEntity, pLocalLight) = localLights_.front();

      auto shader = pEntity->pShader_;
      //  bind all textures aside from the depth texture
      for (uint i = 0; i < _textures.size() - 1; ++i)
        shader->SetUniform(i, _textures[i]);

      shader->SetUniform(viewportSize_, "viewport");
      shader->SetUniform(pLocalLight->color, "color");
      shader->SetUniform(pEntity->transform_.GetTranslation(), "lightPosition");
      shader->SetUniform(pEntity->transform_.GetScale().x/2.f, "lightRadius");
      shader->SetUniform(pLocalLight->brightness, "lightBrightness");
      RenderEntity(pEntity);
      localLights_.pop_front();
    }
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

  void RenderingService::RenderSkybox()
  {
    glDepthMask(GL_FALSE);
    auto entity = skybox_.first;
    auto cubemap = skybox_.second;
    cubemap->Bind();
    RenderEntity(entity);
    glDepthMask(GL_TRUE);
  }

  void RenderingService::RenderFSQ(
    std::vector<Graphics::Texture2D> const& _textures)
  {
    gBufferShader_->Bind();
    for (uint i = 0; i < _textures.size() - 1; ++i)
      gBufferShader_->SetUniform(i, _textures[i]);

    //  crate a quad in NDC space that maps to all four corners
    auto viewport = Primitive::MakeQuad2D(glm::vec2(0.f), glm::vec2(2.f));
    pRenderer_->RenderToScreen(viewport);

    gBufferShader_->Unbind();
  }

  void RenderingService::ReallocateFramebuffers()
  {
    //  all framebuffers need to be reallocated, so we'll just clear the map
    framebufferMap_.clear();
    framebufferMap_.emplace("gbuffer", std::make_shared<Graphics::Framebuffer>(
      Graphics::Framebuffer::Builder{ viewportSize_ }
        .WithColorBuffer(Graphics::Common::Layout::RGBA_Float, "gPosition")
        .WithColorBuffer(Graphics::Common::Layout::RGBA_Float, "gNormal")
        .WithColorBuffer(Graphics::Common::Layout::RGBA_Float, "gAlbedoSpec")
        .Build())
    );

    framebufferMap_.emplace("scene", std::make_shared<Graphics::Framebuffer>(
      Graphics::Framebuffer::Builder{ viewportSize_ }
        .WithColorBuffer(Graphics::Common::Layout::RGBA_Float, "gScene")
        .Build())
    );
  }

  void RenderingService::SetViewport(glm::ivec2 const& _size)
  {
    viewportSize_ = _size;
    pRenderer_->SetViewport(_size.x, _size.y);
    //  OpenGL is a lil busted, so for now, just reallocate the FBO
    ReallocateFramebuffers();
  }

  void onCameraUpdate(
    MessageService::Observer _observer, Hash _hash, void* _payload)
  {
    RenderingService* pRenderingService = (RenderingService*)_observer;
    World::Camera* pCamera = (World::Camera*)_payload;
    pRenderingService->OnCameraUpdate(*pCamera);
  }
}
