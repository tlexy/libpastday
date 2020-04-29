#include "base_handle.h"

BaseHandle::BaseHandle()
{}

void BaseHandle::handle(std::vector<SapperPtr>& active_sapper)
{
    //包进行批量处理
    /*
    * 如判断active_sapper中谁是读事件，再判断是否接收了一个完整的包（通过getPacket函数）
    */
}

BaseHandle::~BaseHandle()
{}

///--------------------------------------------------------------

BinaryPacketHandle::BinaryPacketHandle()
{}

void BinaryPacketHandle::handle(std::vector<SapperPtr>& active_sapper)
{
    std::vector<BaseSession> vecs;
    for (int i = 0; i < active_sapper.size(); ++i)
    {
        if (!active_sapper[i]->isReadEvent() || !active_sapper[i]->isPostRead())
        {
            continue;
        }
        size_t ret = getPacket(BufferPtr(active_sapper[i]->receiveBuffer(), active_sapper[i]->receiveLength()));
        if (ret > 0)
        {
            //即收到了一个包
            SimpleBuffer buffer = std::make_shared<std::string>();
            active_sapper[i]->readBuffer(*buffer.get(), ret);
            BaseSession sess;
            sess.buffer = buffer;
            sess.sapper = active_sapper[i];
            vecs.push_back(sess);
        }
    }
    handlePacket(vecs);
}

size_t BinaryPacketHandle::getPacket(BufferPtr buffer)
{
    return 0;
}

//handlePacket还跑在io线程中，强烈建议将包分发到业务线程中进行处理，不要阻障io线程收发包
void BinaryPacketHandle::handlePacket(const std::vector<BaseSession>& packets)
{
    //包里还差个fd呀
}