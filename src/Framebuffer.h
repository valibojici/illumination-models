#pragma once
#include "Texture.h"
#include <memory>
#include <vector>
#include "GL/glew.h"

/// <summary>
/// Framebuffer class that sets up attachments and binds them
/// </summary>
class Framebuffer
{
public:
	struct Attachment{
		bool isTexture = false;
		unsigned int id = 0;
	};

private:
	unsigned int m_id = 0;
	unsigned int m_width;
	unsigned int m_height;

	// flag if fbo is created
	bool m_created = false;

	std::vector<Attachment> m_colorAttachments;
	Attachment m_depthAttachment;
public:
	Framebuffer(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}
	
	/// <summary>
	/// Add a color attachment texture/renderbuffer. Multiple can be added
	/// </summary>
	/// <param name="isTexture">: true if attachment is 2D texture</param>
	/// <param name="internalFormat">: default is GL_RGB</param>
	void addColorAttachament(bool isTexture = true, unsigned int internalFormat = GL_RGB);
	
	/// <summary>
	/// Add a depth attachment texture/renderbuffer
	/// </summary>
	/// <param name="isTexture">: true if attachment is 2D texture</param>
	/// <param name="internalFormat">: default is GL_DEPTH_COMPONENT</param>
	void addDepthAttachment(bool isTexture = true, unsigned int internalFormat = GL_DEPTH_COMPONENT);
	
	/// <summary>
	/// create framebuffer and link the added attachments
	/// </summary>
	void create();

	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }
	inline void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	~Framebuffer();

	/// <summary>
	/// Get the id of the colorAttachment<slot>
	/// </summary>
	unsigned int getColorAttachment(int slot) const { return m_colorAttachments[slot].id; }
	
	/// <summary>
	/// Get the id of the depthAttachment
	/// </summary>
	unsigned int getDepthAttachment() const { return m_depthAttachment.id; }
};

