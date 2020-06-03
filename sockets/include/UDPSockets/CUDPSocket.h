/*
 * CUDPSocket.h
 *
 *  Created on: Dec 7, 2018
 *      Author: vxuser
 */

#ifndef SRC_CUDPSOCKET_H_
#define SRC_CUDPSOCKET_H_

#include <string>
#ifdef _WIN32
#include<winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <vector>
#include <UDPSockets/ISocket.h>

#ifndef WIN32
using SOCKET = int;
#endif

class CUDPSocket : public ISocket {
public:
	/**
	 * Конструктор сокета приема данных c возможностью задания возможности блокировки потока
	 **/
	CUDPSocket(const std::string & NameSocket,
			const std::string & ip_me, uint16_t port_me,
			bool block = false);

	/**
	 * Конструктор сокета выдачи данных
	 **/
	CUDPSocket(const std::string & NameSocket,
			const std::string & ip_me, uint16_t port_me,
            const std::string & ip_to, uint16_t port_to,
            bool block = false);

	/**
	 * Конструктор сокета выдачи данных нескольким адресатам
	 **/
	CUDPSocket(const std::string & NameSocket,
			const std::string & ip_me, uint16_t port_me,
            const std::vector<std::string> & ip_to, uint16_t port_to,
            bool block = false);

    ~CUDPSocket() override;

    int Send(void * buf, uint16_t size) override;
    int Received(void * buf, uint16_t size) override;

protected:
    SOCKET sock;
	const std::string name;
	std::vector<struct sockaddr_in> addr_to;

	/**
	 * Делегат отправки
	 **/
	int (CUDPSocket::*__Send)(void * buf, uint16_t size);
	/**
	 * Делегат приема
	 **/
	int (CUDPSocket::*__Received)(void * buf, uint16_t size);

	/**
	 * Инициализация сокета на текущем устройстве
	 **/
	bool initMeData(const std::string & ip_me, uint16_t port_me);

	/**
	 * Функция отправки данных
	 **/
	int _Send(void * buf, uint16_t size);

	/**
	 * Функция приема данных данных
	 **/
	int _Received(void * buf, uint16_t size);

	/**
	 * Функция ошибки оправки данных, в связи с тем, что сокет настроен на прием
	 **/
	int _ErrorSend(void * buf, uint16_t size);

	/**
	 * Функция ошибки приема данных, в связи с тем, что сокет настроен на отправку
	 **/
	int _ErrorReceived(void * buf, uint16_t size);

	/**
	 * Функция ошибки сокета из-за проблемы инициализации
	 **/
	int _Error(void * buf, uint16_t size);
};

#endif /* SRC_CUDPSOCKET_H_ */
