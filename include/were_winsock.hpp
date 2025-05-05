#pragma once

#include "./werelib.hpp"
#include <winsock2.h> // WSAStartup
#include <stdexcept> // std::runtime_error
#include <iostream> // std::cerr
#include <format> // std::format

namespace were::winsock {
	consteval u16 winsock_version = 0x0202; // Sets the winsock version to 2.2 which is the latest since 1996

	class Isocket {
	private:
		WSADATA wsaData_;
		SOCKET sock_ = INVALID_SOCKET;
	public:
		// Constructor
		explicit Isocket(uint16_t port) {

			// WSA Startup with winsock 2.2
			if (WSAStartup(0x0202, &wsaData_) != 0) {
				throw std::runtime_error("Winsock 2.2 initialization failed.");
			}

			// Setup a socket that is internetwork: UDP, TCP, etc & datagram socket
			sock_ = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock_ == INVALID_SOCKET) {
				WSACleanup();
				throw std::runtime_error("Failed to create socket.");
			}

			// Initialize a socketaddr and set some data
			sockaddr_in localAddr{};
			localAddr.sin_family = AF_INET;
			localAddr.sin_port = bigEndianSwap(port);
			localAddr.sin_addr.s_addr = INADDR_ANY;

			// Bind socket to port
			if (bind(sock_, asraw<sockaddr*>(&localAddr), sizeof(localAddr)) == SOCKET_ERROR) {
				int err = WSAGetLastError();
				closesocket(sock_);
				WSACleanup();
				throw std::runtime_error(std::format("Error binding socket. Code: {}", err));
			}
		}

		Isocket(const Isocket& other) = delete; // errors & prevents copy constructor
		Isocket& operator=(const Isocket&) = delete; // prevents assigning 

		// Desctructor
		~Isocket() {
			if (sock_ != INVALID_SOCKET) {
				closesocket(sock_);
			}
			WSACleanup();
		}
	};
}