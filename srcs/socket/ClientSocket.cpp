#include "../../includes/socket/ClientSocket.hpp"

ClientSocket::ClientSocket(void) : BaseSocket(), addr(), addrLen(0), recvBuffer(), sendBuffer(), lastActivity(time(NULL)), closed(false)
{
}

ClientSocket::ClientSocket(int afd, const struct sockaddr_in &a, socklen_t aLen)
    : BaseSocket(), addr(a), addrLen(aLen), recvBuffer(), sendBuffer(), lastActivity(time(NULL)), closed(false)
{
    fd = afd;
}

ClientSocket::~ClientSocket(void)
{
    setClose();
}

ClientSocket::ClientSocket(const ClientSocket &ref)
    : BaseSocket(ref), addr(ref.addr), addrLen(ref.addrLen), recvBuffer(ref.recvBuffer), sendBuffer(ref.sendBuffer), lastActivity(ref.lastActivity), closed(ref.closed)
{
}

ClientSocket &ClientSocket::operator=(const ClientSocket &ref)
{
    if (this != &ref)
    {
        BaseSocket::operator=(ref);
        addr = ref.addr;
        addrLen = ref.addrLen;
        recvBuffer = ref.recvBuffer;
        sendBuffer = ref.sendBuffer;
        lastActivity = ref.lastActivity;
        closed = ref.closed;
    }
    return *this;
}

void ClientSocket::setClose()
{
    BaseSocket::setClose();
    closed = true;
}

void ClientSocket::appendRecv(const std::string &data)
{
    recvBuffer.append(data);
}

const std::string &ClientSocket::getRecvBuffer() const
{
    return recvBuffer;
}

std::string &ClientSocket::getSendBuffer()
{
    return sendBuffer;
}

void ClientSocket::touch()
{
    lastActivity = time(NULL);
}

time_t ClientSocket::getLastActivity() const
{
    return lastActivity;
}

const struct sockaddr_in &ClientSocket::getAddr() const
{
    return addr;
}

socklen_t ClientSocket::getAddrLen() const
{
    return addrLen;
}

bool ClientSocket::isClosed() const
{
    return closed;
}
