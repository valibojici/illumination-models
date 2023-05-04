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
std::unique_ptr<Mesh> Model::processMesh(const aiScene* scene, const aiMesh* mesh)
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
	static std::unordered_map<aiTextureType, Texture::Type> textureTypeMapping;
	textureTypeMapping[aiTextureType_DIFFUSE] = Texture::Type::DIFFUSE;
	textureTypeMapping[aiTextureType_SPECULAR] = Texture::Type::SPECULAR;
	textureTypeMapping[aiTextureType_NORMALS] = Texture::Type::NORMAL;
	textureTypeMapping[aiTextureType_NORMAL_CAMERA] = Texture::Type::NORMAL;
	textureTypeMapping[aiTextureType_METALNESS] = Texture::Type::METALLIC;
	textureTypeMapping[aiTextureType_REFLECTION] = Texture::Type::METALLIC;
	textureTypeMapping[aiTextureType_DIFFUSE_ROUGHNESS] = Texture::Type::ROUGHNESS;
	textureTypeMapping[aiTextureType_SHININESS] = Texture::Type::ROUGHNESS;
	textureTypeMapping[aiTextureType_EMISSIVE] = Texture::Type::EMISSIVE;
	
	// iterate through all texture types used
	for (auto& it : textureTypeMapping) {
		auto assimpType = it.first;
		auto type = it.second;
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
		const aiTextureType types[] = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR,aiTextureType_NORMALS,aiTextureType_METALNESS,aiTextureType_DIFFUSE_ROUGHNESS };
		for (auto& it : textureTypeMapping) {
			std::string file;
			switch (it.first)
			{
				case aiTextureType_DIFFUSE: file = "diffuse.png"; break;
				case aiTextureType_SPECULAR: file = "specular.png"; break;
				case aiTextureType_NORMALS: file = "normal.png"; break;
				case aiTextureType_METALNESS: file = "metallic.png"; break;
				case aiTextureType_DIFFUSE_ROUGHNESS: file = "roughness.png"; break;
				case aiTextureType_EMISSIVE: file = "emissive.png"; break;
				default: continue;
			}

			// load texture with textureManager (handles caching)
			try {
				textures.push_back(
					TextureManager::get().getTexture(assetDirectory + file.c_str(), it.second)
				);
			}
			catch (std::exception& e) {
				printf("[ERROR] %s\n", e.what());
			}
		}
	}
	return std::make_unique<Mesh>(vertices, indices, textures);
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
	for (auto& mesh : m_meshes) {
		mesh->draw(shader);
	}
}
