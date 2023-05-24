#pragma once
#include <Texture.h>
#include <unordered_map>
#include <memory>

/// <summary>
/// Singleton class for loading and managing textures.
/// It is responsible for creating and managing the lifetime of textures
/// </summary>
class TextureManager
{
private:
	// make constructors private 
	TextureManager() = default;
	TextureManager(const TextureManager& o) = default ;
	TextureManager& operator=(const TextureManager& o) = default;

	// store weak pointers for each texture name, check if they are null and load texture
	std::unordered_map<std::string, std::weak_ptr<Texture> > m_textures;
public: 
	static TextureManager& get();
	/// <summary>
	/// Get a texture, load it if it has not been loaded yet
	/// </summary>
	std::shared_ptr<Texture> getTexture(const std::string& path, Texture::Type type=Texture::Type::DIFFUSE, bool flipY = true);
};

