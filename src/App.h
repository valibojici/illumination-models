#pragma once
#include "Renderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneMenu.h"

class App
{
public:
	void run();
	App();
private:
	Renderer m_renderer;
};

