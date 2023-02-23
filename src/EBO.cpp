#include "EBO.h"

unsigned int EBO::s_currentBoundEBO = 0;

EBO::EBO(void* data, unsigned int size)
{
	glGenBuffers(1, &m_id); // create EBO
	bind();					// bind this EBO
	bufferData(data, size); // buffer the indices
}

EBO::EBO(const std::vector<unsigned int>& indices)
	: EBO((void*)indices.data(), indices.size() * sizeof(unsigned int))
{}

EBO::~EBO()
{
	glDeleteBuffers(1, &m_id);
}

void EBO::create()
{
	if (m_id == 0) {
		glGenBuffers(1, &m_id);
	}
}

void EBO::bind() const
{
	if (s_currentBoundEBO != m_id) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);	
		s_currentBoundEBO = m_id;
	}
}

void EBO::unbind() const
{
	if (s_currentBoundEBO != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		s_currentBoundEBO = 0;
	}
}

void EBO::bufferData(void* data, unsigned int size) const
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

