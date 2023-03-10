#pragma once
#include <string>
#include "stb_image.h"
#include "GL/glew.h"

class Texture
{
private:
	unsigned int m_id = 0;
	int m_width = 0;
	int m_height = 0;
	int m_nrChannels = 0;
	
public:
	Texture() = default;
	Texture(const std::string& path, bool flipY = true);
	
	// delete copy constructor
	Texture(const Texture& o) = delete;
	
	~Texture();

	/// <summary>
	/// Create the texture if not created already.
	/// </summary>
	void create();

	/// <summary>
	/// Load texture from file (using stb_image.h).
	/// </summary>
	void loadFromFile(const std::string& filepath, bool flipY = true);
	
	/// <summary>
	/// Activate the texture slot and bind this texture.
	/// </summary>
	void bind(unsigned int slot) const;
	
	/// <summary>
	/// Activate the texture slot and unbind this texture (bind 0).
	/// </summary>
	void unbind(unsigned int slot) const;
};