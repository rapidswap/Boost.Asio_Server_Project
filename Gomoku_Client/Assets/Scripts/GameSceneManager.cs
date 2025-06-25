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

    // 꼭짓점과 꼭짓점 사이의 전체 거리를 계산합니다.
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

            // [추가] 셀 버튼의 크기를 간격보다 약간 작게 설정합니다.
            // 예를 들어, 간격의 90% 크기로 설정하면 10%의 여유 공간이 생깁니다.
            cellRect.sizeDelta = new Vector2(spacingX * 0.9f, spacingY * 0.9f);

            // [수정] 셀 버튼의 위치는 이전과 동일하게 정확한 꼭짓점 중앙에 배치합니다.
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
            // 1. 돌 프리팹을 선택합니다.
            GameObject stonePrefab = data.isBlack ? blackStonePrefab : whiteStonePrefab;

            // 2. (핵심!) 셀 버튼이 아닌, 'boardTransform'(BoardPanel)의 자식으로 돌을 생성합니다.
            GameObject stoneObject = Instantiate(stonePrefab, boardTransform);
            stoneObject.name = $"Stone_{data.x}_{data.y}";

            // 3. 생성된 돌의 Rect Transform을 가져옵니다.
            RectTransform stoneRect = stoneObject.GetComponent<RectTransform>();

            // 4. InitializeBoard에서 셀 버튼의 위치를 계산했던 것과 '완벽하게 동일한 방식'으로 돌의 위치를 계산합니다.
            RectTransform boardRect = boardTransform as RectTransform;
            float spacingX = boardRect.rect.width / 18.0f;
            float spacingY = boardRect.rect.height / 18.0f;

            // 앵커를 top-left로 설정합니다.
            stoneRect.anchorMin = new Vector2(0, 1);
            stoneRect.anchorMax = new Vector2(0, 1);
            stoneRect.pivot = new Vector2(0.5f, 0.5f); // 돌 자체의 중심점은 정중앙

            // 계산된 위치에 돌을 배치합니다.
            stoneRect.anchoredPosition = new Vector2(data.x * spacingX, -data.y * spacingY);

            // 스케일은 1로 고정하여 원래 크기를 유지합니다.
            stoneRect.localScale = Vector3.one;
            
            // --- 수정 끝 ---

            // 해당 위치의 셀 버튼은 이제 클릭만 막으면 됩니다.
            boardCells[data.x, data.y].interactable = false;

            // 턴을 넘깁니다.
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