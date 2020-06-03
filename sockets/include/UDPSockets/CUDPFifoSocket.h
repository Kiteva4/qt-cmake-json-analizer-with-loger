/*
 * CUDPSocket.h
 *
 *  Created on: Dec 7, 2018
 *      Author: vxuser
 */

#ifndef SRC_CUDPFIFOSOCKET_H_
#define SRC_CUDPFIFOSOCKET_H_

#include <UDPSockets/CUDPSocket.h>
#include <queue>
#include <future>
#include <mutex>

class CUDPFifoSocket : public CUDPSocket
{
public:

	/**
	 * Конструктор сокета приема данных
	 **/
	CUDPFifoSocket(const std::string & NameSocket,
			const std::string & ip_me, uint16_t port_me);

    /**
     * Конструктор сокета выдачи данных нескольким адресатам
     **/
	CUDPFifoSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::vector<std::string> & ip_to, uint16_t port_to);

    ~CUDPFifoSocket() override;

    int Received(void * buf, uint16_t size) override;

protected:
    struct object
	{
        char * data;
    	size_t size;
    };

    std::queue<object> listFifo;
    bool loop = true;
    std::mutex mutex;
    std::future<void> future;

    void ReceivedInThread();

};

#endif /* SRC_CUDPSOCKET_H_ */
