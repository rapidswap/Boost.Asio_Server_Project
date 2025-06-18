#pragma once

#include <memory>
#include <array>
#include "Session.h"
#include "Packet.h"

class GameRoom :public std::enable_shared_from_this<GameRoom>
{
public:
	GameRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session> player2)
	{
		players_[0] = player1;
		playesr_[1] = player2;
	}

	void StartGame()
	{
		// TODO: 게임 시작 처리
		std::cout << "GameRoom: Game Started!\n";
	}

	void HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y)
	{
		// TODO: 돌 놓기 처리
		std::cout << "GameRoom: A player placed a stone st(" << (int)x << ", " << (int)y << ")\n";
		// TODO: 양쪽 플레이어에게 PLACE_STONE_NTF 전송
	}

private:
	std::array<std::shared_ptr<Session>, 2>players_;
	// TODO: 오목판, 현재 턴 등 게임 상태 변수 추가
};