#pragma once

#define MAX_BUFFER_SIZE	1024000		// 1000kb; 1mb

template <typename T>
class Buffer {
private:
	T *buffer;
	unsigned int buf_size;
public:
	Buffer();
	Buffer(unsigned int size);
	~Buffer();
	unsigned int GetSize();
	T* GetPtr();
};