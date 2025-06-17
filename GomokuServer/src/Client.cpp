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

		std::cout << "Connected to server. Sending MATCH_REQ...\n";

		PacketHeader header;
		header.id = static_cast<uint16_t>(PacketID::MATCH_REQ);
		header.size = HEADER_SIZE;

		write(socket, buffer(reinterpret_cast<const char*>(&header), HEADER_SIZE));

		std::cout << "Packet sent! Waiting...\n";
		std::this_thread::sleep_for(std::chrono::seconds(10));

	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << '\n';
	}
	return 0;
}