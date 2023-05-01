#include "Model.h"

/// <summary>
/// Process each aiNode recursively. Each node has a list of aiNode children.
/// Each node has a list of indices for meshes stored in aiScene
/// </summary>
void Model::processNode(const aiScene* scene, const aiNode* node)
{
	// process this node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(scene, mesh));
	}

	// process the children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(scene, node->mChildren[i]);
	}
}

/// <summary>
/// This methods creates a mesh (vertex attributes, indices, textures)
/// </summary>
std::shared_ptr<Mesh> Model::processMesh(const aiScene* scene, const aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	
	/***************************
	   process vertex attributes
	****************************/

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		// vertex positions
		Vertex v;
		v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		// vertex normals
		if (mesh->HasNormals()) {
			v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}
		// vertex tangents
		if (mesh->HasTangentsAndBitangents()) {
			v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		}
		// vertex tex coords (only first slot)
		if (mesh->HasTextureCoords(0)) {
			v.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		vertices.push_back(v);
	}

	/***************************
	   process indices
	****************************/

	std::vector<unsigned int> indices;
	// process faces(triangles) -> collection of indices to vertices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	/***************************
	   process textures
	****************************/
	std::vector<std::shared_ptr<Texture> > textures;
	std::string assetDirectory = m_path.substr(0, m_path.find_last_of('/') + 1);
	// each mesh has 1 material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


	// 1 material has all textures used
	const unsigned int TYPE_COUNT = 5;
	struct {
		aiTextureType assimpType;
		Texture::Type type;
	} texTypes[TYPE_COUNT] = {
		{aiTextureType_DIFFUSE, Texture::Type::DIFFUSE},
		{aiTextureType_SPECULAR, Texture::Type::SPECULAR},
		{aiTextureType_NORMALS, Texture::Type::NORMAL},
		{aiTextureType_METALNESS, Texture::Type::METALLIC},
		{aiTextureType_DIFFUSE_ROUGHNESS, Texture::Type::ROUGHNESS }
	};

	// iterate through all texture types used
	for (unsigned int i = 0; i < TYPE_COUNT; ++i) {
		auto assimpType = texTypes[i].assimpType;
		auto type = texTypes[i].type;
		// get all textures for each type (currently supporting only 1 texture per type)
		// TODO: support loading multiple textures of same type
		for (unsigned int j = 0; j < material->GetTextureCount(assimpType); ++j) {
			aiString path;
			material->GetTexture(assimpType, j, &path);

			// load texture with textureManager (handles caching)
			textures.push_back(
				TextureManager::get().getTexture(assetDirectory + path.C_Str(), type)
			);
		}

	}
	// if no textures are loaded assume we have to load manually
	if (textures.size() == 0) {
		for (unsigned int i = 0; i < TYPE_COUNT; ++i) {
			std::string file;
			switch (texTypes[i].assimpType)
			{
				/*case aiTextureType_DIFFUSE: file = "diffuse.png"; break;
				case aiTextureType_SPECULAR: file = "specular.png"; break;*/
				case aiTextureType_NORMALS: file = "normal.png"; break;
				/*case aiTextureType_METALNESS: file = "metallic.png"; break;
				case aiTextureType_DIFFUSE_ROUGHNESS: file = "roughness.png"; break;*/
			}

			// load texture with textureManager (handles caching)
			try {
				textures.push_back(
					TextureManager::get().getTexture(assetDirectory + file.c_str(), texTypes[i].type)
				);
			}
			catch (std::exception& e) {
				printf("[ERROR] %s\n", e.what());
			}
		}
	}
	return std::make_shared<Mesh>(vertices, indices, textures);
}

void Model::load(const std::string& path)
{
	m_path = path;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FlipUVs
	);

	if (scene == nullptr) {
		printf("Assimp import error: %s", importer.GetErrorString());
		exit(0);
	}

	processNode(scene, scene->mRootNode);
}

void Model::draw(Shader& shader) const
{
	for (auto mesh : m_meshes) {
		mesh->draw(shader);
	}
}
