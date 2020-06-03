#include "UDPSockets/CUDPFifoSocket.h"
#include <iostream>
#include <cstring>

CUDPFifoSocket::CUDPFifoSocket(const std::string& NameSocket,
		const std::string& ip_me, uint16_t port_me) : CUDPSocket(NameSocket, ip_me, port_me, true)
{
	future = std::async(std::launch::async, &CUDPFifoSocket::ReceivedInThread, this);
}

CUDPFifoSocket::CUDPFifoSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::vector<std::string> & ip_to, uint16_t port_to) :
        CUDPSocket(NameSocket, ip_me, port_me, ip_to, port_to, true)
{ }

CUDPFifoSocket::~CUDPFifoSocket()
{
	loop = false;
}

int CUDPFifoSocket::Received(void* buf, uint16_t size)
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

void CUDPFifoSocket::ReceivedInThread()
{
	while (loop)
	{
        char buf[MAXLENPACKEGE];
		int r = CUDPSocket::Received(buf, sizeof(buf));
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
                memcpy(element.data, buf, static_cast<size_t>(r));
                element.size = static_cast<size_t>(r);

                mutex.lock();
                listFifo.push(std::move(element));
                mutex.unlock();
            }
		}
	}
}
