#pragma once
#include "Scene.h"
#include "TriangleScene.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(Scene*& scene) : Scene(scene) {}
	void onRenderImGui() override;
	void onRender() override;
};

