// src/GameRoom.h (수정 후)

#pragma once
#include <memory>
#include <array>
#include "Packet.h"

// Session의 전체 정의 대신 전방 선언을 사용합니다.
class Session;

class GameRoom : public std::enable_shared_from_this<GameRoom>
{
public:
	// 생성자 선언
	GameRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session> player2);

	// 멤버 함수 선언
	void StartGame();
	void HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y);

private:
	std::array<std::shared_ptr<Session>, 2> players_;
	// TODO: 오목판, 현재 턴 등 게임 상태 변수 추가
};