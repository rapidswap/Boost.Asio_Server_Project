#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "Packet.h"

using namespace boost::asio;
using ip::tcp;

class Client
{
public:
    Client(io_context& io_context)
        : socket_(io_context)
    {
    }

    void Connect(const std::string& host, short port)
    {
        socket_.connect(tcp::endpoint(ip::make_address(host), port));
        std::cout << "[Client] Connected to server.\n";

        // 서버로부터 오는 메시지를 비동기적으로 계속 받기 시작
        do_read_header();
    }

    void SendPlaceStone(uint8_t x, uint8_t y)
    {
        PacketHeader header;
        PlaceStoneReqBody body;
        header.id = static_cast<uint16_t>(PacketID::PLACE_STONE_REQ);
        header.size = sizeof(header) + sizeof(body);
        body.x = x;
        body.y = y;

        std::vector<char> packet(header.size);
        memcpy(packet.data(), &header, sizeof(header));
        memcpy(packet.data() + sizeof(header), &body, sizeof(body));

        write(socket_, buffer(packet));
    }

    void SendMatchReq()
    {
        PacketHeader header;
        header.id = static_cast<uint16_t>(PacketID::MATCH_REQ);
        header.size = sizeof(header);
        write(socket_, buffer(&header, header.size));
        std::cout << "[Client] Sent MATCH_REQ.\n";
    }


private:
    void do_read_header()
    {
        async_read(socket_, buffer(reinterpret_cast<char*>(&receivedHeader_), HEADER_SIZE),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec) {
                    do_read_body();
                }
                else {
                    std::cout << "[Client] Connection closed by server.\n";
                    socket_.close();
                }
            });
    }

    void do_read_body()
    {
        size_t bodySize = receivedHeader_.size - HEADER_SIZE;
        if (bodySize > 0) {
            receivedBody_.resize(bodySize);
            async_read(socket_, buffer(receivedBody_.data(), bodySize),
                [this](boost::system::error_code ec, std::size_t /*length*/)
                {
                    if (!ec) {
                        ProcessPacket();
                        do_read_header();
                    }
                });
        }
        else {
            ProcessPacket();
            do_read_header();
        }
    }

    void ProcessPacket()
    {
        switch (static_cast<PacketID>(receivedHeader_.id))
        {
        case PacketID::GAME_START_NTF:
        {
            GameStartNtfBody* body = reinterpret_cast<GameStartNtfBody*>(receivedBody_.data());
            std::cout << ">> Game Started! You are " << (body->isBlack ? "Black" : "White") << ".\n";
            break;
        }
        case PacketID::PLACE_STONE_NTF:
        {
            PlaceStoneNtfBody* body = reinterpret_cast<PlaceStoneNtfBody*>(receivedBody_.data());
            std::cout << ">> " << (body->isBlack ? "Black" : "White") << " placed a stone at ("
                << (int)body->x << ", " << (int)body->y << ").\n";
            break;
        }
        default:
            std::cout << ">> Unknown packet received. ID: " << receivedHeader_.id << "\n";
            break;
        }
    }

    tcp::socket socket_;
    PacketHeader receivedHeader_;
    std::vector<char> receivedBody_;
};


int main()
{
    try
    {
        io_context io_context;
        Client client(io_context);
        client.Connect("127.0.0.1", 12345);

        // 네트워크 통신은 별도의 스레드에서 처리
        std::thread t([&io_context]() { io_context.run(); });

        // 메인 스레드에서는 사용자 입력 처리
        char line[1024] = { 0, };
        while (std::cin.getline(line, 1024))
        {
            if (strncmp(line, "match", 5) == 0)
            {
                client.SendMatchReq();
            }
            else if (strncmp(line, "put", 3) == 0)
            {
                int x, y;
                if (sscanf(line + 4, "%d %d", &x, &y) == 2) {
                    client.SendPlaceStone(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
                }
            }
        }

        t.join(); // 스레드 종료 대기
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}