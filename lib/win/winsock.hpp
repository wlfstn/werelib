#pragma once

#include <winsock2.h> // WSAStartup
#include <stdexcept> // std::runtime_error
#include <iostream> // std::cerr
#include <format> // std::format
#include <unordered_map> // std::unordered_map

#include "./werelib.hpp"

// Windows Networking (Winsock 2.2)
namespace were::win {
	constexpr u16 winsock_version = 0x0202; // Sets the winsock version to 2.2 which is the latest since 1996
	constexpr int IP = AF_INET; // 2 : internetwork: UDP, TCP, etc.

	// Socket types for winsock 2.2
	enum class sock_type : int {
		TCP = SOCK_STREAM,		// 1 : stream socket
		UDP = SOCK_DGRAM,			// 2 : datagram socket
		RAW = SOCK_RAW,				// 3 : raw-protocol interface
		RDM = SOCK_RDM, 			// 4 : reliably-delivered message
		SEQ = SOCK_SEQPACKET,	// 5: sequenced packet stream
	};

	class winsock {
	public:
		winsock(int type, u16 port);
		winsock(const winsock& other) = delete; //prevents copying
		winsock& operator=(const winsock&) = delete; //prevents assigning
		~winsock();

		private:
		void openSocket(int type, u16 port);
		void closeSocket();
		WSADATA wsaData_{};
		SOCKET sock_ = INVALID_SOCKET;
	};
}