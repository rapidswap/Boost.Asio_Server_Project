#pragma once
#include <cstdint> //uint16_t�� ���� ���� ũ�� ���� ���

const int HEADER_SIZE = 4;

struct PacketHeader
{
	uint16_t size;
	uint16_t id;
};