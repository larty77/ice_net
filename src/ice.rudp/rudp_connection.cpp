#include "rudp_connection.h"

rudp_connection::rudp_connection(
	SERV_C_H sch,
	SERV_C_S scs,
	SERV_C_D scd,
	SERV_P_L rpl)
{
	this->serv_callback_handle = sch;
	this->serv_callback_send = scs;
	this->serv_callback_disconnect = scd;
	this->serv_reliable_packet_lost = rpl;
}

end_point rudp_connection::get_remote_point()
{
	return remote_point;
}

const end_point* rudp_connection::get_remote_point_ptr()
{
	return &remote_point;
}

void rudp_connection::update()
{
	if (!scheduler.empty()) scheduler.execute();
}

void rudp_connection::connect(end_point remote_point)
{
	this->current_state = connected;

	this->remote_point = remote_point;

	rudp_peer::rudp_init();
}

void rudp_connection::handle(ice_data::read& data)
{
	if (current_state == disconnected) return;

	char packet_id = data.get_flag();

	switch (packet_id)
	{

	case rudp::headers_server::s_heartbeat_request:
		handle_heartbeat_request();
		break;

	case rudp::headers_server::s_heartbeat_response:
		handle_heartbeat_response();
		break;

	case rudp::headers_server::s_unreliable:
		handle_unreliable(data);
		break;

	case rudp::headers_server::s_reliable:
		handle_reliable(data);
		break;

	case rudp::headers_server::s_ack:
		handle_ack(data);
		break;

	default:
		break;

	}
}

void rudp_connection::ch_handle(ice_data::read& data)
{
	serv_callback_handle(*this, data);
}

void rudp_connection::ch_send(ice_data::write& data)
{
	serv_callback_send(remote_point, data);
}

void rudp_connection::ch_reliable_packet_lost(char* data, unsigned short size, unsigned short id)
{
	serv_reliable_packet_lost(*this, data, size, id);
}

void rudp_connection::disconnect(bool notify)
{
	if (current_state == disconnected) return;

	rudp_peer::rudp_stop();

	serv_callback_disconnect(remote_point, notify);
}

inline char rudp_connection::_flag_heartbeat_request()
{
	return rudp::headers_client::c_heartbeat_request;
}

inline char rudp_connection::_flag_heartbeat_response()
{
	return rudp::headers_client::c_heartbeat_response;
}

inline char rudp_connection::_flag_unreliable()
{
	return rudp::headers_client::c_unreliable;
}

inline char rudp_connection::_flag_reliable()
{
	return rudp::headers_client::c_reliable;
}

inline char rudp_connection::_flag_ack()
{
	return rudp::headers_client::c_ack;
}