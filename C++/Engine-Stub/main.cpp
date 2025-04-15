// file:    main.cpp
// author:  Tristan Baskerville
// brief:   
//
// Copyright © 2024 DigiPen, All rights reserved.

#include <stdafx.h>

#include <Application/Application.h>
#include <Services/MessageService.h>
#include <Services/WindowService.h>
#include <Services/GUIService.h>
#include <Services/RenderingService.h>
#include <Services/SceneLogic.h>

int main(void)
{
  using namespace triskerville;
  //  build our application with our desired setup
  Application app = Application::Builder{}
    .InstallService<MessageService>()
    .InstallService<WindowService>()
    .InstallService<GUIService>()
    .InstallService<RenderingService>()
    .InstallService<SceneLogic>()
    .Build();
  //  start running our application. returns at end of application
  app.Run();
}
