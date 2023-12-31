#include "rudp_peer.h"

rudp_peer::~rudp_peer()
{
	rudp_reset();
}

#pragma region calculations

void rudp_peer::set_rtt(unsigned long long rtt)
{
	this->rtt = static_cast<unsigned short>(rtt > 0xFFFF ? 0xFFFF : rtt);
}

unsigned short rudp_peer::get_smooth_rtt()
{
	return smooth_rtt = static_cast<unsigned short>((0.2 * rtt) + (0.8 * smooth_rtt));
}

unsigned short rudp_peer::get_resend_time() const
{
	return
		smooth_rtt * 2 < rudp::max_resend_time ?
		(rudp::min_resend_time > smooth_rtt * 2 ? rudp::min_resend_time : smooth_rtt * 2) :
		rudp::max_resend_time;
}

unsigned short rudp_peer::get_next_packet_id()
{
	return last_packet_id =
		(last_packet_id == 65535) ? 1 :
		(++last_packet_id);
}

unsigned short rudp_peer::get_ping()
{
	return get_smooth_rtt() / 2;
}

#pragma endregion

void rudp_peer::rudp_init()
{
	rudp_reset();

	start_heartbeat_timer();
	start_disconnect_timer();
}

void rudp_peer::rudp_stop()
{
	rudp_reset();

	std::unique_lock<std::shared_mutex> w_lock(mutex);

	current_state = disconnected;
}

void rudp_peer::rudp_reset()
{
	std::unique_lock<std::shared_mutex> w_lock(mutex);

	for (auto& it : pending_packets) delete it.second;
	pending_packets.clear();

	scheduler.clear();

}

void rudp_peer::handle_heartbeat_request()
{
	send_heartbeat_response();
}

void rudp_peer::handle_heartbeat_response()
{
	rtt_watch.stop();
	set_rtt(rtt_watch.get_elapsed_milliseconds());

	stop_disconnect_timer();
	start_disconnect_timer();
}

void rudp_peer::handle_unreliable(ice_data::read& data)
{
	ch_handle(data);
}

void rudp_peer::handle_reliable(ice_data::read& data)
{
	unsigned short packet_id = data.get_int16();
	if (packet_id < 1) return;
	send_ack(packet_id);
	ch_handle(data);
}

void rudp_peer::handle_ack(ice_data::read& data)
{
	std::shared_lock<std::shared_mutex> r_lock(mutex);

	unsigned short packet_id = data.get_int16();

	if (packet_id < 1) return;

	auto pair = pending_packets.find(packet_id);

	if (pair == pending_packets.end()) return;

	r_lock.unlock();

	auto packet = pair->second;

	std::unique_lock<std::shared_mutex> w_lock(mutex);

	scheduler.remove(packet->element);

	pending_packets.erase(pair);

	w_lock.unlock();

	delete packet;
}

void rudp_peer::send_heartbeat_request()
{
	ice_data::write data(1);
	data.set_flag((char)rudp::heartbeat_request);

	ch_send(data);

	rtt_watch.start();
}

void rudp_peer::send_heartbeat_response()
{
	ice_data::write data(1);
	data.set_flag((char)rudp::heartbeat_response);
	ch_send(data);
}

void rudp_peer::send_unreliable(ice_data::write& data)
{
	std::shared_lock<std::shared_mutex> r_lock(mutex);

	if (current_state != connected) return;

	r_lock.unlock();

	data.set_flag(rudp::unreliable);
	ch_send(data);
}

void rudp_peer::send_reliable(ice_data::write& data)
{
	std::shared_lock<std::shared_mutex> r_lock(mutex);

	if (current_state != connected) return;

	r_lock.unlock();

	char* reliable_arr = new char[data.get_buffer_size() - 1];
	memcpy(reliable_arr, data.get_buffer() + 1, data.get_buffer_size() - 1);

	unsigned short packet_id = get_next_packet_id();

	ice_data::write* reliable_data = new ice_data::write(data.get_buffer_size() - 1);

	reliable_data->set_flag(rudp::reliable);

	reliable_data->add_int16(packet_id);

	reliable_data->add_buffer(reliable_arr, data.get_buffer_size());

	pending_packet* packet = new pending_packet;

	std::unique_lock<std::shared_mutex> w_lock(mutex);

	pending_packets.insert(std::make_pair(packet_id, packet));

	w_lock.unlock();

	packet->data = reliable_data;

	packet->packet_id = packet_id;

	send_reliable_attempt(packet_id);
}

void rudp_peer::send_reliable_attempt(int packet_id)
{
	std::shared_lock<std::shared_mutex> r_lock(mutex);

	if (current_state != connected) return;

	auto pair = pending_packets.find(packet_id);

	if (pair == pending_packets.end()) return;

	r_lock.unlock();

	auto packet = pair->second;

	if (packet->attempts >= rudp::max_resend_count)
	{
		ice_logger::log("reability", ("reliable packet[" + std::to_string(packet->packet_id) + "] was not handled!"));

		std::unique_lock<std::shared_mutex> w_lock(mutex);

		pending_packets.erase(pair);

		w_lock.unlock();

		delete packet->data;

		delete packet;

		return;
	}

	std::unique_lock<std::shared_mutex> w_lock(mutex);

	++packet->attempts;

	packet->element = scheduler.add([this, packet_id]() { send_reliable_attempt(packet_id); }, get_resend_time());

	w_lock.unlock();

	ch_send(*packet->data);
}

void rudp_peer::send_ack(unsigned short packet_id)
{
	ice_data::write data(3);
	data.set_flag(rudp::ack);
	data.add_int16(packet_id);

	ch_send(data);
}

void rudp_peer::start_heartbeat_timer()
{
	send_heartbeat_request();

	heartbeat_element = scheduler.add([this]() { start_heartbeat_timer(); }, rudp::heartbeat_interval);
}

void rudp_peer::start_disconnect_timer()
{
	disconnect_element = scheduler.add([this]() { disconnect(); }, rudp::disconnect_timeout);
}

void rudp_peer::stop_disconnect_timer()
{
	scheduler.remove(disconnect_element);
}