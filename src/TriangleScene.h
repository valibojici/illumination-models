#pragma once
#include "Scene.h"
#include "SceneMenu.h"

class TriangleScene : public Scene
{
private:
	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
public:
	TriangleScene(Scene*& scene) : Scene(scene) {}
	void onRender() override;
	void onRenderImGui() override;
};

