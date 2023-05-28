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

class TextureScene : public Scene
{
private:
	struct MaterialMesh {
		std::vector<std::unique_ptr<Material>> materials;
		glm::mat4 modelMatrix;
		std::unique_ptr<Mesh> mesh;
		int currentMesh = 0; // 0 = plane | 1 = sphere
		std::string name; // name of this material mesh
		int textureIndex = 0; // index of texture array to use
		float textureScaleX = 1.0f;
		float textureScaleY = 1.0f;
		int modelIndex = 0; // which lighting model is used
	};

	std::vector<MaterialMesh> m_materialMeshes;

	// framebuffer to use lighting with hdr
	Framebuffer m_hdrFBO;
	// framebuffer used to output after postprocessing
	Framebuffer m_outputFBO;

	// helper to render texture to screen
	ScreenQuadRenderer m_screenQuadRenderer;


	std::vector<std::unique_ptr<Mesh>> m_meshes;

	// all lights in the scene
	std::vector<std::unique_ptr<Light> > m_lights;

	Camera m_camera;
	std::vector<Shader> m_shaders;
	Shader m_postprocessShader;
	Shader m_textureDisplayShader;

	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_projMatrix = glm::mat4(1.0f);

	// helper for post processing UI
	PostprocessUI m_postProcessUI;

	// enable/disable wireframes, for debug
	bool m_wireframeEnabled = false;

	// textures used for meshes
	std::vector<std::vector<std::shared_ptr<Texture>>> m_textures;

	// 
public:
	TextureScene(std::unique_ptr<Scene>& scene, unsigned int width, unsigned int height);
	~TextureScene();
	void onRender() override;
	void onRenderImGui() override;
	void updateWidthHeight(unsigned int width, unsigned int height) override;
};

