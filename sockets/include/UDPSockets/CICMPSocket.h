/*
 * CICMPSocket.h
 *
 *  Created on: Feb 10, 2019
 *      Author: vxuser
 */

#ifndef SRC_CICMPSOCKET_H_
#define SRC_CICMPSOCKET_H_

#include <string>
#if defined(_WIN32)
#include <winsock2.h>
#elif defined(__APPLE__)
#include <netinet/in.h>
#include <netinet/ip.h>

using SOCKET = int;
#else
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

using SOCKET = int;
#endif



#include <iostream>

#define	DEFDATALEN	(64-ICMP_MINLEN)	/* default data length */
#define	MAXIPLEN	60
#define	MAXICMPLEN	76
#define	MAXPACKET	(65536 - 60 - ICMP_MINLEN)/* max packet size */

class CICMPSocket
{
public:
	CICMPSocket(const std::string &soketname, const std::string &adress);
	virtual ~CICMPSocket();


	inline bool Send() { return (this->*__Send)(); }
	inline bool Received() { return (this->*__Received)(); }

private:
	CICMPSocket() = delete;
	CICMPSocket(const CICMPSocket & ) = delete;

	struct sockaddr_in to;

	std::string soketname;
    SOCKET sock;

	uint16_t in_cksum(uint16_t *addr, unsigned len);

	/**
	 * Отправка контрольных данных адресату
	 **/
	bool _Send();
	/**
	 * Прием контрольных данных от адресата
	 **/
	bool _Received();

	/**
	 * Функция ошибки сокета из-за проблемы инициализации
	 **/
	bool _Error();

	/**
	 * Делегат отправки
	 **/
	bool (CICMPSocket::*__Send)();
	/**
	 * Делегат приема
	 **/
	bool (CICMPSocket::*__Received)();

};






#endif /* SRC_CICMPSOCKET_H_ */
