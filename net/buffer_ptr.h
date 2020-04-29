#ifndef BUFFER_PTR
#define BUFFER_PTR

#include <string.h>
#include "simple_buffer.h"

class BufferPtr
{
private:
    /* data */
public:
    BufferPtr(const void* buff, size_t len);
    BufferPtr(SimpleBuffer);
    ~BufferPtr();

    void setBuffer(const void* buff, size_t len);

    const void* data();
    size_t length();

private:
    const void* _buff;
    size_t _len;
    SimpleBuffer _s_buff;
};


#endif
