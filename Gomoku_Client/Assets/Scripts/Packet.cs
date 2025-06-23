using System.Runtime.InteropServices;

public enum PacketID : ushort
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
    GAME_START_NTF = 501, // 게임 시작 알림
    PLACE_STONE_REQ = 502, // 돌 놓기 요청
    PLACE_STONE_NTF = 503, // 돌 놓기 알림
    GAME_END_NTF = 504, // 게임 종료 알림
}

// C++의 메모리 구조와 똑같이 맞추기 위한 어트리뷰트
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PacketHeader
{
    public ushort size;
    public ushort id;
}

public static class Packet
{
    public const int HEADER_SIZE = 4; // 헤더 크기
}

// ===== 패킷 바디(Body) 구조체 정의 =====

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PlaceStoneReqBody
{
    public byte x;
    public byte y;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct GameStartNtfBody
{
    [MarshalAs(UnmanagedType.I1)] // C++의 bool과 크기(1바이트)를 맞춤
    public bool isBlack;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PlaceStoneNtfBody
{
    public byte x;
    public byte y;
    [MarshalAs(UnmanagedType.I1)]
    public bool isBlack;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct GameEndNtfBody
{
    [MarshalAs(UnmanagedType.I1)]
    public bool isWinner;
}