#ifndef BASE_HANDLE_H
#define BASE_HANDLE_H

#include "poller.h"
#include "sapper.h"
#include "buffer_ptr.h"
#include "simple_buffer.h"
#include <string>

typedef struct _base_session
{
    SimpleBuffer buffer;
    SapperPtr sapper;
}BaseSession;

class BaseHandle
{
public:
    BaseHandle();
    virtual void handle(std::vector<SapperPtr>& active_sapper);
    virtual size_t getPacket(BufferPtr buffer) = 0;
    virtual void handlePacket(const std::vector<BaseSession>& packets) = 0;

    virtual ~BaseHandle();

private:
};

class BinaryPacketHandle : public BaseHandle
{
public:
    BinaryPacketHandle();
    virtual void handle(std::vector<SapperPtr>& active_sapper);
    virtual size_t getPacket(BufferPtr buffer);
    virtual void handlePacket(const std::vector<BaseSession>& packets);
};

#endif
