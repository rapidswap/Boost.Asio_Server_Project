#include "Session.h"
#include "GameRoom.h"
#include "LobbyManager.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

Session::~Session()
{
    // 접속 종료 시 로비에서 퇴장 처리 (LobbyManager에 Leave 함수가 있어야 합니다)
    LobbyManager::Instance().Leave(shared_from_this());
}

void Session::start()
{
    LobbyManager::Instance().Enter(shared_from_this());
    do_read_header();
}

void Session::EnterGameRoom(std::shared_ptr<GameRoom> room) {
    gameRoom_ = room;
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
        if (auto room = gameRoom_.lock())
        {
            PlaceStoneReqBody* body = reinterpret_cast<PlaceStoneReqBody*>(receivedBody_.data());
            room->HandlePlaceStone(shared_from_this(), body->x, body->y);
        }
        break;
    default:
        std::cout << "Unknown Packet ID: " << (int)receivedHeader_.id << '\n';
        break;
    }
}