#pragma once
#include <cstdint> //uint16_t와 같은 고정 크기 정수 사용

const int HEADER_SIZE = 4;

struct PacketHeader
{
	uint16_t size;
	uint16_t id;
};