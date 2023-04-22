#include "Framebuffer.h"

//Framebuffer::Framebuffer(unsigned int w, unsigned int h, unsigned int interalColorFormat) : m_width(w), m_height(h)
//{
//	// create and bind
//	glGenFramebuffers(1, &m_id);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
//	// create texture
//	m_colorTexture->create();
//	m_colorTexture->bind(0);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, interalColorFormat, m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// bind this texture to framebuffer
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->getId(), 0);
//	
//	// create depth renderbuffer
//	glGenRenderbuffers(1, &m_depthRbo);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
//	// bind renderbuffer to framebuffer
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//		printf("ERROR Framebuffer not complete.");
//		exit(0);
//	}
//	// unbind framebuffer
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

void Framebuffer::addColorAttachament(bool isTexture, unsigned int internalFormat)
{
	Attachment colorAttachment;
	colorAttachment.isTexture = isTexture;
	if (isTexture) {
		// create and bind texture id
		glGenTextures(1, &colorAttachment.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorAttachment.id);
		// create texture and parameters
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		// TODO
	}
	m_colorAttachments.push_back(colorAttachment);
}

void Framebuffer::addDepthAttachment(bool isTexture, unsigned int internalFormat)
{
	m_depthAttachment.isTexture = isTexture;
	if (isTexture) {
		// create and bind texture id
		glGenTextures(1, &m_depthAttachment.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment.id);
		// create texture and parameters
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		// create renderbuffer
		glGenRenderbuffers(1, &m_depthAttachment.id);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachment.id);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);
		// bind renderbuffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachment.id);
	}
}

void Framebuffer::create()
{
	if (m_created) {
		printf("Error: Framebuffer already created\n");
		exit(0);
	}
	m_created = true;

	//create and bind
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	if (m_colorAttachments.size() == 0) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	for (size_t i = 0; i < m_colorAttachments.size(); ++i) {
		if (m_colorAttachments[i].isTexture) {
			// activate slot and bind this texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i].id);
			// link texture to fbo
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorAttachments[i].id, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else {
			// bind renderbuffer
			glBindRenderbuffer(GL_RENDERBUFFER, m_colorAttachments[i].id);
			// link renderbuffer to fbo
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, m_colorAttachments[i].id);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}
	
	if (m_depthAttachment.isTexture) {
		// activate slot and bind this texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment.id);
		// link texture to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment.id, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		// bind renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachment.id);
		// link renderbuffer to fbo
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachment.id);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR Framebuffer not complete.");
		exit(0);
	}
	// bind default
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

Framebuffer::~Framebuffer()
{
	// delete color attachments
	for (auto& colorAttachment : m_colorAttachments) {
		if (colorAttachment.isTexture) {
			glDeleteTextures(1, &colorAttachment.id);
		}
		else {
			glDeleteRenderbuffers(1, &colorAttachment.id);
		}
	}

	// delete depth attachments
	if (m_depthAttachment.isTexture) {
		glDeleteTextures(1, &m_depthAttachment.id);
	}
	else {
		glDeleteRenderbuffers(1, &m_depthAttachment.id);
	}
	glDeleteFramebuffers(1, &m_id);
}
