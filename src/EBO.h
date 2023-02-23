#pragma once
#include "GL/glew.h"
#include <vector>

class EBO
{
private:
	unsigned int m_id = 0;
	static unsigned int s_currentBoundEBO;
public:
	EBO() = default;
	EBO(void* data, unsigned int size);
	EBO(const std::vector<unsigned int>& indices);
	~EBO();
	void create();
	void bufferData(void* data, unsigned int size) const;
	void bind() const;
	void unbind() const;
};

