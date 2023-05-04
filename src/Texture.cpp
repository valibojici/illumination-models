#include "Texture.h"

Texture::Texture(const std::string& path, Type type, bool flipY)
{
	m_type = type;
	m_name = path;
	create();
	loadFromFile(path, flipY);

	printf("Loaded texture %s with type %d\n", m_name.c_str(), m_type);
}

Texture::~Texture()
{
	if (m_id) {
		glDeleteTextures(1, &m_id);
		printf("Deleted texture %s with type %d\n", m_name.c_str(), m_type);
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
		std::stringstream ss;
		ss << "Failed to load file " << filepath.c_str();
		throw std::runtime_error(ss.str().c_str());
	}

	if (m_id == 0) {
		create();
	}

	// bind this texture
	glBindTexture(GL_TEXTURE_2D, m_id);

	// set data
	GLenum format = GL_RGB;
	GLenum internalFormat = GL_RGB;
	switch (m_nrChannels)
	{
	case 3:
		format = GL_RGB;
		internalFormat = (m_type == Type::DIFFUSE || m_type == Type::EMISSIVE) ? GL_SRGB : GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		internalFormat = (m_type == Type::DIFFUSE || m_type == Type::EMISSIVE) ? GL_SRGB_ALPHA : GL_RGBA;
		break;
	default:
		format = internalFormat = GL_RED;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
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
	glBindTexture(GL_TEXTURE_2D, 0);
}
