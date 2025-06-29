#include "GameRoom.h"
#include "Session.h" 
#include "GameManager.h"
#include <iostream>
#include <vector>

GameRoom::GameRoom(int room_id, std::shared_ptr<Session> player1, std::shared_ptr<Session> player2)
	: room_id_(room_id)
{
	players_[0] = player1;
	players_[1] = player2;
	board_.fill({});
}

void GameRoom::StartGame()
{
	std::cout << "GameRoom: Game Started!\n";

	// [수정] 흑돌 플레이어 패킷 생성 및 전송
	auto blackPacket = std::make_shared<std::vector<char>>();
	blackPacket->resize(sizeof(PacketHeader) + sizeof(GameStartNtfBody));

	PacketHeader blackHeader;
	GameStartNtfBody blackBody;
	blackHeader.id = static_cast<uint16_t>(PacketID::GAME_START_NTF);
	blackHeader.size = blackPacket->size();
	blackBody.isBlack = true;

	memcpy(blackPacket->data(), &blackHeader, sizeof(blackHeader));
	memcpy(blackPacket->data() + sizeof(blackHeader), &blackBody, sizeof(blackBody));
	players_[0]->SendPacket(blackPacket);

	// [수정] 백돌 플레이어 패킷 생성 및 전송
	auto whitePacket = std::make_shared<std::vector<char>>();
	whitePacket->resize(sizeof(PacketHeader) + sizeof(GameStartNtfBody));

	PacketHeader whiteHeader;
	GameStartNtfBody whiteBody;
	whiteHeader.id = static_cast<uint16_t>(PacketID::GAME_START_NTF);
	whiteHeader.size = whitePacket->size();
	whiteBody.isBlack = false;

	memcpy(whitePacket->data(), &whiteHeader, sizeof(whiteHeader));
	memcpy(whitePacket->data() + sizeof(whiteHeader), &whiteBody, sizeof(whiteBody));
	players_[1]->SendPacket(whitePacket);
}

void GameRoom::HandlePlaceStone(std::shared_ptr<Session> player, uint8_t x, uint8_t y)
{
	// ================== [디버깅 로그 시작] ==================
	std::cout << "\n[DEBUG] HandlePlaceStone called. Received: (" << (int)x << ", " << (int)y << ")\n";

	int playerIndex = (players_[0] == player) ? 0 : 1;
	bool isBlackPlayer = (playerIndex == 0);

	std::cout << "[DEBUG] Player is " << (isBlackPlayer ? "Black" : "White")
		<< ". Current turn is " << (isBlackTurn_ ? "Black" : "White") << ".\n";

	if (isBlackPlayer != isBlackTurn_) {
		std::cout << "[DEBUG] REJECTED: Not this player's turn.\n";
		return;
	}

	if (x >= 19 || y >= 19 || board_[y][x] != 0) {
		std::cout << "[DEBUG] REJECTED: Invalid position or stone already exists.\n";
		return;
	}

	std::cout << "[DEBUG] ACCEPTED: Move is valid. Broadcasting packet...\n";
	// ================== [디버깅 로그 끝] ====================

	int stone = isBlackPlayer ? 1 : 2;
	board_[y][x] = stone;

	auto packet = std::make_shared<std::vector<char>>();
	packet->resize(sizeof(PacketHeader) + sizeof(PlaceStoneNtfBody));

	PacketHeader header;
	PlaceStoneNtfBody body;
	header.id = static_cast<uint16_t>(PacketID::PLACE_STONE_NTF);
	header.size = packet->size();
	body.x = x;
	body.y = y;
	body.isBlack = isBlackPlayer;

	memcpy(packet->data(), &header, sizeof(header));
	memcpy(packet->data() + sizeof(header), &body, sizeof(body));
	BroadcastPacket(packet);

	if (CheckWin(x, y, stone)) {
		std::cout << "[GameRoom] Game End! Winner is " << (isBlackPlayer ? "Black" : "White") << '\n';
		EndGame();
	}
	else {
		isBlackTurn_ = !isBlackTurn_;
		std::cout << "[DEBUG] Turn changed. Next turn is " << (isBlackTurn_ ? "Black" : "White") << ".\n";
	}
}

void GameRoom::BroadcastPacket(std::shared_ptr<std::vector<char>> packet)
{
	players_[0]->SendPacket(packet);
	players_[1]->SendPacket(packet);
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

void GameRoom::EndGame()
{
	std::cout << "[GameRoom #" << room_id_ << "] Game is over. Notifying players...\n";

	auto winner = isBlackTurn_ ? players_[0] : players_[1];
	auto loser = isBlackTurn_ ? players_[1] : players_[0];

	auto winPacket = std::make_shared<std::vector<char>>(sizeof(PacketHeader) + sizeof(GameEndNtfBody));
	PacketHeader winHeader;
	GameEndNtfBody winBody;
	winHeader.id = static_cast<uint16_t>(PacketID::GAME_END_NTF);
	winHeader.size = winPacket->size();
	winBody.isWinner = true;
	memcpy(winPacket->data(), &winHeader, sizeof(winHeader));
	memcpy(winPacket->data() + sizeof(winHeader), &winBody, sizeof(winBody));
	winner->SendPacket(winPacket);

	auto losePacket = std::make_shared<std::vector<char>>(sizeof(PacketHeader) + sizeof(GameEndNtfBody));
	PacketHeader loseHeader;
	GameEndNtfBody loseBody;
	loseHeader.id = static_cast<uint16_t>(PacketID::GAME_END_NTF);
	loseHeader.size = losePacket->size();
	loseBody.isWinner = false;
	memcpy(losePacket->data(), &loseHeader, sizeof(loseHeader));
	memcpy(losePacket->data() + sizeof(loseHeader), &loseBody, sizeof(loseBody));
	loser->SendPacket(losePacket);

	winner->CloseConnection();
	loser->CloseConnection();

	GameManager::Instance().RemoveRoom(room_id_);
}