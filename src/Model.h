#pragma once
#include <vector>
#include <Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"
#include "TextureManager.h"

/// <summary>
/// Class for storing model loaded with assimp.
/// </summary>
class Model
{
private:
	// path to model
	std::string m_path;

	// list of meshes used (each mesh = 1 draw call)
	std::vector<std::unique_ptr<Mesh>> m_meshes;

	/// <summary>
	/// Recursive method to process assimp nodes. 
	/// Calls processMesh on every mesh in a node
	/// </summary>
	/// <param name="scene">Root node which has meshes and textures</param>
	/// <param name="node">Current node</param>
	void processNode(const aiScene* scene, const aiNode* node);

	/// <summary>
	/// Process an assimp mesh and create a mesh using vertices, indices and textures
	/// </summary>
	/// <param name="scene">Root node which has meshes and textures</param>
	/// <param name="mesh">Current mesh</param>
	std::unique_ptr<Mesh> processMesh(const aiScene* scene, const aiMesh* mesh);
public:
	// model matrix for model
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	/// <summary>
	/// Load a model
	/// </summary>
	/// <param name="path">path to file that assimp supports</param>
	void load(const std::string& path);

	/// <summary>
	/// Draw this model (uses the model matrix)
	/// </summary>
	void draw(Shader& shader) const;
};

