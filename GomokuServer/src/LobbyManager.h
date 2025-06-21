#pragma once
#include <iostream>
#include <memory>
#include <list>
#include <mutex>
#include "GameRoom.h" 
#include "GameManager.h"

// Session은 포인터로만 다루므로 전방 선언으로 충분합니다.
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

		std::cout << "Matching Success! Asking GameManager to create a room...\n";

		// GameRoom을 생성하고 시작하는 로직
		auto room = GameManager::Instance().AddRoom(player1, player2);
		player1->EnterGameRoom(room);
		player2->EnterGameRoom(room);
		room->StartGame();
	}

private:
	LobbyManager() = default;
	LobbyManager(const LobbyManager&) = delete;
	LobbyManager& operator=(const LobbyManager&) = delete; // 오타 수정: = default -> = delete

	std::mutex mutex_;
	std::list<std::shared_ptr<Session>> sessions_;
};