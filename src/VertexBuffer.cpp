#include "VertexBuffer.h"
#include "Resources.h"


VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	: m_Count(size)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_ID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::BufferData(const void* data)
{
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW));
}

unsigned int VertexBuffer::getID() const { return m_ID; }