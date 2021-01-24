#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H
#include <GL/glew.h>

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	void Bind() const;
	void Unbind() const;

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vl);
private:
	unsigned int m_ID;
};

#endif