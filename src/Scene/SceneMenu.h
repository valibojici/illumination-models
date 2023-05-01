#pragma once
#include "Scene.h"
#include "ModelTestScene.h"
#include "Box.h"
#include "Floor.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(Scene*& scene) : Scene(scene) {}
	void onRenderImGui() override;
	void onRender() override;
};

