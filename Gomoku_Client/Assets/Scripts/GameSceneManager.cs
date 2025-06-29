using UnityEngine;
using UnityEngine.UI;
using TMPro;
public class GameSceneManager : MonoBehaviour
{
    [Header("UI References")]
    public Button matchButton;
    public TextMeshProUGUI statusText;
    public Transform boardTransform;

    [Header("Prefabs")]
    public GameObject cellPrefab;
    public GameObject blackStonePrefab;
    public GameObject whiteStonePrefab;

    private Button[,] boardCells = new Button[19, 19];
    private bool isMyTurn = false;
    private bool isGameStarted = false;

    void Start()
    {
        InitializeBoard();

        // NetworkManager에 연결
        statusText.text = "서버에 연결 중...";
        _ = NetworkManager.Instance.Connect("127.0.0.1", 12345);

        // 버튼 클릭 시 매칭 요청 보내기
        matchButton.onClick.AddListener(OnMatchButtonClick);

        // NetworkManager의 이벤트를 구독합니다.
        NetworkManager.Instance.OnGameStart += HandleGameStart;
        NetworkManager.Instance.OnPlaceStone += HandlePlaceStone;
        NetworkManager.Instance.OnGameEnd += HandleGameEnd;
    }

    private void OnDestroy()
    {
        // NetworkManager.Exists()로 인스턴스가 파괴되었는지 확인 후 안전하게 구독 해제
        if (NetworkManager.Exists())
        {
            NetworkManager.Instance.OnGameStart -= HandleGameStart;
            NetworkManager.Instance.OnPlaceStone -= HandlePlaceStone; 
            NetworkManager.Instance.OnGameEnd -= HandleGameEnd;      
        }
    }

    void InitializeBoard()
{
    if (cellPrefab == null || boardTransform == null)
    {
        Debug.LogError("Cell Prefab 또는 Board Transform이 설정되지 않았습니다.");
        return;
    }

    RectTransform boardRect = boardTransform as RectTransform;
    if (boardRect == null)
    {
        Debug.LogError("Board Transform에 RectTransform 컴포넌트가 없습니다.");
        return;
    }

    float boardWidth = boardRect.rect.width;
    float boardHeight = boardRect.rect.height;

    float spacingX = boardWidth / 18.0f;
    float spacingY = boardHeight / 18.0f;

    for (int y = 0; y < 19; y++)
    {
        for (int x = 0; x < 19; x++)
        {
            GameObject cellGo = Instantiate(cellPrefab, boardTransform);
            cellGo.name = $"Cell_{x}_{y}";

            RectTransform cellRect = cellGo.GetComponent<RectTransform>();
            
            cellRect.anchorMin = new Vector2(0, 1);
            cellRect.anchorMax = new Vector2(0, 1);
            cellRect.pivot = new Vector2(0.5f, 0.5f);

            cellRect.sizeDelta = new Vector2(spacingX * 0.9f, spacingY * 0.9f);

            cellRect.anchoredPosition = new Vector2(x * spacingX, -y * spacingY);

            Button cellButton = cellGo.GetComponent<Button>();

            int capturedX = x;
            int capturedY = y;

            cellButton.onClick.AddListener(() => OnCellClick(capturedX, capturedY));
            boardCells[x, y] = cellButton;
        }
    }
}

    public void OnMatchButtonClick()
    {
        statusText.text = "매칭 요청 중...";
        matchButton.interactable = false;
        NetworkManager.Instance.SendMatchReq();
    }

    private void OnCellClick(int x, int y)
    {
        if (!isGameStarted)
        {
            statusText.text = "아직 게임이 시작되지 않았습니다.";
            return;
        }
        if (!isMyTurn)
        {
            statusText.text = "당신의 턴이 아닙니다.";
            return;
        }

        Debug.Log($"Clicked on cell ({x},{y}). Sending PLACE_STONE_REQ to server.");
        NetworkManager.Instance.SendPlaceStone((byte)x, (byte)y);
    }
    private void HandleGameStart(GameStartNtfBody data)
    {
        // UI 스레드에서 UI 업데이트
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            statusText.text = data.isBlack ? "게임 시작! 당신은 흑돌입니다." : "게임 시작! 당신은 백돌입니다.";
            matchButton.gameObject.SetActive(false);
            isGameStarted = true;
            isMyTurn = data.isBlack;
        });
    }

    private void HandlePlaceStone(PlaceStoneNtfBody data)
    {
         UnityMainThreadDispatcher.Instance().Enqueue(() =>
         {
            GameObject stonePrefab = data.isBlack ? blackStonePrefab : whiteStonePrefab;

            GameObject stoneObject = Instantiate(stonePrefab, boardTransform);
            stoneObject.name = $"Stone_{data.x}_{data.y}";

            RectTransform stoneRect = stoneObject.GetComponent<RectTransform>();

            RectTransform boardRect = boardTransform as RectTransform;
            float spacingX = boardRect.rect.width / 18.0f;
            float spacingY = boardRect.rect.height / 18.0f;

            stoneRect.anchorMin = new Vector2(0, 1);
            stoneRect.anchorMax = new Vector2(0, 1);
            stoneRect.pivot = new Vector2(0.5f, 0.5f); 

            stoneRect.anchoredPosition = new Vector2(data.x * spacingX, -data.y * spacingY);

            stoneRect.localScale = Vector3.one;    

            boardCells[data.x, data.y].interactable = false;

            isMyTurn = !isMyTurn; 
            statusText.text = isMyTurn ? "당신의 턴입니다." : "상대방의 턴입니다.";
         });
    }

    private void HandleGameEnd(GameEndNtfBody data)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            statusText.text = data.isWinner ? "승리했습니다!" : "패배했습니다.";
            isGameStarted = false;
            isMyTurn = false;
        });
    }
}