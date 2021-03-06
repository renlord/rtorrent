// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#include <torrent/bitfield.h>
#include <torrent/peer/connection_list.h>
#include <torrent/peer/peer.h>
#include <torrent/peer/peer_info.h>
#include <torrent/rate.h>
#include <torrent/utils/error_number.h>
#include <torrent/utils/path.h>
#include <torrent/utils/socket_address.h>
#include <torrent/utils/string_manip.h>

#include "core/manager.h"
#include "display/utils.h"

#include "command_helpers.h"
#include "control.h"
#include "globals.h"

torrent::Object
retrieve_p_id(torrent::Peer* peer) {
  const torrent::HashString* hashString = &peer->id();

  return torrent::utils::transform_hex(hashString->begin(), hashString->end());
}

torrent::Object
retrieve_p_id_html(torrent::Peer* peer) {
  const torrent::HashString* hashString = &peer->id();

  return torrent::utils::copy_escape_html(hashString->begin(),
                                          hashString->end());
}

torrent::Object
retrieve_p_address(torrent::Peer* peer) {
  const torrent::utils::socket_address* addr =
    torrent::utils::socket_address::cast_from(
      peer->peer_info()->socket_address());

  if (addr->family() == torrent::utils::socket_address::af_inet6)
    return "[" + addr->address_str() + "]";
  else
    return addr->address_str();
}

torrent::Object
retrieve_p_port(torrent::Peer* peer) {
  return torrent::utils::socket_address::cast_from(
           peer->peer_info()->socket_address())
    ->port();
}

torrent::Object
retrieve_p_client_version(torrent::Peer* peer) {
  char buf[128];
  display::print_client_version(
    buf, buf + 128, peer->peer_info()->client_info());

  return std::string(buf);
}

torrent::Object
retrieve_p_options_str(torrent::Peer* peer) {
  return torrent::utils::transform_hex(peer->peer_info()->options(),
                                       peer->peer_info()->options() + 8);
}

torrent::Object
retrieve_p_completed_percent(torrent::Peer* peer) {
  return (100 * peer->bitfield()->size_set()) / peer->bitfield()->size_bits();
}

void
initialize_command_peer() {
  CMD2_PEER("p.id", std::bind(&retrieve_p_id, std::placeholders::_1));
  CMD2_PEER("p.id_html", std::bind(&retrieve_p_id_html, std::placeholders::_1));
  CMD2_PEER("p.client_version",
            std::bind(&retrieve_p_client_version, std::placeholders::_1));

  CMD2_PEER("p.options_str",
            std::bind(&retrieve_p_options_str, std::placeholders::_1));

  CMD2_PEER("p.is_encrypted",
            std::bind(&torrent::Peer::is_encrypted, std::placeholders::_1));
  CMD2_PEER("p.is_incoming",
            std::bind(&torrent::Peer::is_incoming, std::placeholders::_1));
  CMD2_PEER("p.is_obfuscated",
            std::bind(&torrent::Peer::is_obfuscated, std::placeholders::_1));
  CMD2_PEER("p.is_snubbed",
            std::bind(&torrent::Peer::is_snubbed, std::placeholders::_1));

  CMD2_PEER(
    "p.is_unwanted",
    std::bind(&torrent::PeerInfo::is_unwanted,
              std::bind(&torrent::Peer::peer_info, std::placeholders::_1)));
  CMD2_PEER(
    "p.is_preferred",
    std::bind(&torrent::PeerInfo::is_preferred,
              std::bind(&torrent::Peer::peer_info, std::placeholders::_1)));

  CMD2_PEER("p.address", std::bind(&retrieve_p_address, std::placeholders::_1));
  CMD2_PEER("p.port", std::bind(&retrieve_p_port, std::placeholders::_1));

  CMD2_PEER("p.completed_percent",
            std::bind(&retrieve_p_completed_percent, std::placeholders::_1));

  CMD2_PEER(
    "p.up_rate",
    std::bind(&torrent::Rate::rate,
              std::bind(&torrent::Peer::up_rate, std::placeholders::_1)));
  CMD2_PEER(
    "p.up_total",
    std::bind(&torrent::Rate::total,
              std::bind(&torrent::Peer::up_rate, std::placeholders::_1)));
  CMD2_PEER(
    "p.down_rate",
    std::bind(&torrent::Rate::rate,
              std::bind(&torrent::Peer::down_rate, std::placeholders::_1)));
  CMD2_PEER(
    "p.down_total",
    std::bind(&torrent::Rate::total,
              std::bind(&torrent::Peer::down_rate, std::placeholders::_1)));
  CMD2_PEER(
    "p.peer_rate",
    std::bind(&torrent::Rate::rate,
              std::bind(&torrent::Peer::peer_rate, std::placeholders::_1)));
  CMD2_PEER(
    "p.peer_total",
    std::bind(&torrent::Rate::total,
              std::bind(&torrent::Peer::peer_rate, std::placeholders::_1)));

  CMD2_PEER("p.snubbed",
            std::bind(&torrent::Peer::is_snubbed, std::placeholders::_1));
  CMD2_PEER_VALUE_V("p.snubbed.set",
                    std::bind(&torrent::Peer::set_snubbed,
                              std::placeholders::_1,
                              std::placeholders::_2));
  CMD2_PEER("p.banned",
            std::bind(&torrent::Peer::is_banned, std::placeholders::_1));
  CMD2_PEER_VALUE_V("p.banned.set",
                    std::bind(&torrent::Peer::set_banned,
                              std::placeholders::_1,
                              std::placeholders::_2));

  CMD2_PEER_V("p.disconnect",
              std::bind(&torrent::Peer::disconnect, std::placeholders::_1, 0));
  CMD2_PEER_V("p.disconnect_delayed",
              std::bind(&torrent::Peer::disconnect,
                        std::placeholders::_1,
                        torrent::ConnectionList::disconnect_delayed));
}
