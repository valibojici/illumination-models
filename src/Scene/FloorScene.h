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
#include "Materials/PhongMaterial.h"
#include "Materials/BlinnMaterial.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

class FloorScene : public Scene
{
private:
	std::unique_ptr<Mesh> m_mesh = std::unique_ptr<Mesh>(Mesh::getPlane(12.0f, 12.0f));

	// which lighting model is used
	int m_modelIndex = 0;

	// material for each model for main mesh
	std::vector<std::unique_ptr<Material>> m_materials;

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	std::vector<Shader> m_shaders;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	// flag to enable/disable gamma correction
	bool m_gammaCorrection = true;
public:
	FloorScene(Scene*& scene);
	~FloorScene();
	void onRender() override;
	void onRenderImGui() override;
};

