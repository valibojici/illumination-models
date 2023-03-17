#pragma once
#include "Scene.h"
#include "SceneMenu.h"
#include "Shader.h"
#include "Buffer/VBO.h"
#include "VAO.h"
#include "Buffer/EBO.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h" // for deltaTime(glfwGetTime) in rendering
#include "Camera.h"
#include "Event/EventManager.h" // for registering camera
#include "Mesh.h"
#include "TextureManager.h"
#include <Model.h>

class ModelTestScene : public Scene
{
private:
	Model m_model;
	Mesh* m_lightMesh = Mesh::getSphere(0.1f);
	Camera m_camera;
	Shader m_shader;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_lightModelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	float m_diffuseFactor = 0.5f;
	float m_specFactor = 1.0f;
	float m_shininess = 32;
	glm::vec3 m_lightPos = { 2.0f, 2.0f, 2.0f };
	float m_lightRotationAngle = 0.0f;

public:
	ModelTestScene(Scene*& scene);
	~ModelTestScene();
	void onRender() override;
	void onRenderImGui() override;
};

