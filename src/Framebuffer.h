#pragma once
#include "Texture.h"
#include <memory>

/// <summary>
/// Framebuffer class that sets up attachments and binds them
/// </summary>
class Framebuffer
{
private:
	unsigned int m_id = 0;
	unsigned int m_width;
	unsigned int m_height;
	// texture for color attachement
	std::unique_ptr<Texture> m_colorTexture = std::make_unique<Texture>();

	// renderbuffer id for depth renderbuffer
	unsigned int m_depthRbo;
public:
	Framebuffer(unsigned int w, unsigned int h, unsigned int interalColorFormat = GL_RGB);
	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }
	inline void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	~Framebuffer();
	const Texture* getColorAttachment() const { return m_colorTexture.get(); }
};

