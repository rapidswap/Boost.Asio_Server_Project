#pragma once
#include <cstdint> //uint16_t와 같은 고정 크기 정수 사용

const int HEADER_SIZE = 4;

struct PacketHeader
{
	uint16_t size;
	uint16_t id;
};

enum class PacketID : uint16_t
{
	LOGIN_REQ=101,
	LOGIN_RES=102,

	ENTER_LOBBY_REQ=201,
	ENTER_LObBY_RES=202,

	MATCH_REQ=301,
	MATCH_RES=302,
	MATCH_SUCCESS_NTF=303,

	GAME_START_NTF=501,
	PLACE_STONE_REQ=502,
	PLACE_STONE_NTF=503,
	GAME_END_NTF=504,

};

struct PlaceStoneReqBody
{
	uint8_t x;
	uint8_t y;
};