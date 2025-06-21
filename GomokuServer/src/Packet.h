#pragma once
#include <cstdint> // uint16_t, uint8_t 등 고정 크기 정수 사용

// 헤더의 크기는 4바이트로 고정
const int HEADER_SIZE = 4;

// 모든 패킷의 맨 앞에 붙는 공통 헤더
struct PacketHeader
{
	uint16_t size; // 패킷 전체 크기 (헤더 + 바디)
	uint16_t id;   // 패킷 ID
};

// 패킷의 종류를 나타내는 ID 목록
enum class PacketID : uint16_t
{
	// 로그인 (향후 확장용)
	LOGIN_REQ = 101,
	LOGIN_RES = 102,

	// 로비 (향후 확장용)
	ENTER_LOBBY_REQ = 201,
	ENTER_LOBBY_RES = 202, // 오타 수정: LObBY -> LOBBY

	// 매칭
	MATCH_REQ = 301,
	MATCH_RES = 302,

	// 게임 진행
	GAME_START_NTF  = 501, // 게임 시작 알림
	PLACE_STONE_REQ = 502, // 돌 놓기 요청
	PLACE_STONE_NTF = 503, // 돌 놓기 알림
	GAME_END_NTF    = 504, // 게임 종료 알림
};


// ===== 패킷 바디(Body) 구조체 정의 =====

// PLACE_STONE_REQ 패킷의 바디
struct PlaceStoneReqBody
{
	uint8_t x;
	uint8_t y;
};

// GAME_START_NTF 패킷의 바디
struct GameStartNtfBody
{
	bool isBlack; // 내가 흑돌인지 여부
};

// PLACE_STONE_NTF 패킷의 바디
struct PlaceStoneNtfBody
{
	uint8_t x;
	uint8_t y;
	bool isBlack; // 놓여진 돌이 흑돌인지 여부
};
