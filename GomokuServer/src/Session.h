#pragma once

#include "GameRoom.h"

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
	std::weak_ptr<GameRoom> gameRoom_;
};