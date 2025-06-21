#pragma once
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "Packet.h"

// GameRoom�� ��ü ���� ��� ���� ������ ����Ͽ� ��ȯ ������ �����ϴ�.
class GameRoom;

class Session : public std::enable_shared_from_this<Session>
{
public:
	explicit Session(boost::asio::ip::tcp::socket socket);
	~Session();

	void start();
	void EnterGameRoom(std::shared_ptr<GameRoom> room);
	void SendPacket(std::shared_ptr<std::vector<char>> packet);

private:
	void do_read_header();
	void do_read_body();
	void ProcessPacket();

	boost::asio::ip::tcp::socket socket_;
	PacketHeader receivedHeader_;
	std::vector<char> receivedBody_;
	std::weak_ptr<GameRoom> gameRoom_; // GameRoom�� ���� ������ �����ϴ�.
};