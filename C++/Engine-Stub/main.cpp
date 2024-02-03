// file:    main.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2021 DigiPen, All rights reserved.

#include <stdafx.h>

#include <Application/Application.h>
#include <Services/MessageService.h>
#include <Services/WindowService.h>
#include <Services/GUIService.h>
#include <Services/AudioService.h>
#include <Services/RenderingService.h>
#include <Services/SceneLogic.h>

int main(void)
{
  using namespace tbasque;
  //  build our application with our desired setup
  Application app = Application::Builder{}
    .InstallService<MessageService>()
    .InstallService<WindowService>()
    .InstallService<GUIService>()
    .InstallService<AudioService>()
    .InstallService<RenderingService>()
    .InstallService<SceneLogic>()
    .Build();
  //  start running our application. returns at end of application
  app.Run();
}
