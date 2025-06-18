# C++ Boost.Asio 기반 오목 게임 서버 프로젝트

이 프로젝트는 C++와 Boost.Asio 라이브러리를 사용하여 비동기 소켓 기반의 멀티플레이어 오목 게임 서버를 개발하는 과정을 담고 있습니다. 기본적인 네트워크 통신부터 시작하여, 여러 사용자를 관리하는 로비, 매치메이킹, 그리고 실제 게임 로직을 단계적으로 구현하는 것을 목표로 합니다.

## ✨ 주요 기능 (Current Features)

현재까지 구현된 서버의 핵심 기능은 다음과 같습니다.

* **비동기 I/O 서버 구조**: Boost.Asio의 Proactor 패턴을 기반으로 한 고성능 비동기 소켓 통신을 구현했습니다.
* **패킷 기반 통신**: `[Header] + [Body]` 구조의 커스텀 프로토콜을 정의하여 클라이언트와 서버가 정해진 규격으로 통신합니다.
* **로비 및 세션 관리**: 접속한 클라이언트(`Session`)들을 중앙에서 관리하는 로비 시스템을 구현했습니다.
* **스레드 안전성 확보**: 여러 클라이언트의 동시 요청으로부터 공유 데이터를 보호하기 위해 `std::mutex`를 사용합니다.
* **기초 매치메이킹**: 로비에 대기 중인 플레이어 2명을 찾아 게임방으로 연결하는 간단한 매치메이킹 로직을 구현했습니다.
* **게임방 생성**: 매칭된 플레이어들을 위한 `GameRoom` 객체를 생성하고 관리합니다.

## 🛠️ 기술 스택 (Tech Stack)

* **언어**: C++17
* **네트워킹**: Boost.Asio
* **빌드 시스템**: CMake
* **의존성 관리**: vcpkg
* **개발 환경**: Visual Studio 2022 (Windows)

## 📂 프로젝트 구조

```
GomokuServer/
├── .gitignore         # Git이 무시할 파일 목록
├── CMakeLists.txt     # CMake 빌드 스크립트 (프로젝트 설계도)
├── build/             # (Git에 의해 무시됨) 빌드 결과물이 생성되는 폴더
└── src/               # 모든 소스 코드가 위치하는 폴더
    ├── main.cpp       # 서버의 시작점 (main 함수, Server 클래스)
    ├── Session.h/.cpp # 클라이언트 한 명과의 연결을 담당하는 클래스
    ├── LobbyManager.h # 로비와 매치메이킹을 관리하는 싱글톤 클래스
    ├── GameRoom.h/.cpp  # 한 판의 게임을 관리하는 클래스
    ├── Packet.h       # 통신 패킷 구조 정의
    └── client.cpp     # 테스트용 콘솔 클라이언트
```

## 🚀 시작하기 (Getting Started)

이 프로젝트를 로컬 컴퓨터에서 빌드하고 실행하는 방법입니다.

### 사전 요구 사항

* Visual Studio 2022 (C++ 데스크톱 개발 워크로드 설치)
* [CMake](https://cmake.org/download/)
* [Git](https://git-scm.com/downloads/)
* [vcpkg](https://github.com/microsoft/vcpkg)

### 빌드 순서

1.  **vcpkg로 라이브러리 설치**
    `CMakeLists.txt`에 명시된 `asio`, `system`, `algorithm` 컴포넌트를 설치합니다.
    ```bash
    vcpkg install boost-asio boost-system boost-algorithm:x64-windows
    ```

2.  **프로젝트 클론 및 빌드 파일 생성**

    ```bash
    # 1. 이 저장소를 클론합니다.
    git clone [Your-GitHub-Repo-URL]
    cd [Your-Repo-Folder]/GomokuServer

    # 2. 빌드 폴더를 생성하고 들어갑니다.
    mkdir build
    cd build

    # 3. CMake를 실행하여 Visual Studio 솔루션 파일을 생성합니다.
    # [vcpkg_root_path] 부분은 자신의 vcpkg 설치 경로로 변경해주세요.
    # 예: C:/dev/vcpkg
    cmake .. -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root_path]/scripts/buildsystems/vcpkg.cmake
    ```

3.  **Visual Studio에서 빌드**
    * `build` 폴더에 생성된 `GomokuServer.sln` 파일을 Visual Studio로 엽니다.
    * `Ctrl+Shift+B` 단축키를 눌러 솔루션 전체를 빌드합니다.

## 🎮 실행 방법

1.  Visual Studio의 솔루션 탐색기에서 `server` 프로젝트를 마우스 오른쪽 클릭 -> `시작 프로젝트로 설정`을 선택합니다.
2.  `Ctrl+F5` (디버깅 없이 시작)를 눌러 서버를 실행합니다.
3.  서버 콘솔에 `Server started on port 12345...` 메시지를 확인합니다.
4.  다시 솔루션 탐색기에서 `client` 프로젝트를 마우스 오른쪽 클릭 -> `시작 프로젝트로 설정`을 선택합니다.
5.  `Ctrl+F5`를 눌러 첫 번째 클라이언트를 실행합니다. 서버에 "A client entered the lobby" 로그가 나타납니다.
6.  `Ctrl+F5`를 한번 더 눌러 두 번째 클라이언트를 실행합니다.
7.  두 번째 클라이언트가 접속하면 서버에 "Matching Success!" 와 "GameRoom: Game Started!" 로그가 출력됩니다.

## 🗺️ 앞으로의 계획 (Roadmap)

* [ ] **게임 로직 구체화**: `GameRoom` 내부에 실제 오목판, 턴 관리, 승패 판정 로직 구현
* [ ] **패킷 핸들링 강화**: 게임 진행에 필요한 모든 패킷(`PLACE_STONE_NTF` 등) 처리 로직 구현
* [ ] **클라이언트 개선**: 단발성 테스트 클라이언트가 아닌, 지속적으로 서버와 통신하며 사용자 입력을 처리하는 비동기 클라이언트로 개선
* [ ] **(최종 목표) Unity 클라이언트 개발**: 그래픽 UI를 갖춘 Unity 클라이언트를 개발하여 서버와 연동
