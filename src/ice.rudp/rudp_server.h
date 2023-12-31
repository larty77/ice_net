#pragma once

#ifndef ICE_RUDP_SERV
#define ICE_RUDP_SERV

#include "../ice.core/ice_data.h"
#include "../ice.core/ice_logger.h"

#include "common/transport/a_server.h"
#include "common/rudp.h"

#include "rudp_connection.h"

#include <vector>
#include <functional>
#include <shared_mutex>

class rudp_server final
{

private:

	enum server_state
	{
		disconnected,
		connected,
	};

	server_state current_state = disconnected;

private:

	a_server* socket;

private:

	std::map<end_point, rudp_connection*> connections;

	std::vector<rudp_connection*> connections_arr;

public:

	std::function<bool(end_point&)> predicate_add_connection;

public:

	std::function<void(rudp_connection&)> connection_added_callback;

	std::function<void(rudp_connection&)> connection_removed_callback;

public:

	std::function<void(rudp_connection&, ice_data::read&)> external_data_callback;

private:

	std::shared_mutex mutex;

public:

	end_point get_local_point();

public:

	void update();

public:

	template<class T>
	bool try_start(end_point local_point);

private:

	void receive();

private:

	bool try_get_connection(rudp_connection*& connection, end_point& remote_point);

	bool try_add_connection(rudp_connection*& connection, end_point& remote_point);

	bool try_remove_connection(end_point& remote_point);

private:

	void connection_callback_handle(rudp_connection& connection, ice_data::read& data);

	void connection_callback_send(end_point& remote_point, ice_data::write& data);

	void connection_callback_disconnect(end_point& remote_point);

private:

	inline void ext_connection_added(rudp_connection& c);

	inline void ext_connection_removwd(rudp_connection& c);

	inline void ext_data_handled(rudp_connection& c, ice_data::read& d);

private:

	void send(end_point& remote_point, ice_data::write& data);

public:

	void stop();
};

#endif