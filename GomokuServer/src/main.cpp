#include <iostream>
#include <memory>
#include <boost/asio.hpp>

#include "Session.h"
#include "LobbyManager.h"

using namespace boost::asio;
using ip::tcp;

// Session 클래스 정의를 완전히 삭제했습니다. Server 클래스와 main 함수만 남깁니다.

class Server {
public:
	Server(io_context& io_context, short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
		do_accept();
	}

private:
	void do_accept() {
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket) {
				if (!ec) {
					// Session 객체를 생성하고 start()를 호출합니다.
					std::make_shared<Session>(std::move(socket))->start();
				}
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
		std::cerr << "Exception: " << e.what() << '\n';
	}
	return 0;
}