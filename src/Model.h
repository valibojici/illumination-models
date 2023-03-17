#pragma once
#include <vector>
#include <Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"
#include "TextureManager.h"

class Model
{
private:
	std::string m_path;
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	void processNode(const aiScene* scene, const aiNode* node);
	std::shared_ptr<Mesh> processMesh(const aiScene* scene, const aiMesh* mesh);
public:
	void load(const std::string& path);
	void draw(Shader& shader) const;
};

