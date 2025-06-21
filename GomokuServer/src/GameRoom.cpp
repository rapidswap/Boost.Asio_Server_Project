#include "GameRoom.h"
#include "Session.h" // Session의 전체 정의를 알아야 하므로 포함
#include <iostream>
#include <vector>

GameRoom::GameRoom(std::shared_ptr<Session> player1, std::shared_ptr<Session> player2)
{
	players_[0] = player1;
	players_[1] = player2;
	board_.fill({});
}

void GameRoom::StartGame()
{
	// TODO: 게임 시작 처리
	std::cout << "GameRoom: Game Started!\n";

	PacketHeader blackHeader;
	GameStartNtfBody blackBody;
	blackHeader.id = static_cast<uint16_t>(PacketID::GAME_START_NTF);
	blackHeader.size = sizeof(blackHeader) + sizeof(blackBody);
	blackBody.isBlack = true;

	std::vector<char> blackPacket(blackHeader.size);
	memcpy(blackPacket.data(), &blackHeader, sizeof(blackHeader));
	memcpy(blackPacket.data() + sizeof(blackHeader), &blackBody, sizeof(blackBody));
	players_[0]->SendPacket(blackPacket.data(), blackPacket.size());

	PacketHeader whiteHeader;
	GameStartNtfBody whiteBody;
	whiteHeader.id = static_cast<uint16_t>(PacketID::GAME_START_NTF);
	whiteHeader.size = sizeof(whiteHeader) + sizeof(whiteBody);
	whiteBody.isBlack = false;

	std::vector<char> whitePacket(whiteHeader.size);
	memcpy(whitePacket.data(), &whiteHeader, sizeof(whiteHeader));
	memcpy(whitePacket.data() + sizeof(whiteHeader), &whiteBody, sizeof(whiteBody));
	players_[1]->SendPacket(whitePacket.data(), whitePacket.size());
}

void GameRoom::HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y)
{
	int playerIndex = (players_[0] == player) ? 0 : 1;
	bool isBlackPlayer = (playerIndex == 0);

	if (isBlackPlayer != isBlackTurn_) {
		return;
	}

	if (x >= 19 || y >= 19 || board_[y][x] != 0) {
		return;
	}

	board_[y][x] = isBlackPlayer ? 1 : 2;
	std::cout << "GameRoom: " << (isBlackPlayer ? "Black" : "White") << " place at(" << (int)x << ", " << (int)y << ")\n";

	PacketHeader header;
	PalceStoneNtfBody body;
	header.id = static_cast<uint16_t>(PacketID::PLACE_STONE_NTF);
	header.size = sizeof(header) + sizeof(body);
	body.x = x;
	body.y = y;
	body.isBlack = isBlackPlayer;

	std::vector<char>packet(header.size);
	memcpy(packet.data(), &header, sizeof(header));
	memcpy(packet.data() + sizeof(header), &body, sizeof(body));
	BroadcastPacket(packet.data(), packet.size());

	isBlackTurn_ = !isBlackTurn_;

	// TODO: 승패 판정
	
}

void GameRoom::BroadcastPacket(const char* data, int size)
{
	players_[0]->SendPacket(data, size);
	players_[1]->SendPacket(data, size);
}

bool GameRoom::CheckWin(int x, int y, int stoneType)
{
	int dy[] = { 1,1,1,0 };
	int dx[] = { -1,0,1,1 };
	
	for (int i = 0; i < 4; i++) {
		int count = 1;

		for (int j = 1; j < 5; j++) {
			int nx = x + dx[i] * j;
			int ny = y + dy[i] * j;
			if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19 && board_[ny][nx] == stoneType)
				count++;
			else
				break;
		}

		for (int j = 1; j < 5; ++j)
		{
			int nx = x - dx[i] * j;
			int ny = y - dy[i] * j;
			if (nx >= 0 && nx < 19 && ny >= 0 && ny < 19 && board_[ny][nx] == stoneType)
				count++;
			else
				break;
		}

		if (count >= 5) {
			return true;
		}
	}
	return false;
}