/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_NETWORK_CONNECTOR_HPP
#define LIBBITCOIN_NETWORK_CONNECTOR_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <bitcoin/system.hpp>
#include <bitcoin/network/channel.hpp>
#include <bitcoin/network/define.hpp>
#include <bitcoin/network/settings.hpp>

namespace libbitcoin {
namespace network {

/// Create outbound socket connections.
/// This class is thread safe against stop.
/// This class is not safe for concurrent connection attempts.
class BCT_API connector
  : public system::enable_shared_from_base<connector>, system::noncopyable,
    track<connector>
{
public:
    typedef std::shared_ptr<connector> ptr;
    typedef std::function<void(const system::code& ec, channel::ptr)>
        connect_handler;

    /// Construct an instance.
    connector(system::threadpool& pool, const settings& settings);

    /// Validate connector stopped.
    ~connector();

    /// Try to connect to the endpoint.
    virtual void connect(const system::config::endpoint& endpoint,
        connect_handler handler);

    /// Try to connect to the authority.
    virtual void connect(const system::config::authority& authority,
        connect_handler handler);

    /// Try to connect to host:port.
    virtual void connect(const std::string& hostname, uint16_t port,
        connect_handler handler);

    /// Cancel outstanding connection attempt.
    void stop(const system::code& ec);

private:
    typedef std::shared_ptr<system::asio::query> query_ptr;

    bool stopped() const;

    void handle_resolve(const system::boost_code& ec,
        system::asio::iterator iterator, connect_handler handler);
    void handle_connect(const system::boost_code& ec,
        system::asio::iterator iterator, system::socket::ptr socket,
        connect_handler handler);
    void handle_timer(const system::code& ec, system::socket::ptr socket,
        connect_handler handler);

    // These are thread safe
    std::atomic<bool> stopped_;
    system::threadpool& pool_;
    const settings& settings_;
    mutable system::dispatcher dispatch_;

    // These are protected by mutex.
    query_ptr query_;
    system::deadline::ptr timer_;
    system::asio::resolver resolver_;
    mutable system::upgrade_mutex mutex_;
};

} // namespace network
} // namespace libbitcoin

#endif
