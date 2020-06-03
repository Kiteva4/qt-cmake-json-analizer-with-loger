#ifndef CUDPCONFIGSOCKET_H
#define CUDPCONFIGSOCKET_H

#include <UDPSockets/ISocket.h>
#include <vector>
#include <CThread.h>
#include <functional>
#include <list>

#ifdef _WIN32
#define UDPSOCKETNETPATH "D:/data/data_yak/config/net.json"
#define UDPSOCKETMAINPATH "D:/data/data_yak/config/main.json"
#else
#define UDPSOCKETNETPATH "/home/data/data_yak/config/net.json"
#define UDPSOCKETMAINPATH "/home/data/data_yak/config/main.json"
#endif

class CUDPConfigSocket
{
public:
    /**
     * Конструктор сокета из конфига
     */
    explicit CUDPConfigSocket(const char *net_path = UDPSOCKETNETPATH, const char *main_path = UDPSOCKETMAINPATH);

    /**
     * Конструктор блокирующего сокета из конфига
     */
    explicit CUDPConfigSocket(bool block, const char *net_path = UDPSOCKETNETPATH, const char *main_path = UDPSOCKETMAINPATH);

    /**
     * Деструктор сокета
     */
    ~CUDPConfigSocket();

    /**
     * @brief Открытие порта.
     * @param[in] link_name название порта (из сетевого конфига net.json)
     */
    void Create(const char *link_name);

    /**
     * @brief Отправка данных.
     * @param[in] data указатель на буфер
     * @param[in] len длина буфера
     * @return кол-во отправленных байт, если все хорошо; отрицательное число иначе
     */
    int Send(void *data, int len);

    /**
     * @brief Прием данных.
     * @param[in] data указатель на буфер
     * @param[in] len длина буфера
     * @return кол-во принятых байт, если все хорошо; отрицательное число, если ошибка; 0, если данных нет
     */
    int GetReceived(void *data, int len);

protected:
    ISocket * socket;

    std::string net_path;
    std::string main_path;
    std::string name_block;
    std::string link_name;
    std::string ip;

    std::vector<std::string> dest_ip;
    uint8_t list_systems_rs = 0;
    uint16_t ElementInNetJson = 0;

    uint16_t port = 0;
    uint16_t dest_port = 0;

    bool fifo = false;
    bool udp = false;
    bool zip = false;

    enum
    {
        NOT_SOCKET,
        RECV_SOCKET,
        SEND_SOCKET
    } typeSocket = NOT_SOCKET;

    uint16_t rs485_port = 0;
    uint16_t port_delta = 0;

    bool DefNameBlock();
    bool FindSocketData();

    void CreateSocket();

    void CreateUDPSendSocket();
    void CreateUDPReciveSocket();
    void CreateUDPReciveFifoSocket();
    void CreateUDPSendFifoSocket();

    void CreateUDPRSSendSocket();
    void CreateUDPRSReciveSocket();
    void CreateUDPRSReciveFifoSocket();
    void CreateUDPRSSendFifoSocket();

    void NotCreateSocket();

    unsigned char compressBuf[MAXLENPACKEGE];

    std::string GetStringFlags();

    bool block;
};

#endif // CUDPCONFIGSOCKET_H
