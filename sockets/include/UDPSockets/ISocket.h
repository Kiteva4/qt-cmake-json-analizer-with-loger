/*
 * CUDPSocket.h
 *
 *  Created on: Dec 7, 2018
 *      Author: vxuser
 */

#ifndef ISOCKET_H_
#define ISOCKET_H_

#include <cstdint>

#define MAXLENPACKEGE 10000
#define MAXFIFOLEN 2000

typedef struct PackageSocket
{
    PackageSocket(uint16_t number_element, uint16_t saved_id, uint8_t number_system)
    {
        head.number_element = number_element;
        head.saved_id       = saved_id;
        head.id_last_pack   = 0;
        head.number_system  = number_system;
    }

    PackageSocket()
    {
        head.number_element = 0;
        head.saved_id       = 0;
        head.id_last_pack   = 0;
        head.number_system  = 0;
    }

    struct
    {
        uint16_t number_element; /* Номер элемента в массиве net.json */
        uint16_t saved_id;       /* Резерв (Пишу сюда порт отправки) */
        uint16_t id_last_pack;   /* Счетчик пакетов */
        uint8_t  number_system;  /* Номер системы в массиве net.json */
    } __attribute__((packed)) head;

    uint8_t data[MAXLENPACKEGE];

} __attribute__((packed)) PackageSocket;

class ISocket
{
public:

    ISocket() {}
    virtual ~ISocket() {}

    virtual int Received(void * buf, uint16_t size) = 0;
    virtual int Send(void* buf, uint16_t size) = 0;
};

#endif /* ISOCKET_H_ */
