#pragma once
#include "Scene.h"
#include "ModelTestScene.h"
#include "Box.h"
#include "TextureScene.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(std::unique_ptr<Scene>& scene, unsigned int width, unsigned int height) : Scene(scene, width, height) {}
	void onRenderImGui() override;
	void onRender() override;
};

