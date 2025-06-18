# CMake 최소 요구 버전
cmake_minimum_required(VERSION 3.15)

# 프로젝트 이름과 언어 설정
project(GomokuServer CXX)

# C++17 표준을 사용하도록 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg를 통해 필요한 Boost 컴포넌트들을 찾기
find_package(Boost CONFIG REQUIRED COMPONENTS asio system algorithm)


# 'server' 실행 파일은 이제 src/main.cpp와 src/Session.cpp 두 파일을 사용하여 만듭니다.
add_executable(server
    src/main.cpp
    src/Session.cpp
)

# 'server'에 필요한 라이브러리들을 연결합니다.
target_link_libraries(server PRIVATE Boost::asio Boost::system Boost::algorithm)

# 'client' 실행 파일은 오직 src/client.cpp 만을 사용하여 만듭니다.
add_executable(client src/client.cpp)

# 'client'에 필요한 라이브러리들을 연결합니다.
target_link_libraries(client PRIVATE Boost::asio Boost::system Boost::algorithm)