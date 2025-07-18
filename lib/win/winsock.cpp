#include "./winsock.hpp"

namespace were::win {
	winsock::winsock(int type, u16 port) {
		if (WSAStartup(0x0202, &wsaData_) != 0) {
			throw std::runtime_error("Winsock 2.2 initialization failed.");
		}
		openSocket(type, port);
	}

	winsock::~winsock() {
		closeSocket();
		WSACleanup();
	}

	void openSocket(int type, u16 port) {
		// Setup a socket that is internetwork: UDP, TCP, etc & datagram socket
		SOCKET sock = socket(IP, type, 0);
		if (sock == INVALID_SOCKET) {
			throw std::runtime_error("Failed to create socket.");
		}

		// Initialize a socketaddr and set some data
		sockaddr_in localAddr{};
		localAddr.sin_family = AF_INET;
		localAddr.sin_port = bigEndianSwap(port);
		localAddr.sin_addr.s_addr = INADDR_ANY;

		// Bind socket to port
		if (bind(sock, asraw<sockaddr*>(&localAddr), sizeof(localAddr)) == SOCKET_ERROR) {
			int err = WSAGetLastError();
			closesocket(sock);
			throw std::runtime_error(std::format("Error binding socket. Code: {}", err));
		}
	}

	void winsock::closeSocket() noexcept {
		if (sock_ != INVALID_SOCKET) {
			closesocket(sock_);
			sock_ = INVALID_SOCKET;
		}
	}

}