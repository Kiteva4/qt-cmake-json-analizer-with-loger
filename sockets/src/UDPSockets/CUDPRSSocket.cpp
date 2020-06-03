#include "UDPSockets/CUDPRSSocket.h"

#include <iostream>
#include <limits>
#include <cstring>

CUDPRSSocket::CUDPRSSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_eth, uint16_t port_rs,
                           bool block) :
    sock_eth(NameSocket + " ETH", ip_me, port_eth, block),
    sock_rs(NameSocket + " RS", ip_me, port_rs, block),
    timeLastRecivePack(0), name(NameSocket)
{

}

CUDPRSSocket::CUDPRSSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::string & ip_to, uint16_t port_eth, uint16_t port_rs,
                       uint16_t saved_id, uint16_t number_element, uint8_t number_system) :
    sock_eth(NameSocket + " ETH", ip_me, port_me, ip_to, port_eth),
    sock_rs(NameSocket + " RS", ip_me, port_me, ip_me, port_rs),
    eth_package(number_element, saved_id, number_system),
    timeLastRecivePack(0), name(NameSocket)
{

}

CUDPRSSocket::CUDPRSSocket(const std::string & NameSocket,
                       const std::string & ip_me, uint16_t port_me,
                       const std::vector<std::string> & ip_to, uint16_t port_to, uint16_t port_rs,
                       uint16_t saved_id, uint16_t number_element, uint8_t number_system) :
    sock_eth(NameSocket + " ETH", ip_me, port_me, ip_to, port_to),
    sock_rs(NameSocket + " RS", ip_me, port_me, ip_me, port_rs),
    eth_package(number_element, saved_id, number_system),
    timeLastRecivePack(0), name(NameSocket)
{

}

CUDPRSSocket::~CUDPRSSocket()
{

}

int CUDPRSSocket::Send(void *buf, uint16_t size)
{
    if (size > MAXLENPACKEGE)
    {
        std::cout << "size buffer in socket " << name << " more that MAXLENPACKEGE" << std::endl;
        return -1;
    }

    memcpy(eth_package.data, buf, size);
    eth_package.head.id_last_pack++;

    sock_eth.Send(&eth_package, size + sizeof(PackageSocket::head));
    sock_rs.Send(&eth_package, size + sizeof(PackageSocket::head));

    return size;
}

int CUDPRSSocket::Received(void *buf, uint16_t size)
{
    static PackageSocket rs_package;
    static uint16_t id_last_pack = 0;

    auto r_rs = sock_rs.Received(&rs_package, sizeof (PackageSocket));
    auto r_eth = sock_eth.Received(&eth_package, sizeof (PackageSocket));

    int result = 0;

    auto isEthValid = r_eth > 0 &&
            (IsPackageLater(id_last_pack, eth_package.head.id_last_pack) ||
             difftime(time(nullptr), timeLastRecivePack) > timeout);

    auto isRsValid = r_rs > 0 &&
            (IsPackageLater(id_last_pack, rs_package.head.id_last_pack) ||
             difftime(time(nullptr), timeLastRecivePack) > timeout);

    if (isEthValid && isRsValid)
    {
        // Есть данные по двум линиям
        if (IsPackageLater(rs_package.head.id_last_pack, eth_package.head.id_last_pack))
        {
            // Пакет интернета оказался более свежим
            result = ReciveData(eth_package, static_cast<uint16_t>(r_eth), buf, size, id_last_pack);
        }
        else
        {
            // Пакет rs оказался более свежим
            result = ReciveData(rs_package, static_cast<uint16_t>(r_rs), buf, size, id_last_pack);
        }

        timeLastRecivePack = time(nullptr);
    }
    else if (isEthValid)
    {
        // Есть пакеты только по интернету
        result = ReciveData(eth_package, static_cast<uint16_t>(r_eth), buf, size, id_last_pack);
        timeLastRecivePack = time(nullptr);
    }
    else if (isRsValid)
    {
        // Есть пакеты только по rs
        result = ReciveData(rs_package, static_cast<uint16_t>(r_rs), buf, size, id_last_pack);
        timeLastRecivePack = time(nullptr);
    }

    return result;
}

bool CUDPRSSocket::IsPackageLater(const uint16_t old_package, const uint16_t new_package)
{
    int32_t razn = new_package - old_package;

    return (razn < std::numeric_limits<uint16_t>::max() / 2 && razn > 0) ||
           (razn > -std::numeric_limits<uint16_t>::max() / 2 && razn < 0);
}

int CUDPRSSocket::ReciveData(const PackageSocket &package, const uint16_t size_recive,
                             void *buf, uint16_t size, uint16_t &id_last_pack)
{
    if (size < size_recive - sizeof(PackageSocket::head))
    {
        std::cout << "size buffer in socket " << name << " less that buffer "
                  << size << " < " << size_recive - static_cast<int>(sizeof(PackageSocket::head)) << std::endl;
        return -1;
    }
    memcpy(buf, package.data, size_recive - sizeof(PackageSocket::head));
    id_last_pack = package.head.id_last_pack;
    return size_recive - sizeof(PackageSocket::head);
}
