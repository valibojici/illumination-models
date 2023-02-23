#include "VAO.h"

unsigned int VAO::s_currentBoundVAO = 0;

VAO::~VAO()
{
	glDeleteVertexArrays(1, &m_id);
}

void VAO::addLayout(DataType dataType, unsigned int count, unsigned int attribDivisor)
{
	switch (dataType)
	{
	case VAO::DataType::FLOAT:
		m_layout.push_back({ m_stride, count, GL_FLOAT, attribDivisor});
		m_stride += sizeof(float) * count;
		break;
	case VAO::DataType::UNSIGNED_INT:
		m_layout.push_back({ m_stride, count, GL_UNSIGNED_INT, attribDivisor});
		m_stride += sizeof(unsigned int) * count;
		break;
	case VAO::DataType::VEC2:
		m_layout.push_back({ m_stride, 2 * count, GL_FLOAT, attribDivisor});
		m_stride += sizeof(float) * 2 * count;
		break;
	case VAO::DataType::VEC3:
		m_layout.push_back({ m_stride, 3 * count, GL_FLOAT, attribDivisor});
		m_stride += sizeof(float) * 3 * count;
		break;
	case VAO::DataType::VEC4:
		m_layout.push_back({ m_stride, 4 * count, GL_FLOAT, attribDivisor});
		m_stride += sizeof(float) * 4 * count;
		break;
	case VAO::DataType::MAT3:
		// for MAT3 i push 3 vec3  'count' times
		for (int c = 0; c < count; ++c) {
			for (int i = 0; i < 3; ++i) {
				m_layout.push_back({ m_stride, 3, GL_FLOAT, attribDivisor});
				m_stride += sizeof(float) * 3;
			}
		}
		break;
	case VAO::DataType::MAT4:
		// for MAT4 i push 4 vec4  'count' times
		for (int c = 0; c < count; ++c) {
			for (int i = 0; i < 4; ++i) {
				m_layout.push_back({ m_stride, 4, GL_FLOAT, attribDivisor});
				m_stride += sizeof(float) * 4;
			}
		}
		break;
	default:
		break;
	}
}

void VAO::linkVBO(const VBO& vbo)
{
	vbo.bind();
	bind();
	for (int i = 0; i < m_layout.size(); ++i) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i, 
			m_layout[i].count, 
			m_layout[i].dataType, 
			GL_FALSE, 
			m_stride, 
			(void*)m_layout[i].offset
		);
		//glVertexAttribDivisor(i, m_layout[i].attribDivisor);
	}
}

void VAO::bind() const
{
	if (s_currentBoundVAO != m_id) {
		glBindVertexArray(m_id);
		s_currentBoundVAO = m_id;
	}
}

void VAO::unbind() const
{
	if (s_currentBoundVAO != 0) {
		glBindVertexArray(0);
		s_currentBoundVAO = 0;
	}
}

void VAO::create()
{
	if (m_id == 0) {
		glGenVertexArrays(1, &m_id);
	}
}
