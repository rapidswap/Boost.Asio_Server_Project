using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;
using System.Runtime.InteropServices; // Marshal 클래스를 사용하기 위해 추가!
using System.Buffers; // Buffer.BlockCopy를 사용하기 위해 추가!

public class NetworkManager : MonoBehaviour
{
    private static NetworkManager _instance;
    public static NetworkManager Instance
    {
        get
        {
            if (_instance == null)
            {
                GameObject go = new GameObject("NetworkManager");
                _instance = go.AddComponent<NetworkManager>();
                DontDestroyOnLoad(go);
            }
            return _instance;
        }
    }

    // [추가] 인스턴스가 존재하는지 안전하게 확인하는 함수
    public static bool Exists()
    {
        return _instance != null;
    }

    private TcpClient socket;
    private NetworkStream stream;
    
    private readonly Dictionary<PacketID, Action<byte[]>> packetHandlers = new Dictionary<PacketID, Action<byte[]>>();
    
    public event Action<GameStartNtfBody> OnGameStart;
    public event Action<PlaceStoneNtfBody> OnPlaceStone;
    public event Action<GameEndNtfBody> OnGameEnd;

    void Awake()
    {
        if (_instance != null && _instance != this)
        {
            Destroy(gameObject);
        }
        else
        {
            _instance = this;
            DontDestroyOnLoad(gameObject);
            InitializePacketHandlers();
        }
    }

    private void InitializePacketHandlers()
    {
        packetHandlers[PacketID.GAME_START_NTF] = (data) => OnGameStart?.Invoke(PacketConvert.ByteArrayToStructure<GameStartNtfBody>(data));
        packetHandlers[PacketID.PLACE_STONE_NTF] = (data) => OnPlaceStone?.Invoke(PacketConvert.ByteArrayToStructure<PlaceStoneNtfBody>(data));
        packetHandlers[PacketID.GAME_END_NTF] = (data) => OnGameEnd?.Invoke(PacketConvert.ByteArrayToStructure<GameEndNtfBody>(data));
    }

    public async Task Connect(string host, int port)
    {
        if (socket != null && socket.Connected) return;
        try
        {
            socket = new TcpClient();
            await socket.ConnectAsync(host, port);
            stream = socket.GetStream();
            Debug.Log("Connected to server!");
            _ = BeginRead();
        }
        catch (Exception e)
        {
            Debug.LogError($"Connection failed: {e.Message}");
        }
    }

    private async Task BeginRead()
    {
        while (socket != null && socket.Connected)
        {
            try
            {
                byte[] headerBuffer = new byte[Packet.HEADER_SIZE];
                int bytesRead = await stream.ReadAsync(headerBuffer, 0, Packet.HEADER_SIZE);
                if (bytesRead < Packet.HEADER_SIZE) break;

                PacketHeader header = PacketConvert.ByteArrayToStructure<PacketHeader>(headerBuffer);
                int bodySize = header.size - Packet.HEADER_SIZE;
                
                byte[] bodyBuffer = null;
                if (bodySize > 0)
                {
                    bodyBuffer = new byte[bodySize];
                    bytesRead = await stream.ReadAsync(bodyBuffer, 0, bodySize);
                    if (bytesRead < bodySize) break;
                }

                if (packetHandlers.TryGetValue((PacketID)header.id, out var handler))
                {
                    handler.Invoke(bodyBuffer);
                }
            }
            catch (Exception e)
            {
                Debug.LogError($"Read error: {e.Message}");
                break;
            }
        }
        Disconnect();
    }

    public void SendMatchReq()
    {
        PacketHeader header = new PacketHeader
        {
            id = (ushort)PacketID.MATCH_REQ,
            size = (ushort)Packet.HEADER_SIZE
        };
        byte[] packet = PacketConvert.StructureToByteArray(header);
        stream.Write(packet, 0, packet.Length);
    }

    public void SendPlaceStone(byte x, byte y)
    {
        PacketHeader header = new PacketHeader {
            id = (ushort)PacketID.PLACE_STONE_REQ,
            size = (ushort)(Packet.HEADER_SIZE + Marshal.SizeOf<PlaceStoneReqBody>())
        };

        PlaceStoneReqBody body = new PlaceStoneReqBody { x = x, y = y };

        byte[] packet = new byte[header.size];
        Buffer.BlockCopy(PacketConvert.StructureToByteArray(header), 0, packet, 0, Packet.HEADER_SIZE);
        Buffer.BlockCopy(PacketConvert.StructureToByteArray(body), 0, packet, Packet.HEADER_SIZE, Marshal.SizeOf<PlaceStoneReqBody>());

        stream.Write(packet, 0, packet.Length);
    }
    
    public void Disconnect()
    {
        if (socket == null || !socket.Connected) return;
        stream.Close();
        socket.Close();
        socket = null;
        Debug.Log("Disconnected from server.");
    }

    private void OnApplicationQuit()
    {
        Disconnect();
    }
}