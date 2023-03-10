#pragma once
#include "Scene.h"
#include "SceneMenu.h"
#include "Shader.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h" // for deltaTime(glfwGetTime) in rendering
#include "Camera.h"
#include "EventManager.h" // for registering camera
#include "Mesh.h"

class TextureTestScene : public Scene
{
private:
	Mesh* m_mesh = Mesh::getPlane(2.0f, 2.0f);
	Camera m_camera;
	Shader m_shader;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);
	Texture* m_texture;


	glm::vec3 m_color = { 1.0f, 0.0f, 0.0f };
	float m_diffuseFactor = 0.5f;
	float m_specFactor = 1.0f;
	float m_shininess = 32;
	glm::vec3 m_specColor = glm::vec3(1.0f);
	glm::vec3 m_lightPos = { 5.0f, 5.0f, 5.0f };
	float m_lightRotationAngle = 0.0f;

public:
	TextureTestScene(Scene*& scene);
	~TextureTestScene();
	void onRender() override;
	void onRenderImGui() override;
};