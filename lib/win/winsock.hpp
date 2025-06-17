#pragma once

#include <winsock2.h> // WSAStartup
#include <stdexcept> // std::runtime_error
#include <iostream> // std::cerr
#include <format> // std::format
#include <unordered_map> // std::unordered_map

#include "./werelib.hpp"

// Windows socket
namespace were::winsock {
	consteval u16 winsock_version = 0x0202; // Sets the winsock version to 2.2 which is the latest since 1996
	consteval int IP = AF_INET; // 2 : internetwork: UDP, TCP, etc.

	// Socket types for winsock 2.2
	consteval int TCP = SOCK_STREAM;		// 1 : stream socket
	consteval int UDP = SOCK_DGRAM;			// 2 : datagram socket
	consteval int RAW = SOCK_RAW;				// 3 : raw-protocol interface
	consteval int RDM = SOCK_RDM; 			// 4 : reliably-delivered message
	consteval int SEQ = SOCK_SEQPACKET;	// 5: sequenced packet stream

	class api {
	private:
		WSADATA wsaData_{};
		std::unordered_map<int, SOCKET> sockets_;
		int next_id_ = 1;

	public:
		// Constructor -- winsock 2.2
		api() {
			if (WSAStartup(0x0202, &wsaData_) != 0) {
				throw std::runtime_error("Winsock 2.2 initialization failed.");
			}
		}

		// prevent copying and assigning 
		api(const api& other) = delete;
		api& operator=(const api&) = delete; 

		// Desctructor
		~api() {
			for (auto& [_, sock] : sockets_) {
				if (sock != INVALID_SOCKET) {
					closesocket(sock);
				}
			}
			WSACleanup();
		}

		int openSocket(int type, u16 port) {
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

			int id = next_id_++;
			sockets_[id] = sock;
			return id;
		}

		void closeSocket(int id) {
			auto it = sockets_.find(id);
			if (it != sockets_.end()) {
				if (it->second != INVALID_SOCKET) {
					closesocket(it->second);
				}
				sockets_.erase(it);
			}
		}

		SOCKET getSocket(int id) const {
			auto it = sockets_.find(id);
			if (it != sockets_.end()) {
				return it->second;
			}
			throw std::out_of_range("Invalid socket ID");
		}
	};
}