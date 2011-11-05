/***************************************************************
 * Copyright 2011, Zhang, Jun. All rights reserved.            *
 * Author: Zhang, Jun (ralph dot j dot zhang at gmail dot com) *
 *                                                             *
 * This source code is provided with absolutely no warranty.   *
 ***************************************************************/ 
#pragma once
#include "sink.hpp"
#include "file_sink.hpp"

namespace ku { namespace log {

class ConsoleSink : public FileSink
{
public:
  ConsoleSink() : FileSink(STDOUT_FILENO) { }

  virtual ~ConsoleSink() { }
};

} } // namespace ku::log

