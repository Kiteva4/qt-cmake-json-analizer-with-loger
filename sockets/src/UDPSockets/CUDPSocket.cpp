#include "UDPSockets/CUDPSocket.h"

#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>

#define INVALID_SOCKET	static_cast<SOCKET>(-1)
#define SOCKET_ERROR	(-1)
#endif

CUDPSocket::CUDPSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       bool block) :
    sock(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), name( NameSocket), __Send( &CUDPSocket::_Error), __Received( &CUDPSocket::_Error)
{
    (void)ip_me;

    //Create a socket
    if(sock == INVALID_SOCKET)
    {
        std::cout << "Socket " << name << " is no Create\n";
        return;
    }

    struct sockaddr_in addr_me;
    memset( &addr_me, 0, sizeof( addr_me));

    addr_me.sin_family = AF_INET;
    addr_me.sin_port   = htons( port_me);
    addr_me.sin_addr.s_addr = INADDR_ANY;

    if ( bind( sock, reinterpret_cast<struct sockaddr *>(&addr_me), sizeof( addr_me)) == SOCKET_ERROR)
    {
        std::cout << "Socket " << name << " is not assigns an address\n";
        sock = INVALID_SOCKET;
        return;
    }

    if (!block)
    {
#ifdef _WIN32
        u_long mode = 1;  // 1 to enable non-blocking socket
        ioctlsocket(sock, FIONBIO, &mode);
#else
        if ( fcntl( sock, F_SETFL, O_NONBLOCK, 1 ) == SOCKET_ERROR )
        {
            std::cout << "Socket " << name << " failed to set non-blocking\n";
            sock = INVALID_SOCKET;
            return;
        }
#endif
    }
    else
    {
#ifdef _WIN32
        char timeout = 1;
#else
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
#endif

        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == SOCKET_ERROR)
        {
            std::cout << "Socket " << name << " failed to set timeout\n";
            sock = INVALID_SOCKET;
            return;
        }
    }

    __Send     = &CUDPSocket::_ErrorSend;
    __Received = &CUDPSocket::_Received;
}

CUDPSocket::CUDPSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::string & ip_to, uint16_t port_to,
                       bool block) :
    sock(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)), name( NameSocket), __Send( &CUDPSocket::_Error), __Received( &CUDPSocket::_Error)
{
    (void)port_me;

    //Create a socket
    if(sock == INVALID_SOCKET)
    {
        std::cout << "Socket " << name << " is no Create\n";
        return;
    }

    addr_to.resize(1);

    if (ip_me == ip_to)
    {
        addr_to[0].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
#ifdef _WIN32
    else
        addr_to[0].sin_addr.s_addr = inet_addr(ip_to.c_str());
#else
    else if ( !inet_aton( ip_to.c_str(), &addr_to[0].sin_addr))
    {
        std::cout << "ip " << name << " is not correct\n";
        sock = INVALID_SOCKET;
        return;
    }
#endif

    addr_to[0].sin_family = AF_INET;
    addr_to[0].sin_port   = htons( port_to);

    if (!block)
    {
#ifdef _WIN32
        u_long mode = 1;  // 1 to enable non-blocking socket
        ioctlsocket(sock, FIONBIO, &mode);
#else
        if ( fcntl( sock, F_SETFL, O_NONBLOCK, 1 ) == SOCKET_ERROR )
        {
            std::cout << "Socket " << name << " failed to set non-blocking\n";
            sock = INVALID_SOCKET;
            return;
        }
#endif
    }
    else
    {
#ifdef _WIN32
        char timeout = 1;
#else
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
#endif

        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == SOCKET_ERROR)
        {
            std::cout << "Socket " << name << " failed to set timeout\n";
            sock = INVALID_SOCKET;
            return;
        }
    }

    __Send     = &CUDPSocket::_Send;
    __Received = &CUDPSocket::_ErrorReceived;
}

CUDPSocket::CUDPSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::vector<std::string> & ip_to, uint16_t port_to,
                       bool block) :
    sock( socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP)), name( NameSocket), __Send( &CUDPSocket::_Error), __Received( &CUDPSocket::_Error)
{
    (void)port_me;

    if ( sock == INVALID_SOCKET)
    {
        std::cout << "Socket " << name << " is no Create\n";
        return;
    }

    addr_to.resize(ip_to.size());

    for (size_t i = 0; i < ip_to.size(); i++)
    {
        if (ip_me == ip_to[i])
        {
            addr_to[i].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        }
#ifdef _WIN32
        else
            addr_to[i].sin_addr.s_addr = inet_addr(ip_to[i].c_str());
#else
        else if ( !inet_aton( ip_to[i].c_str(), &addr_to[i].sin_addr))
        {
            std::cout << "ip " << name << " is not correct\n";
            sock = INVALID_SOCKET;
            return;
        }
#endif
        addr_to[i].sin_family = AF_INET;
        addr_to[i].sin_port   = htons( port_to);
    }

    if (!block)
    {
#ifdef _WIN32
        u_long mode = 1;  // 1 to enable non-blocking socket
        ioctlsocket(sock, FIONBIO, &mode);
#else
        if ( fcntl( sock, F_SETFL, O_NONBLOCK, 1 ) == SOCKET_ERROR )
        {
            std::cout << "Socket " << name << " failed to set non-blocking\n";
            sock = INVALID_SOCKET;
            return;
        }
#endif
    }
    else
    {
#ifdef _WIN32
        char timeout = 1;
#else
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
#endif

        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == SOCKET_ERROR)
        {
            std::cout << "Socket " << name << " failed to set timeout\n";
            sock = INVALID_SOCKET;
            return;
        }
    }

    __Send     = &CUDPSocket::_Send;
    __Received = &CUDPSocket::_ErrorReceived;
}

int CUDPSocket::_Send( void * buf, uint16_t size)
{
    int send_bytes = 0;

    for (size_t i = 0; i < addr_to.size(); i++)
    {
        send_bytes = static_cast<int>(sendto( sock, static_cast<char *>(buf), size, 0, reinterpret_cast<struct sockaddr *>(&addr_to[i]), sizeof( addr_to[i])));

//        if ( send_bytes != size )
//        {
//            std::cout <<  "failed to send packet: " << name << " return value = " << send_bytes << "\n";
//        }
    }

    return send_bytes;
}

int CUDPSocket::_Received(void * buf, uint16_t size)
{
    memset( buf, 0, size);
    int recive = static_cast<int>(recvfrom( sock, static_cast<char *>(buf), size, 0, nullptr, nullptr));
    return recive > 0 ? recive : 0;

}

/*! Функция ошибки оправки данных, в связи с тем, что сокет настроен на прием */
int CUDPSocket::_ErrorSend( void * buf, uint16_t size)
{
    (void)buf;
    (void)size;

    std::cout << "Socket " << name << " is receiver\n";
    return 0;
}

/*! Функция ошибки приема данных, в связи с тем, что сокет настроен на отправку */
int CUDPSocket::_ErrorReceived( void * buf, uint16_t size)
{
    (void)buf;
    (void)size;

    std::cout << "Socket " << name << " is sender\n";
    return 0;
}

/*! Функция ошибки сокета из-за проблемы инициализации */
int CUDPSocket::_Error(void * buf, uint16_t size)
{
    (void)buf;
    (void)size;

    std::cout << "Socket " << name << " is not activate\n";
    return 0;
}

CUDPSocket::~CUDPSocket()
{
#ifdef _WIN32
        closesocket( sock);
#else
        close( sock);
#endif
}

int CUDPSocket::Send(void *buf, uint16_t size)
{
    return (this->*__Send)(buf, size);
}

int CUDPSocket::Received(void *buf, uint16_t size)
{
    return (this->*__Received)(buf, size);
}
