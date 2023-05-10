#pragma once
#include "Buffer/VBO.h"

class VAO
{
public:
	enum class DataType {
		FLOAT,
		UNSIGNED_INT,
		VEC2,
		VEC3,
		VEC4,
	};
	VAO() = default;
	~VAO();
	void addLayout(DataType dataType, unsigned int count, unsigned int attribDivisor = 0);
	void linkVBO(const VBO& vbo);
	void bind() const;
	void unbind() const;
	void create();
private:
	struct VAOLayout {
		unsigned int offset = 0;
		unsigned int count = 0;
		unsigned int dataType = GL_FLOAT;
		unsigned int attribDivisor = 0;
		VAOLayout() = default;
		VAOLayout(unsigned int offset, 
			unsigned int count, 
			unsigned int type, 
			unsigned int attribDivisor = 0)
			: offset(offset), count(count), dataType(type), attribDivisor(attribDivisor) {}
	};

	unsigned int m_id = 0;
	static unsigned int s_currentBoundVAO;
	unsigned int m_stride = 0;
	std::vector<VAOLayout> m_layout;	
};