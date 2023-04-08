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
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

class PhongTest : public Scene
{
private:
	std::unique_ptr<Mesh> m_mesh = std::unique_ptr<Mesh>(Mesh::getSphere(1.0f, 50));

	// mesh for floor, walls, ceiling
	std::unique_ptr<Mesh> m_wall = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 4.0f));

	// rotations and traslations for each wall: left, front, right, top, bottom
	std::vector<glm::mat4> m_transforms{
		glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, 0.0f, -2.0f)),
		glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, 2.0f, 0.0f))* glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, -2.0f, 0.0f))* glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	};
	// materials for each wall (color, diffuse, etc)
	std::vector<std::unique_ptr<PhongMaterial>> m_wallMaterials;
	// material for main mesh
	std::unique_ptr<PhongMaterial> m_material = std::make_unique<PhongMaterial>();

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	Shader m_shader;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	// flag to enable/disable gamma correction
	bool m_gammaCorrection = true;
public:
	PhongTest(Scene*& scene);
	~PhongTest();
	void onRender() override;
	void onRenderImGui() override;
};