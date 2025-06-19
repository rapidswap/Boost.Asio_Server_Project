#include "Session.h"
#include "GameRoom.h"
#include "LobbyManager.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

Session::~Session()
{
    // 접속 종료 시 로비에서 퇴장 처리
    if (auto lobby = &LobbyManager::Instance())
    {
        lobby->Leave(shared_from_this());
    }
}

void Session::start()
{
    LobbyManager::Instance().Enter(shared_from_this());
    do_read_header();
}

void Session::EnterGameRoom(std::shared_ptr<GameRoom> room) {
    gameRoom_ = room; // weak_ptr에 shared_ptr 대입
}

void Session::do_read_header()
{
    auto self = shared_from_this();
    boost::asio::async_read(socket_, boost::asio::buffer(reinterpret_cast<char*>(&receivedHeader_), HEADER_SIZE),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec) {
                do_read_body();
            }
        });
}

void Session::do_read_body()
{
    auto self = shared_from_this();
    size_t bodySize = receivedHeader_.size - HEADER_SIZE;
    if (bodySize > 0) {
        receivedBody_.resize(bodySize);
        boost::asio::async_read(socket_, boost::asio::buffer(receivedBody_.data(), bodySize),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
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

void Session::ProcessPacket()
{
    switch (static_cast<PacketID>(receivedHeader_.id))
    {
    case PacketID::MATCH_REQ:
        std::cout << "Received MATCH_REQ from a client.\n";
        LobbyManager::Instance().TryMatch();
        break;
    case PacketID::PLACE_STONE_REQ:
        if (auto room = gameRoom_.lock()) // weak_ptr을 lock()하여 유효한 shared_ptr인지 확인
        {
            PlaceStoneReqBody* body = reinterpret_cast<PlaceStoneReqBody*>(receivedBody_.data());
            room->HandlePlaceStone(shared_from_this(), body->x, body->y);
        }
        break;
    default:
        std::cout << "Unknown Packet ID: " << static_cast<int>(receivedHeader_.id) << '\n';
        break;
    }
}

void Session::SendPacket(const char* data, int size)
{
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(data, size),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (ec) {
                std::cout << "SendPacket Error: " << ec.message() << '\n';
            }
        });
}