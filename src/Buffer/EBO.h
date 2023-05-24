#pragma once
#include "GL/glew.h"
#include <vector>

class EBO
{
private:
	unsigned int m_id = 0;
	static unsigned int s_currentBoundEBO; // id of currently bound EBO, 0 if not bound
public:
	EBO() = default;
	EBO(void* data, unsigned int size);
	EBO(const std::vector<unsigned int>& indices);
	~EBO();
	// create EBO and assign id
	void create();
	
	// load data into EBO
	void bufferData(void* data, unsigned int size);

	// bind this EBO if not bound already
	void bind() const;

	// unbind (bind 0)
	void unbind() const;
};

