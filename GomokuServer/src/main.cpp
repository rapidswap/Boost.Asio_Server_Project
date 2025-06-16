#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>


#include "Packet.h"
using namespace boost::asio;
using ip::tcp;

// shared_from_this를 상속받아 비동기 작업 중에도 객체의 생존을 보장
class Session :public std::enable_shared_from_this<Session> {
public:
	explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

	void start() {
		do_read_header();
	}

private:
	void do_read_header() {
		auto self = shared_from_this();

		async_read(socket_,buffer(reinterpret_cast<char*>(&receivedHeader_), HEADER_SIZE),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					do_read_body();
				}
				else {
					//에러처리
				}
			});
	}

	void do_read_body() {
		auto self = shared_from_this();

		size_t bodySize = receivedHeader_.size - HEADER_SIZE;
		receivedBody_.resize(bodySize);

		async_read(socket_, buffer(receivedBody_.data(), bodySize),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					std::cout << "Packet Received! ID: " << receivedHeader_.id << ", Size: " << receivedHeader_.size << '\n';
					std::string bodyStr(receivedBody_.begin(), receivedBody_.end());
					std::cout << "Body: " << bodyStr << '\n';

					do_read_header(); // 다음 패킷을 받기 위한 헤더읽기
				}
				else {
					//에러처리
				}
			});
	}

	void do_write(std::size_t length) {
		auto self = shared_from_this();
		//비동기적으로 데이터 쓰기

		async_write(socket_, buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
			if (!ec) {
				//do_read();
			}
		});
	}

	tcp::socket socket_; //클라이언트와 연결된 소켓

	PacketHeader receivedHeader_;
	std::vector<char>receivedBody_;
	enum{max_length = 1024};
	char data_[max_length];
};

class Server {
public:
	//생성자에서 io_context와 포트 번호 받기
	Server(io_context& io_context, short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
		//서버 시작시 바로 접속 받기 시작
		do_accept();
	}

private:
	void do_accept() {
		//비동기적 접속 기다림
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket) {
				if (!ec) {
					//접속 성공 시, 새로운 Session 만들어 통신
					std::make_shared<Session>(std::move(socket))->start();
				}
				//바로 다음 접속 기다리기
				do_accept();
			});
	}

	tcp::acceptor acceptor_;
};

int main() {
	try {
		io_context io_context;

		Server s(io_context, 12345);
		std::cout << "Server started on port 12345...\n";

		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exxception: " << e.what() << '\n';
	}

	return 0;
}