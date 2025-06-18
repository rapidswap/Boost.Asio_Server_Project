#include "GameRoom.h"
#include "Session.h" // Session의 전체 정의를 알아야 하므로 포함
#include <iostream>

GameRoom::GameRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session> player2)
{
	players_[0] = player1;
	players_[1] = player2;
}

void GameRoom::StartGame()
{
	// TODO: 게임 시작 처리
	std::cout << "GameRoom: Game Started!\n";
}

void GameRoom::HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y)
{
	// TODO: 돌 놓기 처리
	std::cout << "GameRoom: A player placed a stone at (" << (int)x << ", " << (int)y << ")\n";
	// TODO: 양쪽 플레이어에게 PLACE_STONE_NTF 전송
}