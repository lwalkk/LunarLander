#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int size);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	int GetCount() const;

private:
	unsigned int m_ID;
	int m_Count;
};
#endif