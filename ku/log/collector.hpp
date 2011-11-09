/***************************************************************
 * Copyright 2011, Zhang, Jun. All rights reserved.            *
 * Author: Zhang, Jun (ralph dot j dot zhang at gmail dot com) *
 *                                                             *
 * This source code is provided with absolutely no warranty.   *
 ***************************************************************/ 
#pragma once
#include <cstring>
#include <string>
#include <type_traits>
#include "util.hpp"
#include "log_level.hpp"
#include "message.hpp"
#include "buffer_queue.hpp"

namespace ku { namespace log {

class Logger;

// =======================================================================================
// Collector is the logger front end.
// It collects log data to a buffer, and submit for further processing.
// =======================================================================================
class Collector : private util::noncopyable
{
  friend class Logger;

public:
  Collector() = delete;
  ~Collector();

  void append(char c) { message_.append(c); }
  void append(std::string const& s) { message_.append(s.c_str(), s.size()); }
  void append(char const* s, size_t size) { message_.append(s, size); }

  Collector& initialize(LogLevel log_level);
  void set_format(char const* fmt) { format_ = fmt; }

  // C printf style collecting support
  template <typename... Args>
  Collector& operator () (char const* fmt, Args... args);

private:
  Collector(BufferQueue& free_queue, Logger& logger)
    : logger_(logger), message_(free_queue), format_(nullptr)
  { }

  Collector(Collector&& col)
    : logger_(col.logger_), message_(std::move(col.message_)), format_(col.format_)
  { }

private:
  Logger& logger_;
  Message message_;
  char const* format_;
};

// =======================================================================================
// C++ stream style collecting support.
// User can support custome type collecting by providing to_str() function for the type,
// or if doing so being more efficient, provide custom opeartor <<
// =======================================================================================
template <typename T>
Collector& operator << (Collector& c, T const& t)
{
  c.append(to_log(t));
  return c;
}

inline Collector& operator << (Collector& c, char ch)
{
  c.append(ch);
  return c;
}

inline Collector& operator << (Collector& c, char const* s)
{
  c.append(s, std::strlen(s));
  return c;
}

inline Collector& operator << (Collector& c, std::string const& s)
{
  c.append(s);
  return c;
}

template <typename T>
auto operator << (Collector& c, T t)
  -> typename std::enable_if<std::is_unsigned<T>::value, Collector&>::type
{
  char buf[32];
  size_t len = sprintf(buf, "%llu", util::implicit_cast<unsigned long long>(t));
  c.append(buf, len);
  return c;
}

template <typename T>
auto operator << (Collector& c, T t)
  -> typename std::enable_if<std::is_signed<T>::value, Collector&>::type
{
  char buf[32];
  size_t len = sprintf(buf, "%lld", util::implicit_cast<unsigned long long>(t));
  c.append(buf, len);
  return c;
}

template <typename T>
auto operator << (Collector& c, T t)
  -> typename std::enable_if<std::is_floating_point<T>::value, Collector&>::type
{
  char buf[32];
  size_t len = sprintf(buf, "%f", util::implicit_cast<unsigned long long>(t));
  c.append(buf, len);
  return c;
}

// =======================================================================================
// C printf style collecting support
// Usage like:
//   collector("format string: answer to life, %s and everything: %d", "universe", 42); 
// User support custom type collecting by providing to_log() function for the type
// =======================================================================================
template <typename... Args>
Collector& Collector::operator () (char const* fmt, Args... args)
{
  // For logging, it's fine to just store the pointer, collector won't live over the line.
  set_format(fmt);
  // TODO
  return *this;
}

} } // namespace ku::log

