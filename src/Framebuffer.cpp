#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int w, unsigned int h, unsigned int interalColorFormat) : m_width(w), m_height(h)
{
	// create and bind
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	// create texture
	m_colorTexture->create();
	m_colorTexture->bind(0);

	glTexImage2D(GL_TEXTURE_2D, 0, interalColorFormat, m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// bind this texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->getId(), 0);
	
	// create depth renderbuffer
	glGenRenderbuffers(1, &m_depthRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	// bind renderbuffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR Framebuffer not complete.");
		exit(0);
	}
	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}
