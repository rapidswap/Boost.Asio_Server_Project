#pragma once
#include <memory>
#include <array>
#include <vector>
#include "Packet.h"

// Session의 전체 정의 대신 전방 선언을 사용합니다.
class Session;

class GameRoom : public std::enable_shared_from_this<GameRoom>
{
public:
	// 생성자 선언: room_id를 받도록 수정합니다.
	GameRoom(int room_id, std::shared_ptr<Session> player1, std::shared_ptr<Session> player2);

	// 멤버 함수 선언
	void StartGame();
	void HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y);

private:
	// private 멤버 함수 선언
	void BroadcastPacket(std::shared_ptr<std::vector<char>> packet);
	bool CheckWin(int x, int y, int stoneType);
	void EndGame();


	int room_id_; 
	std::array<std::shared_ptr<Session>, 2> players_;
	std::array<std::array<int, 19>, 19> board_;
	bool isBlackTurn_ = true;
};