#pragma once

#include <array>
#include <bit>
#include <optional>
#include <string_view>
#include <type_traits>

#include "../werelib.hpp"

namespace were::artnet {

	consteval u8 id[8] = {'A','r','t','-','N','e','t',0x00}; //Art-Net [0-8]

	// Art-Net operation codes
	enum class Op : u16 {
    Poll      				= 0x2000, // This is an ArtPoll packet, no other data is contained in this UDP packet.
    PollReply 				= 0x2100, // This is an ArtPollReply Packet. It contains device status information.
    DiagData  				= 0x2300, // Diagnostics and data logging packet.
		Command						= 0x2400, // This is an ArtCommand packet. It is used to send text based parameter commands.
		DataRequest				= 0x2700, // This is an ArtDataRequest packet. It is used to request data such as products URLs.
		DataReply 				= 0x2800, // his is an ArtDataReply packet. It is used to reply to ArtDataRequest packets.
		Dmx								= 0x5000, // This is an ArtDmx data packet. It contains zero start code DMX512 information for a single Universe.
		Nzs								= 0x5100, // This is an ArtNzs data packet. It contains non-zero start code (except RDM) DMX512 information for a single Universe.
		Sync 							= 0x5200, // This is an ArtSync data packet. It is used to force synchronous transfer of ArtDmx packets to a node’s output.
		Address 					= 0x6000, // This is an ArtAddress packet. It contains remote programming information for a Node.
		Input 						= 0x7000, // This is an ArtInput packet. It contains enable – disable data for DMX inputs.
		TodRequest 				= 0x8000, // This is an ArtTodRequest packet. It is used to request a Table of Devices (ToD) for RDM discovery.
		TodData 					= 0x8100, // This is an ArtTodData packet. It is used to send a Table of Devices (ToD) for RDM discovery.
		TodControl 				= 0x8200, // This is an ArtTodControl packet. It is used to send RDM discovery control messages.
		Rdm 							= 0x8300, // This is an ArtRdm packet. It is used to send all non discovery RDM messages.
		RdmSub 						= 0x8400, // This is an ArtRdmSub packet. It is used to send compressed, RDM Sub-Device data.
		VideoSetup 				= 0xa010, // This is an ArtVideoSetup packet. It contains video screen setup information for nodes that implement the extended video features.
		VideoPalette 			= 0xa020, // This is an ArtVideoPalette packet. It contains colour palette setup information for nodes that implement the extended video features.
		VideoData 				= 0xa040, // This is an ArtVideoData packet. It contains display data for nodes that implement the extended video features.
		MacMaster 				= 0xf000, // This packet is deprecated.
		MacSlave 					= 0xf100, // This packet is deprecated.
		FirmwareMaster 		= 0xf200, // This is an ArtFirmwareMaster packet. It is used to upload new firmware or firmware extensions to the Node.
		FirmwareReply			= 0xf300, // This is an ArtFirmwareReply packet. It is returned by the node to acknowledge receipt of an ArtFirmwareMaster packet or ArtFileTnMaster packet.
		FileTnMaster			=	0xf400, // Uploads user file to node.
		FileFnMaster			= 0xf500, // Downloads user file from node.
		FileFnReply 			= 0xf600, // Server to Node acknowledge for download packets.
		IpProg						= 0xf800, // This is an ArtIpProg packet. It is used to reprogramme the IP address and Mask of the Node.
		IpProgReply				= 0xf900, // This is an ArtIpProgReply packet. It is returned by the node to acknowledge receipt of an ArtIpProg packet.
		Media 						= 0x9000, // This is an ArtMedia packet. It is Unicast by a Media Server and acted upon by a Controller.
		MediaPatch				= 0x9100, // This is an ArtMediaPatch packet. It is Unicast by a Controller and acted upon by a Media Server.
		MediaControl			= 0x9200, // This is an ArtMediaControl packet. It is Unicast by a Controller and acted upon by a Media Server.
		MediaContrlReply	= 0x9300, // This is an ArtMediaControlReply packet. It is Unicast by a Media Server and acted upon by a Controller.
		TimeCode					= 0x9700, // This is an ArtTimeCode packet. It is used to transport time code over the network.
		TimeSync					= 0x9800, // Used to synchronise real time date and clock.
		Trigger 					= 0x9900, // Used to send trigger macros.
		Directory 				= 0x9a00, // Requests a node's file list.
		DirectoryReply 		= 0x9b00, // Replies to OpDirectory with file list.
	};

	u16 Version = 0x000E; // 1.4 or "14"

	enum class NodeFlag : u8 {
		None									= 0x00,			// no bits set
		SendPollReplyOnChange	= 1u << 1,	// Bit-1
    ReceiveDiagnostics		= 1u << 2,  // Bit-2
    UnicastDiagnostics		= 1u << 3,  // Bit-3
    DisableVLC						= 1u << 4,  // Bit-4
    EnableTargetedMode		= 1u << 5,  // Bit-5
		// Bits 6 & 7 are reserved and must remain zero
	};

	u8 DiagPriority;								// 6
	u8 TargetPortAddressTopHi;			// 7
	u8 TargetPortAddressTopHi;			// 8
	u8 TargetPortAddressTopHi;			// 9
	u8 TargetPortAddressBottomLo;		// 10
	u8 EstaManHi; 									// 11
	u8 EstaManLo; 									// 12
	u8 OemHi; 											// 13
	u8 OemLo; 											// 14
	
}