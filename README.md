# C++ Boost.Asio 기반 오목(Gomoku) 게임 서버 & Unity 클라이언트

이 프로젝트는 C++와 Boost.Asio 라이브러리를 사용하여 개발한 비동기 소켓 기반의 멀티플레이어 오목 게임 서버와, Unity 엔진으로 제작한 클라이언트의 전체 개발 과정을 담고 있습니다.

## ✨ 프로젝트 개요 및 주요 기능

### 서버 (C++)

  * **비동기 I/O 아키텍처**: Boost.Asio의 Proactor 패턴을 기반으로, 적은 스레드로 다수의 클라이언트 요청을 효율적으로 처리하는 고성능 서버 구조를 구현했습니다.
  * **객체지향 및 모듈화**: `Session`, `LobbyManager`, `GameRoom`, `GameManager` 등 각 기능별로 클래스를 분리하여 코드의 재사용성과 유지보수성을 높였습니다.
  * **스레드 안전성**: 로비, 게임방 등 여러 클라이언트가 동시에 접근하는 공유 데이터는 `std::mutex`를 사용하여 안전하게 관리합니다.
  * **완전한 게임 로직**: 로비에서의 매치메이킹, 게임방 생성, 턴 기반의 착수 처리, 5목 완성 시 승패 판정 및 게임 종료 처리까지 오목 게임 한 판의 전체 흐름을 완벽하게 구현했습니다.

### 클라이언트 (Unity C\#)

  * **비동기 네트워크 통신**: C\#의 `TcpClient`와 `async/await`를 사용하여 서버와 비동기적으로 패킷을 주고받습니다.
  * **이벤트 기반 아키텍처**: `NetworkManager`가 서버로부터 패킷을 받으면, `OnGameStart`, `OnPlaceStone` 등의 C\# 이벤트를 발생시킵니다. UI 로직은 이 이벤트를 구독하여 서버의 내부 동작과 분리된 채로 화면을 갱신합니다.
  * **인터랙티브 UI**: 유니티의 UI 시스템을 활용하여 동적으로 오목판을 생성하고, 사용자의 클릭 입력을 받아 서버로 전송하며, 서버의 알림에 따라 돌을 화면에 표시하는 등 완전한 상호작용을 구현했습니다.
  * **메인 스레드 동기화**: `UnityMainThreadDispatcher`를 구현하여, 네트워크 스레드에서 받은 이벤트를 유니티의 메인 스레드에서 안전하게 처리하도록 보장합니다.

## 🛠️ 기술 스택 (Tech Stack)

  * **서버 (Server)**
      * **Language**: C++17
      * **Library**: Boost.Asio
      * **Build**: CMake
      * **Dependency**: vcpkg
  * **클라이언트 (Client)**
      * **Engine**: Unity 2022.x
      * **Language**: C\#

## 📂 프로젝트 구조

```
/
├── .gitignore
├── README.md
├── GomokuServer/      # C++ 서버 프로젝트
│   ├── CMakeLists.txt
│   └── src/
└── GomokuClient_Unity/  # Unity 클라이언트 프로젝트
    ├── Assets/
    │   └── _Scripts/
    └── ProjectSettings/
```

## 🚀 시작하기 (Getting Started)

### 1\. C++ 서버 빌드 및 실행

**사전 요구 사항:** Visual Studio 2022, CMake, Git, vcpkg

1.  **vcpkg로 라이브러리 설치:**
    ```bash
    vcpkg install boost-asio boost-system boost-algorithm:x64-windows
    ```
2.  **CMake로 프로젝트 생성:** `GomokuServer` 폴더 안에 `build` 폴더를 만들고, 그 안에서 아래 명령어를 실행합니다.
    ```bash
    # [vcpkg_root_path]는 실제 vcpkg 설치 경로로 변경 (예: C:/vcpkg)
    cmake .. -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root_path]/scripts/buildsystems/vcpkg.cmake
    ```
3.  **빌드 및 실행:** `build` 폴더에 생성된 `.sln` 파일을 Visual Studio로 열고 `server` 프로젝트를 빌드(`Ctrl+Shift+B`)한 뒤, `Ctrl+F5`로 실행합니다.

### 2\. Unity 클라이언트 실행

1.  Unity Hub에서 `GomokuClient_Unity` 폴더를 프로젝트로 추가합니다.
2.  프로젝트를 엽니다.
3.  `_Scenes` 폴더에 있는 메인 씬을 열고, 유니티 에디터의 플레이(▶) 버튼을 누릅니다. (C++ 서버가 먼저 실행되어 있어야 합니다.)

## 🎮 게임 플레이 방법

1.  C++ 서버를 먼저 실행합니다.
2.  유니티 클라이언트를 두 개 실행합니다. (에디터에서 하나, 빌드해서 하나 실행하는 방식 추천)
3.  각 클라이언트 화면에서 `Match` 버튼을 누릅니다.
4.  잠시 후 `statusText`에 "게임 시작\! 당신은 흑돌/백돌입니다." 메시지가 출력됩니다.
5.  흑돌 차례인 클라이언트에서 오목판의 원하는 위치를 클릭하여 돌을 놓습니다.
6.  양쪽 클라이언트 화면에 돌이 놓이는 것을 확인하고, 턴이 넘어가는 것을 확인하며 게임을 진행합니다.
7.  한쪽이 5개의 돌을 연속으로 놓으면, "승리/패배" 메시지가 출력되며 게임이 종료됩니다.
