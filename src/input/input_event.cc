// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2005-2011, Jari Sundell <jaris@ifi.uio.no>

#include "display/attributes.h"

#include "input/input_event.h"

namespace input {

void
InputEvent::insert(torrent::Poll* p) {
  p->open(this);
  p->insert_read(this);
}

void
InputEvent::remove(torrent::Poll* p) {
  p->remove_read(this);
  p->close(this);
}

void
InputEvent::event_read() {
  int c;

  while ((c = getch()) != ERR)
    m_slotPressed(c);
}

void
InputEvent::event_write() {}

void
InputEvent::event_error() {}

}
