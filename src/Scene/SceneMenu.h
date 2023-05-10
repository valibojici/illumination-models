#pragma once
#include "Scene.h"
#include "ModelTestScene.h"
#include "Box.h"
#include "Floor.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(Scene*& scene, unsigned int width, unsigned int height) : Scene(scene, width, height) {}
	void onRenderImGui() override;
	void onRender() override;
};

