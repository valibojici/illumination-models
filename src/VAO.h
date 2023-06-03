#pragma once
#include "Buffer/VBO.h"

/// <summary>
/// Class for abstracting Vertex Array
/// </summary>
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
	// specify datatype and count
	void addLayout(DataType dataType, unsigned int count, unsigned int attribDivisor = 0);

	// link data from VBO to this VAO
	void linkVBO(const VBO& vbo);

	// bind this VAO
	void bind() const;
	void unbind() const;
	void create();
private:

	// layout for 1 data type
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
	std::vector<VAOLayout> m_layout; // layout info for each data type

	unsigned int m_id = 0;
	static unsigned int s_currentBoundVAO; // id of currently bound VAO
	unsigned int m_stride = 0; // current stride in bytes
};