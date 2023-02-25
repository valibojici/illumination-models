#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "SceneMenu.h"

class App
{
public:
	void run();
	App();
private:
	Renderer m_renderer;
	// TODO: maybe use smart pointers
	Scene* m_scene = new SceneMenu(m_scene);
};

