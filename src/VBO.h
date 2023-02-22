#pragma once
#include "GL/glew.h"

class VBO {
private:
	unsigned int m_id = 0;
public:
	VBO() = default;
	VBO(void* data, unsigned int size);
	void create();
	void bufferData(void *data, unsigned int size) const;
	void bind() const;
	void unbind() const;
};