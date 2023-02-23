#pragma once

// Network Observer
//	- manages network bandwidth based on network conditions

#include "..\NetworkCommon.h"

// enables/disables LIVE netcode, so we can use the LIVE serverlist
// true  - LIVE network protocol enabled
// false - System Link network protocol enabled

#if !defined(_CARTOGRAPHER_DLL_CONF)
#define USE_LIVE_NETWORK_PROTOCOL true

#if defined(USE_LIVE_NETWORK_PROTOCOL)
#	if USE_LIVE_NETWORK_PROTOCOL == true
#		define INCREASE_NETWORK_TICKRATE_OBSOLETE 0 // old method of incresing the packet rate, now OBSOLETE
#		define LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS 1
#	endif
#endif
#endif

// network heap size
#define k_network_preference_size 108

// default: 1048576
#define k_network_heap_size 10485760 

// defaults
#define k_online_netcode_client_rate_real 60.0f
#define k_online_netcode_server_rate_real 60.0f

#define k_online_netcode_client_max_packet_size_bytes 1264
#define k_online_netcode_server_max_packet_size_bytes 1264

// size in bits
#define k_online_netcode_client_max_bandwidth_per_channel ((int)k_online_netcode_client_rate_real * k_online_netcode_client_max_packet_size_bytes * 8)
#define k_online_netcode_server_max_bandwidth_per_channel ((int)k_online_netcode_server_rate_real * k_online_netcode_server_max_packet_size_bytes * 8)

struct s_network_observer_configuration;

struct s_qos_probe_data
{
	DWORD cProbesXmit;
	DWORD cProbesRecv;
	DWORD wRttMinInMsecs;
	DWORD wRttMedInMsecs;
	DWORD dwUpBitsPerSec;
	DWORD dwDnBitsPerSec;
	DWORD cbData;
	DWORD pbData;
};

struct __declspec(align(8)) s_observer_channel
{
	enum e_observer_channel_state : int
	{
		none = 0,
		pending_transport_layer = 3, // waiting for xnet connection to be established
		pending_game_layer = 4, // xnet layer connected, waiting for game to establish
		connected = 7
	};

	int state;
	DWORD field_4;
	BYTE observer_flags;
	BYTE field_9;
	WORD field_A;
	DWORD channel_index;
	DWORD field_10;
	XNADDR xnaddr;
	DWORD field_38;
	DWORD session_index;
	int field_40;
	DWORD field_44;
	DWORD field_48;
	DWORD field_4C;
	DWORD field_50;
	DWORD field_54;
	DWORD field_58;
	network_address address;
	DWORD qos_probe_datum;
	s_qos_probe_data qos_probe_result;
	DWORD field_94;
	DWORD field_98;
	DWORD field_9C;
	BYTE gap_A0[8];
	DWORD field_A8;
	BYTE gap_AC[212];
	DWORD field_180;
	BYTE gap_184[268];
	DWORD field_290;
	BYTE gap_294[156];
	DWORD field_330;
	BYTE gap_334[52];
	DWORD field_368;
	BYTE gap_36C[212];
	DWORD field_440;
	BYTE gap_444[236];
	DWORD field_530;
	BYTE gap_534[28];
	DWORD field_550;
	BYTE gap_554[268];
	DWORD field_660;
	DWORD field_664;
	DWORD field_668;
	DWORD field_66C;
	DWORD field_670;
	DWORD field_674;
	DWORD field_678;
	DWORD field_67C;
	DWORD field_680;
	DWORD field_684;
	DWORD field_688;
	BYTE gap_68C[4];
	__int64 field_690;
	DWORD unmanaged_stream_bandwidth;
	float net_rate_unmanaged_stream;
	DWORD unmanaged_stream_window_size;
	BYTE managed_stream;
	BYTE field_6A5;
	bool simulation_attached;
	bool simulation_authority;
	bool simulation_not_authority;
	BYTE gap_6A9[3];
	int net_managed_stream_bandwidth;
	DWORD net_managed_stream_window_size;
	float net_rate_managed_stream;
	bool field_6B8;
	char field_6B9;
	char field_6BA;
	int field_6BC;
	DWORD total_bits_received;
	DWORD field_6C4;
	DWORD field_6C8;
	DWORD field_6CC;
	DWORD field_6D0;
	DWORD field_6D4;
	bool field_6D8;
	DWORD field_6DC;
	int field_6E0;
	signed int field_6E4;
	int field_6E8;
	float field_6EC;
	DWORD field_6F0;
	DWORD field_6F4;
	DWORD field_6F8;
	DWORD field_6FC;
	DWORD field_700;
	signed int field_704;
	DWORD field_708;
	bool field_70C;
	int field_710;
	int field_714;
	float field_718;
	DWORD field_71C;
	DWORD field_720;
	BYTE field_724;
	char field_725;
	char field_726;
	char field_727;
	DWORD field_728;
	int field_72C;
	DWORD field_730;
	DWORD field_734;
	LONGLONG field_738;
};
CHECK_STRUCT_SIZE(s_observer_channel, 0x740);

struct __declspec(align(8)) s_network_observer
{
	void* network_observer_vtbl; // vtable at the start
	void* network_link;
	void* network_message_gateway;
	void* message_types;
	s_network_observer_configuration* configuration;
	int *field_14;
	BYTE gap_18[8];
	XNKID sessionId;
	BYTE gap28[32];
	BYTE field_48;
	XNKEY xnkey;
	DWORD field_5C;
	BYTE gap_60[4];
	DWORD field_64;
	BYTE gap_68[12];
	DWORD field_74;
	BYTE gap_78[8];
	s_observer_channel observer_channels[16];
	bool network_obeserver_enabled;
	char field_7481;
	int field_7484;
	DWORD throughput;
	int congestion;
	DWORD field_7490;
	bool field_7494;
	BYTE gap_7495[3];
	DWORD field_7498;
	DWORD field_749C;
	DWORD field_74A0;
	DWORD field_74A4;
	DWORD field_74A8;
	BYTE gap_74AC[4];
	float field_74B0;
	BYTE gap_74B4[212];
	int field_7588;
	DWORD field_758C;
	DWORD field_7590;
	DWORD field_7594;
	char field_7598;
	int field_759C;
	int bits_per_sec;
	float field_75A4;
	float packets_per_sec;
	int field_75AC;
	DWORD field_75B0;
	int field_75B4;
	DWORD field_75B8;
	char field_75BC;
	char field_75BD;
	char field_75BE;
	DWORD field_75C0;
	DWORD field_75C4;

	enum e_network_message_send_protocol : char
	{
		in_band,
		out_of_band
	};

	static void ApplyGamePatches();
	static void ResetNetworkPreferences();
	static void ForceConstantNetworkRate();

	bool __thiscall channel_should_send_packet_hook(int network_channel_index,
		bool a4,
		bool a5,
		int a6,
		int* out_send_sequenced_packet,
		int* out_force_fill_packet,
		int* out_packet_size,
		int* out_voice_size,
		int out_voice_chat_data_buffer_size,
		BYTE* out_voice_chat_data_buffer);

	bool __thiscall GetNetworkMeasurements(DWORD *out_throughput, float *out_satiation, DWORD *a4);
	int getObserverState(int observerIndex) { return observer_channels[observerIndex].state; };
	void sendNetworkMessage(int session_index, int observer_index, e_network_message_send_protocol send_out_of_band, int type, int size, void* data);
};
CHECK_STRUCT_SIZE(s_network_observer, 0x75C8);

struct s_network_observer_configuration
{
	DWORD field_0;
	DWORD field_4;
	BYTE gap_8[28];
	DWORD field_24;
	DWORD field_28;
	DWORD field_2C;
	DWORD field_30;
	BYTE gap_34[20];
	DWORD field_48;
	DWORD field_4C;
	DWORD field_50;
	DWORD field_54;
	DWORD field_58;
	BYTE gap_5C[16];
	DWORD field_6C;
	DWORD field_70;
	DWORD field_74;
	DWORD field_78;
	DWORD field_7C;
	DWORD field_80;
	DWORD field_84;
	DWORD field_88;
	DWORD field_8C;
	float field_90;
	float field_94;
	float field_98;
	DWORD unk_total_flt_array_elements;
	float unk_floats_A0[7];
	BYTE gap_BC[36];
	DWORD field_E0;
	float field_E4;
	float field_E8;
	float field_EC;
	float field_F0;
	DWORD field_F4;
	DWORD field_F8;
	DWORD field_FC;
	DWORD field_100;
	float field_104;
	DWORD field_108;
	DWORD field_10C;
	float field_110;
	int field_114;
	int field_118;
	float field_11C;
	int field_120;
	DWORD field_124;
	DWORD field_128;
	DWORD field_12C;
	DWORD field_130;
	DWORD field_134;
	DWORD field_138;
	DWORD field_13C;
	DWORD field_140;
	BYTE field_144;
	DWORD field_148;
	DWORD field_14C;
	DWORD field_150;
	DWORD field_154;
	DWORD max_bits_per_second_single_player;
	DWORD max_bits_per_second_full_lobby;
	DWORD max_bits_per_second_splitscreen_players;
	DWORD field_164;
	DWORD field_168;
	DWORD field_16C;
	DWORD field_170;
	DWORD field_174;
	float field_178;
	DWORD field_17C;
	float field_180;
	DWORD field_184;
	DWORD field_188;
	DWORD field_18C;
	float field_190;
	DWORD field_194;
	float field_198;
	DWORD field_19C;
	DWORD field_1A0;
	DWORD field_1A4;
	DWORD field_1A8;
	float field_1AC;
	DWORD field_1B0;
	DWORD field_1B4;
	DWORD field_1B8;
	DWORD field_1BC;
	DWORD field_1C0;
	float field_1C4;
	float field_1C8;
	DWORD field_1CC;
	float field_1D0;
	DWORD field_1D4;
	DWORD field_1D8;
	DWORD field_1DC;
	DWORD field_1E0;
	DWORD field_1E4;
	DWORD field_1E8;
	DWORD field_1EC;
	DWORD field_1F0;
	DWORD field_1F4;
	DWORD field_1F8;
	float field_1FC;
	DWORD field_200;
};
CHECK_STRUCT_SIZE(s_network_observer_configuration, 0x204);
