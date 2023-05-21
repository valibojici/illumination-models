#pragma once
#include "Texture.h"
#include <memory>
#include <vector>
#include "GL/glew.h"
#include "stb_image_write.h"
#include <ctime>

/// <summary>
/// Framebuffer class that sets up attachments and binds them
/// </summary>
class Framebuffer
{
public:
	struct Attachment{
		unsigned int type = GL_RENDERBUFFER;
		unsigned int id = 0;
	};

private:
	unsigned int m_id = 0;
	unsigned int m_width;
	unsigned int m_height;

	// flag if fbo is created
	bool m_created = false;

	std::vector<Attachment> m_colorAttachments;
	std::vector<Attachment> m_depthAttachments;
public:
	Framebuffer(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}
	
	/// <summary>
	/// Add a color attachment texture/renderbuffer. Multiple can be added
	/// </summary>
	/// <param name="type">: GL_RENDERBUFFER or GL_TEXTURE_2D</param>
	/// <param name="internalFormat">: default is GL_RGB</param>
	void addColorAttachament(unsigned int type = GL_RENDERBUFFER, unsigned int internalFormat = GL_RGB);
	
	/// <summary>
	/// Add a depth attachment texture/renderbuffer
	/// </summary>
	/// <param name="type">: GL_TEXTURE_2D, GL_RENDERBUFFER or GL_TEXTURE_CUBE_MAP</param>
	/// <param name="internalFormat">: default is GL_DEPTH_COMPONENT</param>
	unsigned int addDepthAttachment(unsigned int type = GL_TEXTURE_2D, unsigned int internalFormat = GL_DEPTH_COMPONENT);
	
	/// <summary>
	/// create framebuffer and link the added attachments
	/// </summary>
	void create();

	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }
	inline void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	~Framebuffer();

	/// <summary>
	/// Get the id of the colorAttachment
	/// </summary>
	unsigned int getColorAttachment(int slot) const { return m_colorAttachments[slot].id; }
	
	/// <summary>
	/// Get the id of the depthAttachment
	/// </summary>
	unsigned int getDepthAttachment(int slot) const { return m_depthAttachments[slot].id; }

	/// <summary>
	/// Link the depthAttachment at position "slot" to the framebuffer
	/// </summary>
	/// <param name="target">: texture target, GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP_direction</param>
	void activateDepthAttachment(int slot, unsigned int target = GL_TEXTURE_2D);

	/// <summary>
	/// Add a depth attachment texture/renderbuffer overwriting an existing one
	/// </summary>
	/// <param name="type">: GL_TEXTURE_2D, GL_RENDERBUFFER or GL_TEXTURE_CUBE_MAP</param>
	/// <param name="internalFormat">: default is GL_DEPTH_COMPONENT</param>
	unsigned int addDepthAttachmentAtSlot(int slot, unsigned int type = GL_TEXTURE_2D, unsigned int internalFormat = GL_DEPTH_COMPONENT);

	/// <summary>
	/// Save the color attachment at a slot to PNG file
	/// The name is the timestamp
	/// </summary>
	void saveColorAttachmentToPNG(int slot);
};

