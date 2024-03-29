#pragma once
#include <string>
#include "stb_image.h"
#include "GL/glew.h"
#include <stdexcept>
#include <sstream>

/// <summary>
/// Class to load/store textures
/// </summary>
class Texture
{

public:
	enum class Type {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		ROUGHNESS,
		METALLIC,
		EMISSIVE,
		OPACITY,
		CUBEMAP
	};

	Texture() = default;
	Texture(const std::string& path, Type type = Type::DIFFUSE, bool flipY = true);
	
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
	/// Load cubemap texture from file (using stb_image.h).
	/// </summary>
	void loadCubemapFromFile(const std::string& directory, bool flipY = true);
	
	/// <summary>
	/// Activate the texture slot and bind this texture.
	/// </summary>
	void bind(unsigned int slot) const;
	
	/// <summary>
	/// Activate the texture slot and unbind this texture (bind 0).
	/// </summary>
	void unbind(unsigned int slot) const;

	inline Type getType() const { return m_type; }
	inline unsigned int getId() const { return m_id; }
private:
	unsigned int m_id = 0;
	int m_width = 0;
	int m_height = 0;
	int m_nrChannels = 0;
	std::string m_name;
	Type m_type = Type::DIFFUSE;
};