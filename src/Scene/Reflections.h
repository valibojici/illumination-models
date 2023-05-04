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

class Reflections : public Scene
{
private:
	Framebuffer m_hdrFBO;
	ScreenQuadRenderer m_screenQuadRenderer;
	
	std::unique_ptr<Mesh> m_mesh;
	
	// which lighting model is used
	int m_modelIndex = 0;

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	std::vector<Shader> m_shaders;
	Shader m_postprocessShader;
	Shader m_skyboxShader;

	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	PostprocessUI m_postProcessUI;

	std::vector<std::unique_ptr<Material>> m_materials;

	// enable/disable wireframes, for debug
	bool m_wireframeEnabled = false;

	std::shared_ptr<Texture> m_skyboxTexture;
public:
	Reflections(Scene*& scene);
	~Reflections();
	void onRender() override;
	void onRenderImGui() override;
};
