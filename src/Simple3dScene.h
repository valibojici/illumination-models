#pragma once
#include "Scene.h"
#include "SceneMenu.h"
#include "Shader.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "glm/gtc/matrix_transform.hpp"

class Simple3dScene : public Scene
{
private:
	glm::vec3 m_color = { 1.0f, 0.0f, 0.0f };

	VBO m_vbo;
	VAO m_vao;
	EBO m_ebo;
	Shader m_shader;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);
	float m_rotationAngle = 0.0f;
	glm::vec3 m_rotationAxis = glm::vec3(1.0f);
public:
	Simple3dScene(Scene*& scene);
	~Simple3dScene();
	void onRender() override;
	void onRenderImGui() override;
};

