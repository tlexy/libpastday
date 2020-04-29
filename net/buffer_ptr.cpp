#include "buffer_ptr.h"

BufferPtr::BufferPtr(const void* buff, size_t len)
    :_buff(buff),
    _len(len)
{
}

void BufferPtr::setBuffer(const void* buff, size_t len)
{
    _buff = buff;
    _len = len;
}

BufferPtr::BufferPtr(SimpleBuffer buffer)
    :_buff(buffer->c_str()),
    _len(buffer->size()),
    _s_buff(buffer)
{
}

BufferPtr::~BufferPtr()
{
}

const void* BufferPtr::data()
{
    return _buff;
}

size_t BufferPtr::length()
{
    return _len;
}

