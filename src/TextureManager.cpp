#include "TextureManager.h"

TextureManager& TextureManager::get()
{
	static TextureManager instance = TextureManager();
	return instance;
}

std::shared_ptr<Texture> TextureManager::getTexture(
	const std::string& path, 
	Texture::Type type,
	bool flipY)
{
	std::weak_ptr<Texture> storedTexture = m_textures[path];

	// try to access the texture
	std::shared_ptr<Texture> texture = storedTexture.lock();
	if (texture == nullptr) {
		// texture is not loaded
		texture = std::make_shared<Texture>(path, type, flipY);
		m_textures[path] = texture; // store weak_ptr to loaded texture
	}

	return texture;
}
