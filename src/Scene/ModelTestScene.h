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
#include "Materials/CookTorranceMaterial.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"
#include "Framebuffer.h"
#include "Postprocess/PostprocessUI.h"
#include "Postprocess/ScreenQuadRenderer.h"
#include "Model.h"

class ModelTestScene : public Scene
{
private:
	struct MaterialMesh {
		glm::mat4 modelMatrix;
		std::unique_ptr<Mesh> mesh;
		std::string name;
		float textureScaleX = 1.0f;
		float textureScaleY = 1.0f;
	};

	Framebuffer m_hdrFBO;
	// framebuffer used to output after postprocessing
	Framebuffer m_outputFBO;
	Framebuffer m_shadowFBO;
	ScreenQuadRenderer m_screenQuadRenderer;
	std::vector<MaterialMesh> m_meshes;
	std::vector<MaterialMesh> m_wallMeshes;

	// mesh for floor, walls, ceiling
	std::unique_ptr<Mesh> m_wall = std::unique_ptr<Mesh>(Mesh::getPlane(4.0f, 4.0f));

	// rotations and traslations for each wall: left, front, right, top, bottom
	std::vector<glm::mat4> m_transforms{
		glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, 0.0f, -2.0f)),
		glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
		glm::translate(glm::vec3(0.0f, -2.0f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	};

	// which lighting model is used
	int m_modelIndex = 0;

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	Shader m_shader;
	Shader m_postprocessShader;
	Shader m_shadowShader;
	Shader m_textureDisplayShader;

	std::vector<glm::mat4> m_modelMatrix;
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	PostprocessUI m_postProcessUI;

	CookTorranceMaterial m_material;

	// enable/disable wireframes, for debug
	bool m_wireframeEnabled = false;
	
	std::vector<Model> m_models;
public:
	ModelTestScene(std::unique_ptr<Scene>& scene, unsigned int width, unsigned int height);
	~ModelTestScene();
	void onRender() override;
	void onRenderImGui() override;
	void updateWidthHeight(unsigned int width, unsigned int height) override;
};
