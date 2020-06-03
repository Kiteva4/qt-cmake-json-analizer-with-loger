/*
 * CUDPSocket.h
 *
 *  Created on: Dec 7, 2018
 *      Author: vxuser
 */

#ifndef SRC_CUDPRSFIFOSOCKET_H_
#define SRC_CUDPRSFIFOSOCKET_H_

#include <UDPSockets/ISocket.h>
#include <UDPSockets/CUDPRSSocket.h>
#include <queue>
#include <ctime>
#include <mutex>
#include <future>

class CUDPRSFifoSocket : public CUDPRSSocket {
public:
	/**
	 * Конструктор сокета приема данных c возможностью задания возможности блокировки потока
	 **/
    CUDPRSFifoSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_eth, uint16_t port_rs);

	/**
	 * Конструктор сокета выдачи данных
	 **/
    CUDPRSFifoSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::string & ip_to, uint16_t port_eth, uint16_t port_rs,
            uint16_t saved_id, uint16_t number_element, uint8_t number_system);

	/**
	 * Конструктор сокета выдачи данных нескольким адресатам
	 **/
    CUDPRSFifoSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::vector<std::string> & ip_to, uint16_t port_eth, uint16_t port_rs,
            uint16_t saved_id, uint16_t number_element, uint8_t number_system);

    ~CUDPRSFifoSocket() override;

    int Received(void * buf, uint16_t size) override;

protected:
protected:
    struct object
    {
        char * data;
        size_t size;
    };

    std::queue<object> listFifo;

    bool loop = true;

    std::mutex mutex;

    std::future<void> future_eth;
    std::future<void> future_rs;

    void ReceivedInThread(CUDPSocket * socket);
};

#endif /* SRC_CUDPRSFIFOSOCKET_H_ */
