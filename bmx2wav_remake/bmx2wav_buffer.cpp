#include "bmx2wav_buffer.h"

template <typename T>
Buffer<T>::Buffer() : Buffer(MAX_BUFFER_SIZE) {}

template <typename T>
Buffer<T>::Buffer(unsigned int size) : buf_size(size) {
	if (!buf_size)
		throw;
	buffer = (T*)malloc(sizeof(T) * size);
}

template <typename T>
Buffer<T>::~Buffer() {
	free(buffer);
}

template <typename T>
unsigned int Buffer<T>::GetSize() {
	return buf_size;
}

template <typename T>
T* Buffer<T>::GetPtr() {
	return buffer;
}