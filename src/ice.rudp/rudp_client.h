#pragma once

#ifndef ICE_RUDP_CLIENT
#define ICE_RUDP_CLIENT

#include "../ice.core/ice_data.h"
#include "../ice.core/ice_logger.h"

#include "common/transport/a_sock.h"
#include "common/rudp_peer.h"
#include "common/rudp.h"
	
#include <functional>
#include <map>
#include <string>

class rudp_client final : public rudp_peer
{

public:

	rudp_client();

public:

	a_sock* socket = nullptr;

public:

	std::function<void()> connected_callback;

	std::function<void()> disconnected_callback;

public:

	std::function<void(ice_data::read&)> external_data_callback;

public:

	std::function<void(char*, unsigned short, unsigned short)> reliable_packet_lost;

private:

	const int max_connection_attempts = 3;

	const int connection_timeout = 1500;

	int connection_attempts = 0;

private:

	scheduler::element* connect_element;

public:

	end_point get_local_point();

	end_point get_remote_point();

private:

	end_point remote_point = end_point(0, 0);

public:

	void update();

public:

	void connect(end_point remote_point, end_point local_point, bool sockInit = true);

	void connect_attempt();

private:

	void receive();

private:

	void handle(ice_data::read& data);

private:

	void handle_connect_response();

private:

	void ch_handle(ice_data::read& data) override;

	void ch_send(ice_data::write& data) override;

	void ch_reliable_packet_lost(char* data, unsigned short size, unsigned short id) override;

private:

	void send_connect_confirm();

public:

	void send_unreliable(ice_data::write& data);

	void send_reliable(ice_data::write& data);

private:

	void send_connect_request();

public:

	void disconnect() override;

private:

	inline char _flag_heartbeat_request();

	inline char _flag_heartbeat_response();

	inline char _flag_unreliable();

	inline char _flag_reliable();

	inline char _flag_ack();
};

#endif