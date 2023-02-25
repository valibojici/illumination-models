#include "VBO.h"

unsigned int VBO::s_currentBoundVBO = 0;

VBO::VBO(void* data, unsigned int size)
{
	glGenBuffers(1, &m_id); // generate new VBO
	bind();					// bind this VBO
	bufferData(data, size); // buffer data to VBO
}

VBO::VBO(const std::vector<Vertex> vertices) 
	: VBO((void*)vertices.data(), vertices.size() * sizeof(Vertex))
{}

VBO::~VBO()
{
	if (m_id == s_currentBoundVBO) {
		s_currentBoundVBO = 0;
	}
	glDeleteBuffers(1, &m_id);
}

void VBO::create()
{
	if (m_id == 0) {
		glGenBuffers(1, &m_id);
	}
}

void VBO::bind() const
{
	if (m_id != s_currentBoundVBO) {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		s_currentBoundVBO = m_id;
	}
}

void VBO::unbind() const
{
	if (s_currentBoundVBO != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		s_currentBoundVBO = 0;
	}
}

void VBO::bufferData(void *data, unsigned int size)
{
	create();
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
