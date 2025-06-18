#include "Session.h"
#include "GameRoom.h"
#include <memory>
#include <utility>

void Session::EnterGameRoom(std::shared_shared_ptr<GameRoom> room) {
    gameRoom_ = room; // weak_ptr�� shared_ptr ����
}

void Session::ProcessPacket() {
    switch (static_cast<PacketID>(receivedHeader_.id))
    {
    case PacketID::MATCH_REQ:
        LobbyManager::Instance().TryMatch();
        break;
    case PacketID::PLACE_STONE_REQ:
        // ���ӹ濡 �������� ���� ó��
        if (auto room = gameRoom_.lock()) // weak_ptr�� shared_ptr�� ��ȯ �õ�
        {
            PlaceStoneReqBody* body = reinterpret_cast<PlaceStoneReqBody*>(receivedBody_.data());
            room->HandlePlaceStone(shared_from_this(), body->x, body->y);
        }
        break;
        // ...
    }
}