#pragma once
#include "Scene.h"
#include "SceneMenu.h"
#include "Shader.h"
#include "Buffer/VBO.h"
#include "VAO.h"

class TriangleScene : public Scene
{
private:
	glm::vec3 m_color = { 1.0f, 0.0f, 0.0f };

	VBO m_vbo;
	VAO m_vao;
	Shader m_shader;
public:
	TriangleScene(Scene*& scene);
	~TriangleScene();
	void onRender() override;
	void onRenderImGui() override;
};

