#pragma once
#include <iostream>
#include <memory>
#include <list>
#include <mutex>

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

			auto player1 = sessions_.front();
			sessions_.pop_front();
			auto player2 = sessions_.front();
			sessions_.pop_front();
		}

		std::cout << "Matching Success! Two clients are now in a game\n";

		// TODO: 다음 단계에서 두 플레이어를 게임방으로 보내기
	}

private:
	LobbyManager() = default;
	LobbyManager(const LobbyManager&) = delete;
	LobbyManager& operator=(const LobbyManager&) = default;

	std::mutex mutex_;
	std::list<std::shared_ptr<Session>> sessions_;
};