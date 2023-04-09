#pragma once
#include <memory>
#include "GL/glew.h"
#include <imgui.h>

class Scene
{
protected:
	Scene*& m_currentScene;
public:
	Scene(Scene*& scene) : m_currentScene(scene) {}
	virtual void onRender() {}
	// TODO: add back button rendering here
	virtual void onRenderImGui() {}
	virtual void onUpdate() {}
	virtual ~Scene() {}
	static void helpPoput(const char* text);
};

