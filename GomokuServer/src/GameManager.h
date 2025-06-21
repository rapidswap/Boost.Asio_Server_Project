#pragma once
#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include<iostream>
#include "GameRoom.h"

class GameManager
{
public:
	static GameManager& Instance()
	{
		static GameManager instance;
		return instance;
	}

	std::shared_ptr<GameRoom> AddRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session>player2)
	{
		int room_id = next_room_id_.fetch_add(1);
		auto room = std::make_shared<GameRoom>(room_id, player1, player2);

		std::lock_guard<std::mutex> lock(mutex_);
		rooms_.insert({ room_id,room });
		std::cout << "[GameManager] A new room #" << room_id << "has been added. Total rooms: " << rooms_.size() << '\n';

		return room;
	}

	void RemoveRoom(int room_id)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		rooms_.erase(room_id);
		std::cout << "[GameManager] Room #" << room_id << " has been removed. Total rooms: " << rooms_.size() << std::endl;
	}


private:
	GameManager() = default;
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;

	std::mutex mutex_;
	std::map<int, std::shared_ptr<GameRoom>> rooms_;
	std::atomic<int> next_room_id_{ 1 };
};