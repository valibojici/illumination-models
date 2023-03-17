#pragma once
#include "Scene.h"
#include "TriangleScene.h"
#include "Simple3dScene.h"
#include "MeshTestScene.h"
#include "TextureTestScene.h"
#include "ModelTestScene.h"

class SceneMenu : public Scene
{
public:
	SceneMenu(Scene*& scene) : Scene(scene) {}
	void onRenderImGui() override;
	void onRender() override;
};

