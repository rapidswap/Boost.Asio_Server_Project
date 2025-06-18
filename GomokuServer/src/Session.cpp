#include "Session.h"
#include "GameRoom.h"
#include <memory>
#include <utility>

void Session::EnterGameRoom(std::shared_shared_ptr<GameRoom> room) {
    gameRoom_ = room; // weak_ptr에 shared_ptr 대입
}

void Session::ProcessPacket() {
    switch (static_cast<PacketID>(receivedHeader_.id))
    {
    case PacketID::MATCH_REQ:
        LobbyManager::Instance().TryMatch();
        break;
    case PacketID::PLACE_STONE_REQ:
        // 게임방에 속해있을 때만 처리
        if (auto room = gameRoom_.lock()) // weak_ptr을 shared_ptr로 변환 시도
        {
            PlaceStoneReqBody* body = reinterpret_cast<PlaceStoneReqBody*>(receivedBody_.data());
            room->HandlePlaceStone(shared_from_this(), body->x, body->y);
        }
        break;
        // ...
    }
}