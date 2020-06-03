/*
 * CUDPSocket.h
 *
 *  Created on: Dec 7, 2018
 *      Author: vxuser
 */

#ifndef SRC_CUDPRSSOCKET_H_
#define SRC_CUDPRSSOCKET_H_

#include <UDPSockets/ISocket.h>
#include <UDPSockets/CUDPSocket.h>
#include <ctime>

class CUDPRSSocket : public ISocket {
public:
	/**
	 * Конструктор сокета приема данных c возможностью задания возможности блокировки потока
	 **/
    CUDPRSSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_eth, uint16_t port_rs,
                 bool block = false);

	/**
	 * Конструктор сокета выдачи данных
	 **/
    CUDPRSSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::string & ip_to, uint16_t port_eth, uint16_t port_rs,
            uint16_t saved_id, uint16_t number_element, uint8_t number_system);

	/**
	 * Конструктор сокета выдачи данных нескольким адресатам
	 **/
    CUDPRSSocket(const std::string & NameSocket,
            const std::string & ip_me, uint16_t port_me,
            const std::vector<std::string> & ip_to, uint16_t port_to, uint16_t port_rs,
            uint16_t saved_id, uint16_t number_element, uint8_t number_system);

    ~CUDPRSSocket() override;

    int Send(void * buf, uint16_t size) override;
    int Received(void * buf, uint16_t size) override;

protected:
    CUDPSocket sock_eth;
    CUDPSocket sock_rs;

    PackageSocket eth_package;
    PackageSocket rs_package;

    uint16_t id_last_pack = 0;
    time_t timeLastRecivePack;

    std::string name;
    double timeout = 4.0;

    bool IsPackageLater(const uint16_t old_package, const uint16_t new_package);

    int ReciveData(const PackageSocket &package, const uint16_t size_recive,
                   void * buf, uint16_t size, uint16_t &id_last_pack);
};

#endif /* SRC_CUDPRSSOCKET_H_ */
