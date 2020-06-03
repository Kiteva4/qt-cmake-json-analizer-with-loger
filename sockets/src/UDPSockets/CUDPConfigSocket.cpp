#include "UDPSockets/CUDPConfigSocket.h"
#include "UDPSockets/CUDPFifoSocket.h"
#include "UDPSockets/CUDPRSFifoSocket.h"
#include "UDPSockets/CUDPRSSocket.h"
#include "UDPSockets/CUDPSocket.h"
#include <cstdio>
#include <iostream>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

int compress(unsigned char *dest,   unsigned long *destLen,
                                 const unsigned char *source, unsigned long sourceLen);

int uncompress(unsigned char *dest,   unsigned long *destLen,
                                   const unsigned char *source, unsigned long sourceLen);

#ifdef __cplusplus
}
#endif


static void bufNotComment(char * buf, size_t size)
{
    size_t j = 0;
    bool notCopy = false;

    for (size_t i = 0; i < size - 1; i++)
    {
        if (buf[i] == '/' && buf[i + 1] == '*')
        {
            notCopy = true;
        }

        if (!notCopy)
        {
            buf[j++] = buf[i];
        }

        if (buf[i] == '*' && buf[i + 1] == '/')
        {
            notCopy = false;
            i++;
        }
    }

    if (!notCopy)
    {
        buf[j++] = buf[size];
    }

    for (; j < size; j++)
    {
    	buf[j] = 0;
    }
}

static bool ParseJson(rapidjson::Document &doc, const std::string &path)
{
//    std::vector<char> readBuffer;
	char readBuffer[100000] = {};
//    char buf;

    FILE* file = fopen(path.c_str(), "rb");

    if (file == nullptr) return false;

    fread(readBuffer, 1, 100000, file);
//    while(fread(&buf, 1, 1, file))
//    	readBuffer.push_back(buf);

    bufNotComment(readBuffer, 100000);
    //Print(readBuffer.data(), sizeof(readBuffer.size()));
    if (doc.Parse(readBuffer).HasParseError())
    {
    	std::cout << "Ошибка, формат файла " << path << " некорректен" << std::endl;
    	return false;
    }

    fclose(file);

    return true;
}

CUDPConfigSocket::CUDPConfigSocket(const char *net_path, const char *main_path) :
    socket(nullptr), net_path(net_path), main_path(main_path), block(false)
{

}

CUDPConfigSocket::CUDPConfigSocket(bool block, const char *net_path, const char *main_path) :
    socket(nullptr), net_path(net_path), main_path(main_path), block(block)
{

}

CUDPConfigSocket::~CUDPConfigSocket()
{
    delete socket;
}

void CUDPConfigSocket::Create(const char *link_name)
{
	this->link_name = link_name;

	delete socket;
	socket = nullptr;

    zip = false;
    typeSocket = NOT_SOCKET;
    fifo = false;
    udp = false;
    list_systems_rs = 0;
    ElementInNetJson = 0;
    dest_ip.clear();

    if (!DefNameBlock())
    {
        std::cout << "Ошибка, при чтении файла main.json, у сокета под именем " << link_name << std::endl;
        return;
    }

    //std::cout << "Имя поля - " << name_block << std::endl;

    if (!FindSocketData())
    {
        std::cout << "Ошибка, при чтении файла net.json, у сокета под именем " << link_name << std::endl;
        return;
    }

    CreateSocket();
}

int CUDPConfigSocket::Send(void *data, int len)
{
	int result = -1;

    if (socket != nullptr)
    {
        if (zip)
        {
            unsigned long size;
            if (compress(compressBuf, &size, reinterpret_cast<unsigned char *>(data), len) == Z_OK)
                result = socket->Send(compressBuf, size);
        }
        else
        {
            result = socket->Send(data, static_cast<uint16_t>(len));
        }
    }
	else
		NotCreateSocket();

	return result;
}

int CUDPConfigSocket::GetReceived(void *data, int len)
{
	int result = -1;

	if (socket != nullptr)
    {
        if (zip)
        {
            unsigned long size = socket->Received(compressBuf, sizeof(compressBuf));
            result = size;
            unsigned long sizebuf = len;

            if (size > 0)
            {
                if (uncompress(reinterpret_cast<unsigned char *>(data), &sizebuf, compressBuf, size) == Z_OK)
                    result = sizebuf;
                else
                    result = GetReceived(data, len);
            }
        }
        else
        {
            result = socket->Received(data, static_cast<uint16_t>(len));
        }
    }
	else
		NotCreateSocket();

	return result;
}

bool CUDPConfigSocket::DefNameBlock()
{
    rapidjson::Document main_doc;

    if (!ParseJson(main_doc, main_path)) return false;

    if (!main_doc.IsObject()) return false;
    if (!main_doc.HasMember("block_name")) return false;
    if (!main_doc["block_name"].IsString()) return false;

    name_block = main_doc["block_name"].GetString();
    return true;
}

bool CUDPConfigSocket::FindSocketData()
{
    rapidjson::Document net_doc;

    if (!ParseJson(net_doc, net_path)) return false;

    if (!net_doc.IsObject()) return false;

    if (!net_doc.HasMember("rs485_port")) return false;
    if (!net_doc["rs485_port"].IsUint()) return false;
    rs485_port = net_doc["rs485_port"].GetUint();

    if (!net_doc.HasMember("port_delta")) return false;
    if (!net_doc["port_delta"].IsUint()) return false;
    port_delta = net_doc["port_delta"].GetUint();

    if (!net_doc.HasMember("blocks")) return false;
    if (!net_doc["blocks"].IsArray()) return false;

    for (auto i = net_doc["blocks"].GetArray().begin();
         i != net_doc["blocks"].GetArray().end(); i++)
    {
        if ((*i).HasMember("block_name") &&
                (*i)["block_name"].IsString() &&
                (*i)["block_name"].GetString() == name_block)
        {
            if ((*i).HasMember("ip") && (*i)["ip"].IsString())
            {
                ip = (*i)["ip"].GetString();
            }
            else
            {
                std::cout << "Не найдено поле \"ip\" в сокете " << link_name << std::endl;
                return false;
            }

            if ((*i).HasMember("in") && (*i)["in"].IsArray())
            {
                uint16_t CurrentElementInNetJson = 0;
                for (auto j = (*i)["in"].GetArray().begin();
                     j != (*i)["in"].GetArray().end(); j++, CurrentElementInNetJson++)
                {
                    if ((*j).HasMember("name") &&
                            (*j)["name"].IsString() &&
                            (*j)["name"].GetString() == std::string(link_name))
                    {
                        ElementInNetJson = CurrentElementInNetJson;
                        typeSocket = RECV_SOCKET;

                        if ((*j).HasMember("port") && (*j)["port"].IsInt())
                        {
                            port = (*j)["port"].GetInt();
                        }
                        else
                        {
                            std::cout << "Не найдено поле \"port\" в сокете " << link_name << std::endl;
                            typeSocket = NOT_SOCKET;
                            return false;
                        }

                        if ((*j).HasMember("fifo") && (*j)["fifo"].IsBool())
                        {
                            fifo = (*j)["fifo"].GetBool();
                        }

                        if ((*j).HasMember("udp") && (*j)["udp"].IsBool())
                        {
                            udp = (*j)["udp"].GetBool();
                        }

                        if ((*j).HasMember("zip") && (*j)["zip"].IsBool())
                        {
                            zip = (*j)["zip"].GetBool();
                        }

                        break;
                    }
                }
            }
            if ((*i).HasMember("out") && (*i)["out"].IsArray())
            {
                uint16_t CurrentElementInNetJson = 0;
                for (auto j = (*i)["out"].GetArray().begin();
                     j != (*i)["out"].GetArray().end(); j++, CurrentElementInNetJson++)
                {
                    if ((*j).HasMember("name") &&
                            (*j)["name"].IsString() &&
                            (*j)["name"].GetString() == std::string(link_name))
                    {
                        ElementInNetJson = CurrentElementInNetJson;
                        typeSocket = SEND_SOCKET;

                        if ((*j).HasMember("port") && (*j)["port"].IsInt())
                        {
                            port = (*j)["port"].GetInt();
                        }
                        else
                        {
                            std::cout << "Не найдено поле \"port\" в сокете " << link_name << std::endl;
                            typeSocket = NOT_SOCKET;
                            return false;
                        }

                        if ((*j).HasMember("dest_port") && (*j)["dest_port"].IsInt())
                        {
                            dest_port = (*j)["dest_port"].GetInt();
                        }
                        else
                        {
                            std::cout << "Не найдено поле \"dest_port\" в сокете " << link_name << std::endl;
                            typeSocket = NOT_SOCKET;
                            return false;
                        }

                        std::string dest_block;
                        if ((*j).HasMember("dest_block") && (*j)["dest_block"].IsString())
                        {
                            dest_block = (*j)["dest_block"].GetString();
                        }
                        else
                        {
                            std::cout << "Не найдено поле \"port\" в сокете " << link_name << std::endl;
                            typeSocket = NOT_SOCKET;
                            return false;
                        }

                        if (strstr(dest_block.c_str(), "SELF") != nullptr)
                        {
                        	 dest_ip.push_back("127.0.0.1");
                        }

                        if ((*j).HasMember("fifo") && (*j)["fifo"].IsBool())
                        {
                            fifo = (*j)["fifo"].GetBool();
                        }

                        if ((*j).HasMember("udp") && (*j)["udp"].IsBool())
                        {
                            udp = (*j)["udp"].GetBool();
                        }

                        if ((*j).HasMember("zip") && (*j)["zip"].IsBool())
                        {
                            zip = (*j)["zip"].GetBool();
                        }

                        uint8_t currentlistsystem = 0;
                        for (auto j = net_doc["blocks"].GetArray().begin();
                             j != net_doc["blocks"].GetArray().end(); j++)
                        {
                            if ((*j).HasMember("block_name") &&
                                    (*j)["block_name"].IsString() &&
									strstr(dest_block.c_str(), (*j)["block_name"].GetString()) != nullptr)
                            {
                                list_systems_rs |= 1 << currentlistsystem;
                                if ((*j).HasMember("ip") && (*j)["ip"].IsString())
                                {
                                    dest_ip.push_back((*j)["ip"].GetString());
                                }
                                else
                                {
                                    std::cout << "Не найдено поле \"ip\" в сокете " << link_name << std::endl;
                                    typeSocket = NOT_SOCKET;
                                    return false;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
    return true;
}

void CUDPConfigSocket::CreateSocket()
{
    switch (typeSocket)
    {
    case RECV_SOCKET:
        if ( udp &&  fifo) CreateUDPReciveFifoSocket();
        if (!udp &&  fifo) CreateUDPRSReciveFifoSocket();
        if ( udp && !fifo) CreateUDPReciveSocket();
        if (!udp && !fifo) CreateUDPRSReciveSocket();
        break;
    case SEND_SOCKET:
        if ( udp &&  fifo) CreateUDPSendFifoSocket();
        if (!udp &&  fifo) CreateUDPRSSendFifoSocket();
        if ( udp && !fifo) CreateUDPSendSocket();
        if (!udp && !fifo) CreateUDPRSSendSocket();
        break;
    default:
        NotCreateSocket();
        break;
    }
}

/* ________________________________________UDP_____________________________________________________________________ */

void CUDPConfigSocket::CreateUDPSendSocket()
{
	for (size_t i = 0; i < dest_ip.size(); i++)
        printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d [udp]\033[0m\n",
				link_name.c_str(), ip.c_str(), port, dest_ip[i].c_str(), dest_port);
    socket = new CUDPSocket(link_name, ip, port, dest_ip, dest_port, block);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPReciveSocket()
{
    printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  | [udp]\033[0m\n",
			link_name.c_str(), ip.c_str(), port);
    socket = new CUDPSocket(link_name, ip, port, block);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPReciveFifoSocket()
{
    printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  | [udp, fifo]\033[0m\n",
			link_name.c_str(), ip.c_str(), port);
    socket = new CUDPFifoSocket(link_name, ip, port);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPSendFifoSocket()
{
    for (size_t i = 0; i < dest_ip.size(); i++)
        printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d [udp, fifo]\033[0m\n",
				link_name.c_str(), ip.c_str(), port, dest_ip[i].c_str(), dest_port);
    socket = new CUDPFifoSocket(link_name, ip, port, dest_ip, dest_port);
    printf("\n");
}

/* _________________________________________UDP+RS_________________________________________________________________ */

void CUDPConfigSocket::CreateUDPRSSendSocket()
{
    std::string flags = GetStringFlags();

    for (size_t i = 0; i < dest_ip.size(); i++)
        printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d %s\033[0m\n",
                link_name.c_str(), ip.c_str(), port, dest_ip[i].c_str(), dest_port, flags.c_str());

    printf("\033[32mSocket  rs: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d %s\033[0m\n",
           link_name.c_str(), ip.c_str(), port, ip.c_str(), rs485_port, flags.c_str());
    socket = new CUDPRSSocket(link_name, ip, port, dest_ip, dest_port, rs485_port, dest_port + port_delta, ElementInNetJson, list_systems_rs);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPRSReciveSocket()
{
    std::string flags = GetStringFlags();

    printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  | %s\033[0m\n",
            link_name.c_str(), ip.c_str(), port, flags.c_str());
    printf("\033[32mSocket  rs: %-30s ip = %-15s port = %-5d  | %s\033[0m\n",
            link_name.c_str(), ip.c_str(), port + port_delta, flags.c_str());
    socket = new CUDPRSSocket(link_name, ip, port, port + port_delta, block);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPRSReciveFifoSocket()
{
    std::string flags = GetStringFlags();

    printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  | %s\033[0m\n",
            link_name.c_str(), ip.c_str(), port, flags.c_str());
    printf("\033[32mSocket  rs: %-30s ip = %-15s port = %-5d  | %s\033[0m\n",
            link_name.c_str(), ip.c_str(), port + port_delta, flags.c_str());
    socket = new CUDPRSFifoSocket(link_name, ip, port, port + port_delta);
    printf("\n");
}

void CUDPConfigSocket::CreateUDPRSSendFifoSocket()
{
    std::string flags = GetStringFlags();

    for (size_t i = 0; i < dest_ip.size(); i++)
        printf("\033[32mSocket eth: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d %s\033[0m\n",
                link_name.c_str(), ip.c_str(), port, dest_ip[i].c_str(), dest_port, flags.c_str());
    printf("\033[32mSocket  rs: %-30s ip = %-15s port = %-5d  |  dest_ip = %-15s dest_port = %-5d %s\033[0m\n",
            link_name.c_str(), ip.c_str(), port, ip.c_str(), rs485_port, flags.c_str());
    socket = new CUDPRSFifoSocket(link_name, ip, port, dest_ip, dest_port, rs485_port, dest_port + port_delta, ElementInNetJson, list_systems_rs);
    printf("\n");
}

/* ________________________________________________________________________________________________________________ */

void CUDPConfigSocket::NotCreateSocket()
{
    printf("\033[31mSocket    : %-30s Не создан!\033[0m\n\n", link_name.c_str());
}

std::string CUDPConfigSocket::GetStringFlags()
{
    std::string result;

    if (fifo)                   result  = "[fifo";
    if (udp &&  result.empty()) result  = "[udp";
    if (udp && !result.empty()) result += ", udp";
    if (zip &&  result.empty()) result  = "[zip";
    if (zip && !result.empty()) result += ", zip";

    if (!result.empty())        result += "]";

    return result;
}
