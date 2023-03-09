#pragma once
#include "Scene.h"
#include "TriangleScene.h"
#include "Simple3dScene.h"
#include "MeshTestScene.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(Scene*& scene) : Scene(scene) {}
	void onRenderImGui() override;
	void onRender() override;
};

