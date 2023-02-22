#include "VBO.h"

VBO::VBO(void* data, unsigned int size)
{
	glGenBuffers(1, &m_id);
	bind();
	bufferData(data, size);
}

void VBO::create()
{
	if (m_id == 0) {
		glGenBuffers(1, &m_id);
	}
}

void VBO::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBO::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::bufferData(void *data, unsigned int size) const
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
