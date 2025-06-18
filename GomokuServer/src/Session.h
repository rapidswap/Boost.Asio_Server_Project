#pragma once
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "Packet.h"

// GameRoom의 전체 정의 대신 전방 선언을 사용하여 순환 참조를 끊습니다.
class GameRoom;

class Session : public std::enable_shared_from_this<Session>
{
public:
	explicit Session(boost::asio::ip::tcp::socket socket);
	~Session();

	void start();
	void EnterGameRoom(std::shared_ptr<GameRoom> room);

private:
	void do_read_header();
	void do_read_body();
	void ProcessPacket();

	boost::asio::ip::tcp::socket socket_;
	PacketHeader receivedHeader_;
	std::vector<char> receivedBody_;
	std::weak_ptr<GameRoom> gameRoom_; // GameRoom을 약한 참조로 가집니다.
};