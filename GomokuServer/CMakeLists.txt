# CMake 최소 요구 버전
cmake_minimum_required(VERSION 3.15)

# 프로젝트 이름과 언어 설정
project(GomokuServer CXX)

# C++17 표준을 사용하도록 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg를 통해 필요한 Boost 컴포넌트들을 찾기 (CONFIG 모드 사용)
find_package(Boost CONFIG REQUIRED COMPONENTS asio system algorithm)

# 'server'라는 이름의 실행 파일 생성 (소스는 src/main.cpp)
add_executable(server src/main.cpp)

# 'server' 실행 파일에 필요한 Boost 라이브러리들을 각각 연결
target_link_libraries(server PRIVATE Boost::asio Boost::system Boost::algorithm)