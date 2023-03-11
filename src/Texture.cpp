#include "Texture.h"

Texture::Texture(const std::string& path, Type type, bool flipY)
{
	m_type = type;
	create();
	loadFromFile(path, flipY);
}

Texture::~Texture()
{
	if (m_id) {
		glDeleteTextures(1, &m_id);
	}
}

void Texture::create()
{
	glGenTextures(1, &m_id);
}

void Texture::loadFromFile(const std::string& filepath, bool flipY)
{
	if (flipY) {
		stbi_set_flip_vertically_on_load(true);
	}
	else {
		stbi_set_flip_vertically_on_load(false);
	}
	unsigned char* data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_nrChannels, 0);
	
	// check if image is loaded
	if (!data)
	{
		printf("Failed to load %s", filepath.c_str());
		exit(0);
	}

	if (m_id == 0) {
		create();
	}

	// bind this texture
	glBindTexture(GL_TEXTURE_2D, m_id);

	// set data
	GLenum format = GL_RGB;
	if (m_nrChannels == 1)
		format = GL_RED;
	else if (m_nrChannels == 3)
		format = GL_RGB;
	else if (m_nrChannels == 4)
		format = GL_RGBA;

	
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set parameters for wrapping -> repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set parameters for mip mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}
