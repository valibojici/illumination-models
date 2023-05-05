#include "Framebuffer.h"

void Framebuffer::addColorAttachament(unsigned int type, unsigned int internalFormat)
{
	Attachment colorAttachment;
	colorAttachment.type = type;
	if (type == GL_TEXTURE_2D) {
		// create and bind texture id
		glGenTextures(1, &colorAttachment.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorAttachment.id);
		// create texture and parameters
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		// TODO
	}
	m_colorAttachments.push_back(colorAttachment);
}

unsigned int Framebuffer::addDepthAttachment(unsigned int type, unsigned int internalFormat)
{
	Attachment depthAttachment;
	depthAttachment.type = type;
	if (type == GL_TEXTURE_2D || type == GL_TEXTURE_CUBE_MAP) {
		// create and bind texture id
		glGenTextures(1, &depthAttachment.id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(type, depthAttachment.id);
		// create texture and parameters
		if (type == GL_TEXTURE_2D) {
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
		else {
			// allocate space for each face of the cubemap
			for (int i = 0; i < 6; ++i) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			}
		}
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (type == GL_TEXTURE_CUBE_MAP) {
			glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		// unbind
		glBindTexture(type, 0);
	}
	else {
		// create renderbuffer
		glGenRenderbuffers(1, &depthAttachment.id);
		glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment.id);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);
		// bind renderbuffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAttachment.id);
	}
	m_depthAttachments.push_back(depthAttachment);
	return depthAttachment.id;
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

	std::vector<GLenum> buffers;
	for (size_t i = 0; i < m_colorAttachments.size(); ++i) {
		buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		if (m_colorAttachments[i].type == GL_TEXTURE_2D) {
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
	glDrawBuffers(m_colorAttachments.size(), buffers.data());
	
	activateDepthAttachment(0, m_depthAttachments[0].type == GL_TEXTURE_2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	
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
		if (colorAttachment.type != GL_RENDERBUFFER) {
			glDeleteTextures(1, &colorAttachment.id);
		}
		else {
			glDeleteRenderbuffers(1, &colorAttachment.id);
		}
	}

	// delete depth attachments
	for (auto& depthAttachments : m_depthAttachments) {
		if (depthAttachments.type != GL_RENDERBUFFER) {
			glDeleteTextures(1, &depthAttachments.id);
		}
		else {
			glDeleteRenderbuffers(1, &depthAttachments.id);
		}
	}
	glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::activateDepthAttachment(int slot, unsigned int target)
{
	if (slot >= m_depthAttachments.size()) return;

	if (m_depthAttachments[slot].type == GL_TEXTURE_2D || m_depthAttachments[slot].type == GL_TEXTURE_CUBE_MAP) {
		// activate slot and bind this texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(m_depthAttachments[slot].type, m_depthAttachments[slot].id);
		// link texture to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, m_depthAttachments[slot].id, 0);
		glBindTexture(m_depthAttachments[slot].type, 0);
	}
	else {
		// bind renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachments[slot].id);
		// link renderbuffer to fbo
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachments[slot].id);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}
