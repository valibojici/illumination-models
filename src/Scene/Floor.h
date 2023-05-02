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
#include "Postprocess/ScreenQuadRenderer.h"
#include "Framebuffer.h"
#include "Postprocess/PostprocessUI.h"

class Floor : public Scene
{
private:
	struct MaterialMesh {
		std::vector<std::unique_ptr<Material>> materials;
		glm::mat4 modelMatrix;
		std::unique_ptr<Mesh> mesh;
		std::string name;
		float textureScaleX = 1.0f;
		float textureScaleY = 1.0f;
		int modelIndex = 0;
	};

	// framebuffer to use lighting with hdr
	Framebuffer m_hdrFBO;

	// helper to render texture to screen
	ScreenQuadRenderer m_screenQuadRenderer;


	std::vector<MaterialMesh> m_meshes;
	//std::unique_ptr<Mesh> m_mesh = std::unique_ptr<Mesh>(Mesh::getPlane(12.0f, 12.0f));

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	std::vector<Shader> m_shaders;
	Shader m_postprocessShader;

	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	// helper for post processing UI
	PostprocessUI m_postProcessUI;

	// enable/disable wireframes, for debug
	bool m_wireframeEnabled = false;
public:
	Floor(Scene*& scene);
	~Floor();
	void onRender() override;
	void onRenderImGui() override;
};

