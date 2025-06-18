#pragma once
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "Packet.h"

class GameRoom;
class LobbyManager;

class Session :public std::enable_shared_from_this<Session>
{
public:
	explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

	~Session()
	{
		LobbyManager::Instance().Leave(shared_from_this());
	}
	
	void EnterGameRoom(std::shared_ptr<GameRoom> room);
	
private:
	void do_read_header();
	void do_read_body();
	void ProcessPacket();

	boost::asio::ip::tcp::socket socket_;
	PacketHeader receivedHeader_;
	std::vector<char> receivedBody_;
	std::weak_ptr<GameRoom> gameRoom_;
};