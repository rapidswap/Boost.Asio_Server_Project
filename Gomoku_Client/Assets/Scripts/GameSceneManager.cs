using UnityEngine;
using UnityEngine.UI;

public class GameSceneManager : MonoBehaviour
{
    public Button matchButton;
    public Text statusText;
    // TODO: 오목판, 돌 프리팹 등 UI 요소 연결

    void Start()
    {
        // NetworkManager에 연결
        _ = NetworkManager.Instance.Connect("127.0.0.1", 12345);

        // 버튼 클릭 시 매칭 요청 보내기
        matchButton.onClick.AddListener(() =>
        {
            statusText.text = "매칭 요청 중...";
            NetworkManager.Instance.SendMatchReq();
        });

        // NetworkManager의 이벤트를 구독합니다.
        NetworkManager.Instance.OnGameStart += HandleGameStart;
        NetworkManager.Instance.OnPlaceStone += HandlePlaceStone; // 오타 수정
        NetworkManager.Instance.OnGameEnd += HandleGameEnd;
    }

    private void OnDestroy()
    {
        // NetworkManager.Exists()로 인스턴스가 파괴되었는지 확인 후 안전하게 구독 해제
        if (NetworkManager.Exists())
        {
            NetworkManager.Instance.OnGameStart -= HandleGameStart;
            NetworkManager.Instance.OnPlaceStone -= HandlePlaceStone; // 오타 수정
            NetworkManager.Instance.OnGameEnd -= HandleGameEnd;      // 오타 수정
        }
    }

    private void HandleGameStart(GameStartNtfBody data)
    {
        // UI 스레드에서 UI 업데이트
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            statusText.text = data.isBlack ? "게임 시작! 당신은 흑돌입니다." : "게임 시작! 당신은 백돌입니다.";
            matchButton.gameObject.SetActive(false);
        });
    }

    private void HandlePlaceStone(PlaceStoneNtfBody data)
    {
         UnityMainThreadDispatcher.Instance().Enqueue(() => {
            statusText.text = $"{(data.isBlack ? "흑돌" : "백돌")}이 ({data.x}, {data.y})에 놓였습니다.";
            // TODO: 실제 오목판 위에 돌 프리팹을 생성하는 로직
         });
    }

    private void HandleGameEnd(GameEndNtfBody data)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            statusText.text = data.isWinner ? "승리했습니다!" : "패배했습니다.";
        });
    }
}