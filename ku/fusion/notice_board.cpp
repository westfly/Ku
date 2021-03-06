/***************************************************************
 * Copyright 2011, Zhang, Jun. All rights reserved.            *
 * Author: Zhang, Jun (ralph dot j dot zhang at gmail dot com) *
 *                                                             *
 * This source code is provided with absolutely no warranty.   *
 ***************************************************************/ 
#include "notice_board.hpp"

namespace ku { namespace fusion {

NoticeBoard::NoticeBoard() : pending_updates_(false)
{
}

bool NoticeBoard::remove_notice(NoticeId notice_id)
{
  pending_updates_ = true;
  std::lock_guard<std::mutex> _(mutex_);
  update_list_.emplace_back(notice_id);
  return true;
}

bool NoticeBoard::modify_notice(NoticeId notice_id, Notice::EventHandler const& event_handler,
    std::initializer_list<Notice::EventType> const& event_types)
{
  pending_updates_ = true;
  std::lock_guard<std::mutex> _(mutex_);
  // Set notice.raw_handle = -1 to distinguish from remove (where raw_handle = 0)
  update_list_.emplace_back(notice_id, Notice(-1, event_handler));
  for (Notice::EventType event_type : event_types)
    update_list_.back().notice.set_event_type(event_type);
  return true;
}

NoticeId NoticeBoard::add_notice(int raw_handle, std::initializer_list<Notice::EventType> const& event_types,
    Notice::EventHandler const& event_handler)
{
  pending_updates_ = true;
  std::lock_guard<std::mutex> _(mutex_);
  update_list_.emplace_back(NoticeId(0), Notice(raw_handle, event_handler));
  for (Notice::EventType event_type : event_types)
    update_list_.back().notice.set_event_type(event_type);
  return update_list_.back().notice.id();
}

void NoticeBoard::apply_updates()
{
  if (!pending_updates_)
    return;
  UpdateList list;
  list.reserve(update_list_.size());
  {
    std::lock_guard<std::mutex> _(mutex_);
    list.swap(update_list_);
  }
  for (Update& update : list) {
    if (update.id) {
      if (update.notice.raw_handle()) {
        // Modify
        modify_notice_internal(update.id, update.notice);
      } else {
        // Remove
        remove_notice_internal(update.id);
      }
    } else {
      // Add
      add_notice_internal(std::move(update.notice));
    }
  }
  pending_updates_ = false;
}

} } // namespace ku::fusion

