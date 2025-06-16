#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "Packet.h"

using namespace boost::asio;
using ip::tcp;

int main() {
	try {
		io_context io_context;
		tcp::socket socket(io_context);
		socket.connect(tcp::endpoint(ip::make_address("127.0.0.1"), 12345));

		std::cout << "Endter message: ";
		std::string message;
		std::getline(std::cin, message);

		// 보낼 패킷 생성
		PacketHeader header;
		header.id = 101;
		header.size = HEADER_SIZE+message.length();

		write(socket, buffer(reinterpret_cast<const char*>(&header), HEADER_SIZE));

		write(socket, buffer(message));

		std::cout << "Packet sent!" << '\n';

	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << '\n';
	}
	return 0;
}