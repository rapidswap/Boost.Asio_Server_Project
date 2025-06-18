#pragma once
#include <iostream>
#include <memory>
#include <list>
#include <mutex>
#include "GameRoom.h" 


class Session;

class LobbyManager
{
public:
	static LobbyManager& Instance()
	{
		static LobbyManager instance;
		return instance;
	}

	void Enter(std::shared_ptr<Session> session)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		sessions_.push_back(session);
		std::cout << "A client entered the lobby. Total: " << sessions_.size() << '\n';
	}

	void Leave(std::shared_ptr<Session> session)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		sessions_.remove(session);
		std::cout << "A client left the lobby. Total: " << sessions_.size() << '\n';
	}

	void TryMatch()
	{
		std::shared_ptr<Session> player1, player2;

		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (sessions_.size() < 2) {
				return;
			}
			player1 = sessions_.front();
			sessions_.pop_front();
			player2 = sessions_.front();
			sessions_.pop_front();
		}

		std::cout << "Matching Success! Creating a game room...\n";

		// GameRoom을 생성하고 시작하는 로직 추가
		auto room = std::make_shared<GameRoom>(player1, player2);
		player1->EnterGameRoom(room);
		player2->EnterGameRoom(room);
		room->StartGame();
	}

private:
	LobbyManager() = default;
	LobbyManager(const LobbyManager&) = delete;
	LobbyManager& operator=(const LobbyManager&) = delete; 

	std::mutex mutex_;
	std::list<std::shared_ptr<Session>> sessions_;
};