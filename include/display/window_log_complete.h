// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#ifndef RTORRENT_DISPLAY_WINDOW_LOG_COMPLETE_H
#define RTORRENT_DISPLAY_WINDOW_LOG_COMPLETE_H

#include <torrent/utils/log_buffer.h>

#include "display/window.h"

namespace display {

class WindowLogComplete : public Window {
public:
  typedef torrent::log_buffer::const_iterator iterator;

  WindowLogComplete(torrent::log_buffer* l);
  ~WindowLogComplete();

  virtual void redraw();

private:
  inline iterator find_older();

  torrent::log_buffer* m_log;
};

}

#endif
