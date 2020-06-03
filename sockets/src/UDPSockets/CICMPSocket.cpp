/*
 * CICMPSocket.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: vxuser
 */

#include "UDPSockets/CICMPSocket.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <netdb.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <iostream>

using namespace std;

CICMPSocket::CICMPSocket(const std::string &soketname, const std::string &adress) :
		soketname(soketname), sock(-1),
		__Send(&CICMPSocket::_Error), __Received(&CICMPSocket::_Error)
{
#ifndef _WIN32
	memset( &to, 0, sizeof( to));
	to.sin_family = AF_INET;

	// try to convert as dotted decimal address, else if that fails assume it's a hostname
	to.sin_addr.s_addr = inet_addr(adress.c_str());

    if ( to.sin_addr.s_addr == static_cast<u_int>(-1))
	{
		struct hostent *hp = gethostbyname( adress.c_str());

		if ( !hp)
		{
			std::cerr << "unknown host "<< adress << std::endl;
			return;
		}

        to.sin_family = static_cast<unsigned short>(hp -> h_addrtype);

        bcopy( hp -> h_addr, reinterpret_cast<caddr_t>(&to.sin_addr), static_cast<unsigned int>(hp -> h_length));
	}

	if ( ( sock = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");	/* probably not running as superuser */
		return;
	}

	if ( fcntl( sock, F_SETFL, O_NONBLOCK, 1 ) == -1 )
	{
		std::cout << "Socket " << " failed to set non-blocking\n";

		close( sock);
		sock = -1;
		return;
	}

	__Send     = &CICMPSocket::_Send;
	__Received = &CICMPSocket::_Received;
#endif
}

CICMPSocket::~CICMPSocket()
{
#ifdef _WIN32
    closesocket( sock);
#else
	close( sock);
#endif
}

uint16_t CICMPSocket::in_cksum(uint16_t *addr, unsigned len)
{
	uint16_t answer = 0;
	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	uint32_t sum = 0;

	while ( len > 1)
	{
		sum += *addr++;
		len -= 2;
	}

	// mop up an odd byte, if necessary
	if ( len == 1)
	{
		*(unsigned char *)&answer = *(unsigned char *)addr ;
		sum += answer;
	}

	// add back carry outs from top 16 bits to low 16 bits
	sum = (sum >> 16) + (sum & 0xffff); // add high 16 to low 16
	sum += (sum >> 16); // add carry
    answer = static_cast<uint16_t>(~sum); // truncate to 16 bits
	return answer;
}

/*! Отправка контрольных данных адресату */
bool CICMPSocket::_Send()
{
#ifndef _WIN32
	u_char outpack[ MAXPACKET];
    struct icmp * icp = reinterpret_cast< struct icmp *>(outpack);
	icp->icmp_type  = ICMP_ECHO;
	icp->icmp_code  = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq   = 12345;	/* seq and id must be reflected */
    icp->icmp_id    = static_cast<uint16_t>(getpid());

    icp->icmp_cksum = in_cksum(reinterpret_cast<unsigned short *>(icp), DEFDATALEN + ICMP_MINLEN);

    int i = static_cast<int>(sendto(sock, reinterpret_cast<char *>(outpack), DEFDATALEN + ICMP_MINLEN, 0,
                                    reinterpret_cast<struct sockaddr*>(&to), static_cast<socklen_t>(sizeof(struct sockaddr_in))));

	if ( i < 0 || i != DEFDATALEN + ICMP_MINLEN)
	{
		return false;
	}

	return true;
#else
    return false;
#endif
}


/*!Прием контрольных данных от адресата */
bool CICMPSocket::_Received()
{
#ifndef _WIN32
	struct sockaddr_in from;
	u_char packet[ DEFDATALEN + MAXIPLEN + MAXICMPLEN];
	string hostname;
	struct icmp *icp;
	int ret, fromlen, hlen;
	fd_set rfds;

	// Watch stdin (fd 0) to see when it has input.
	FD_ZERO( &rfds);
	FD_SET( sock, &rfds);

	for ( int i = 0; i < 1000; i++)
	{
		fromlen = sizeof( sockaddr_in);

        if ( ( ret = static_cast<int>(recvfrom( sock, reinterpret_cast<char *>(packet), sizeof( packet), 0, reinterpret_cast<struct sockaddr *>(&from),
                                                reinterpret_cast<socklen_t*>(&fromlen))) ) <= 0)
		{
			return false;
		}

		if ( from.sin_addr.s_addr != to.sin_addr.s_addr)
		{
			continue;
		}

		// Check the IP header
		hlen = sizeof( struct ip );

		if ( ret < (hlen + ICMP_MINLEN))
		{
			return false;
		}

		// Now the ICMP part
        icp = reinterpret_cast<struct icmp *>(packet + hlen);

		if ( icp->icmp_type == ICMP_ECHOREPLY)
		{
			//cout << "Recv: echo reply"<< endl;
			if ( icp->icmp_seq != 12345)
			{
				return false;
			}

			if (icp->icmp_id != getpid())
			{
				return false;
			};

			return true;
		}
		else
		{
			continue;
		}
    }
#endif
	return false;
}

/*! Функция ошибки сокета из-за проблемы инициализации */
bool CICMPSocket::_Error()
{
	return false;
}
