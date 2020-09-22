#include "UDPSockets/CUDPRSFifoSocket.h"

#include <iostream>
#include <limits>
#include <cstring>

CUDPRSFifoSocket::CUDPRSFifoSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_eth, uint16_t port_rs) :
    CUDPRSSocket(NameSocket, ip_me, port_eth, port_rs, true)
{
    future_eth = std::async(std::launch::async, &CUDPRSFifoSocket::ReceivedInThread, this, &sock_eth);
    future_rs  = std::async(std::launch::async, &CUDPRSFifoSocket::ReceivedInThread, this, &sock_rs);
}

CUDPRSFifoSocket::CUDPRSFifoSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::string & ip_to, uint16_t port_eth, uint16_t port_rs,
                       uint16_t saved_id, uint16_t number_element, uint8_t number_system) :
    CUDPRSSocket(NameSocket, ip_me, port_me, ip_to, port_eth, port_rs,
                 saved_id, number_element, number_system)
{

}

CUDPRSFifoSocket::CUDPRSFifoSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::vector<std::string> & ip_to, uint16_t port_eth, uint16_t port_rs,
                       uint16_t saved_id, uint16_t number_element, uint8_t number_system) :
    CUDPRSSocket(NameSocket, ip_me, port_me, ip_to, port_eth, port_rs,
                 saved_id, number_element, number_system)
{

}

CUDPRSFifoSocket::~CUDPRSFifoSocket()
{
    loop = false;
}

int CUDPRSFifoSocket::Received(void *buf, uint16_t size)
{
    int result = 0;
    if (listFifo.empty()) return result;

    mutex.lock();
    object element = std::move(listFifo.front());
    listFifo.pop();
    mutex.unlock();

    if (size >= element.size)
    {
        memcpy(buf, element.data, element.size);
        result = static_cast<int>(element.size);
        delete [] element.data;
    }
    else
    {
        std::cout << "Длина буфера приема у Сокета " << name
                << " меньше, чем принято " << size << " < " << listFifo.front().size << std::endl;
    }

    return result;
}

void CUDPRSFifoSocket::ReceivedInThread(CUDPSocket *socket)
{
    while (loop)
    {
        PackageSocket buf;
        int r = socket->Received(&buf, sizeof(buf));
        if (r > 0)
        {
            if (listFifo.size() > MAXFIFOLEN)
            {
                //std::cout << "Очередь переполнена в сокете " << name << std::endl;
            }
            else
            {
                object element;
                element.data = new char[r];
                memcpy(element.data, buf.data, static_cast<size_t>(r));
                element.size = static_cast<size_t>(r);

                mutex.lock();
                if (IsPackageLater(id_last_pack, buf.head.id_last_pack) ||
                    difftime(time(nullptr), timeLastRecivePack) > timeout)
                {
                    listFifo.push(std::move(element));
                    timeLastRecivePack = time(nullptr);
                    mutex.unlock();
                }
                else
                {
                    mutex.unlock();
                    delete [] element.data;
                }

            }
        }
    }
}
