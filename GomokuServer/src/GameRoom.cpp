#include "GameRoom.h"
#include "Session.h" // Session�� ��ü ���Ǹ� �˾ƾ� �ϹǷ� ����
#include <iostream>

GameRoom::GameRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session> player2)
{
	players_[0] = player1;
	players_[1] = player2;
}

void GameRoom::StartGame()
{
	// TODO: ���� ���� ó��
	std::cout << "GameRoom: Game Started!\n";
}

void GameRoom::HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y)
{
	// TODO: �� ���� ó��
	std::cout << "GameRoom: A player placed a stone at (" << (int)x << ", " << (int)y << ")\n";
	// TODO: ���� �÷��̾�� PLACE_STONE_NTF ����
}