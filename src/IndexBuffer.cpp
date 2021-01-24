#include "IndexBuffer.h"
#include "Resources.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int size) 
	: m_Count(size / sizeof(unsigned int))
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_ID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

int IndexBuffer::GetCount() const
{
	return m_Count;
}



