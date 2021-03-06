#pragma once
#include <system_error>
#include <unistd.h>
#include "util.hpp"

namespace ku { namespace fusion {

namespace ops {
struct Common;
} // namespace ku::fusion::ops


// =======================================================================================
// Handle is a thin wrapper to OS file descriptor (raw_handle) with an error_code.
// A handle owns the file descriptor, copying is prohibited to prevent multiple owners.
// 
// A file descriptor can have at most one owner, the owner is responsible for the 
// life cycle (closing the file descriptor when destroyed).
//
// The ownership can be transferred with C++ move semantics. The handle being moved
// is cleared after the move.
// 
// Handle is a wrapper. It's not supposed to be used polymorphically. It has no
// virtual methods at all (not even destructor).
// =======================================================================================
template <typename T>
class Handle : private util::noncopyable
{
  friend struct ops::Common;
  friend T;
  friend class NoticeBoard;
  friend class WeakHandle;

public:
  Handle() : raw_handle_(0) { }
  Handle(Handle&& h) { *this = std::move(h); }
  ~Handle() { close(); }

  Handle& operator=(Handle&& h)
  {
    raw_handle_ = h.raw_handle_;
    h.clear();
    return *this;
  }

  bool valid() const { return raw_handle_ > 0; }
  explicit operator bool () { return valid(); }
  void close()
  {
    // throwing from dtor is a bad idea, we just swallow errors.
    if (valid()) raw_handle_ = ::close(raw_handle_);
  }

private:
  explicit Handle(int raw_handle) : raw_handle_(raw_handle) { }
  int raw_handle() const { return raw_handle_; }
  void clear() { raw_handle_ = 0; }

private:
  int raw_handle_;
};


// =======================================================================================
// WeakHandle doens't own the file descriptor. It's nothing more than a temporary storage
// =======================================================================================
class WeakHandle
{
  friend class NoticeBoard;

public:
  WeakHandle() = delete;
  ~WeakHandle() = default;
  template <typename T>
  WeakHandle(Handle<T> const& h) : raw_handle_(h.raw_handle_) { }

private:
  int raw_handle() const { return raw_handle_; }
  int raw_handle_;
};

} } // namespace ku::fusion

