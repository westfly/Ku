/***************************************************************
 * Copyright 2011, Zhang, Jun. All rights reserved.            *
 * Author: Zhang, Jun (ralph dot j dot zhang at gmail dot com) *
 *                                                             *
 * This source code is provided with absolutely no warranty.   *
 ***************************************************************/ 
#include <system_error>
#include "util.hpp"
#include "endpoint.hpp"
#include "channel.hpp"

namespace ku { namespace fusion {

void Channel::bind(char const* endpoint)
{
  Endpoint ep(endpoint);
  if (ep.protocol() == Protocol::Invalid)
    throw std::system_error(util::errc(EINVAL), "Channel::bind");
  switch (ep.protocol()) {
  case Protocol::Inproc:
    // TODO UserEventEndpoint
    break;
  default:
    // TODO SocketEndpoint
    break;
  }
  // TODO for Pub/Rep, use acceptor, for Sub/Req, use connector
}

} } // namespace ku::fusion

