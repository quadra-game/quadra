/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 *
 * Quadra, an action puzzle game
 * Copyright (C) 2006 Pierre Phaneuf <pphaneuf@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "update.h"

#include "cfgfile.h"
#include "dict.h"
#include "http_request.h"
#include "stringtable.h"
#include "version.h"

#define UPDATE_HOST "quadra.sourceforge.net"
#define UPDATE_PORT 80
#define UPDATE_PATH "/version.txt"
#define UPDATE_VERSION_KEY "stable"

class AutoUpdaterImpl: public AutoUpdater {
  Http_request *req;
  Buf buf;
public:
  AutoUpdaterImpl():
    req(NULL) {
  }
  void init();
  void step();
  ~AutoUpdaterImpl();
};

// Do not update more than once per this many seconds.
static time_t mindelay = 24 * 60 * 60;
static AutoUpdaterImpl* updater;

void AutoUpdater::start() {
  msgbox("Attempting to start auto-update...\n");

  if(updater) {
    msgbox("Auto-updater already started.\n");
    return;
  }

  updater = new AutoUpdaterImpl;
}

void AutoUpdaterImpl::init() {
  time_t now(time(NULL));
  char st[256];

  msgbox("update: init called.\n");

  if(now < config.info3.last_update + mindelay) {
    now = config.info3.last_update + mindelay;
    msgbox("update: updated too recently, not before %s", ctime(&now));
    ret();
    return;
  }

  buf.resize(0);
  buf.append("GET "UPDATE_PATH" HTTP/1.0\r\n");
  buf.append("Host: "UPDATE_HOST"\r\n");
  buf.append("Connection: close\r\n");
  snprintf(st, sizeof(st), "User-Agent: Quadra/%i.%i.%i\r\n",
	   VERSION_MAJOR, VERSION_MINOR, VERSION_PATCHLEVEL);
  buf.append(st);
  if(*config.info3.last_modified) {
    msgbox("update: setting If-Modified-Since to %s\n",
	   config.info3.last_modified);
    buf.append("If-Modified-Since: ");
    buf.append(config.info3.last_modified);
    buf.append("\r\n");
  } else
    msgbox("update: not setting If-Modified-Since\n");
  buf.append("\r\n");
  req = new Http_request(UPDATE_HOST, UPDATE_PORT, buf.get(), buf.size());
}

void AutoUpdaterImpl::step() {
  if(!req) {
    ret();
    return;
  }

  if(!req->done())
    return;

  ret();

  if(req->getsize()) {
    Stringtable st(req->getbuf(), req->getsize());
    char last_mod[64] = "";
    enum {
      STATUS,
      HEADER,
      REPLY,
    } state = STATUS;
    Dict reply;

    for(int i = 0; i < st.size(); ++i) {
      const char* str = st.get(i);

      switch(state) {
      case STATUS:
	if(strncmp(str, "HTTP/", 5) != 0) {
	  msgbox("update: Invalid HTTP response\n");
	  return;
	}

	while(str && *str != '\0' && *str != ' ' && *str != '\t')
	  ++str;

	while(str && *str != '\0' && (*str == ' ' || *str == '\t'))
	  ++str;

	if(strncmp(str, "200", 3) == 0)
	  msgbox("HTTP status is OK\n");
	else if(strncmp(str, "304", 3) == 0) {
	  msgbox("update: version file not modified\n");
	  config.info3.last_update = time(NULL);
	  config.write();
	  return;
	} else {
	  msgbox("update: HTTP status isn't 200 or 304: %s\n", str);
	  return;
	}

	state = HEADER;
	break;

      case HEADER:
	if(*str == '\0') {
	  state = REPLY;
	  continue;
	}

	// Take note of the Last-Modified header.
	if(strncasecmp(str, "Last-Modified:", 14) == 0) {
	  while(str && *str != '\0' && *str != ' ' && *str != '\t')
	    ++str;

	  while(str && *str != '\0' && (*str == ' ' || *str == '\t'))
	    ++str;

	  snprintf(last_mod, sizeof(last_mod), "%s", str);
	  msgbox("update: Last-Modified: %s\n", last_mod);
	}

	break;

      case REPLY:
	msgbox("update: reply: %s\n", str);
	reply.add(str);
	break;
      };
    }

    const char* val = reply.find("qserv");
    if(val)
      snprintf(config.info3.default_game_server_address,
	       sizeof(config.info3.default_game_server_address),
	       "%s", val);

    if(reply.find_sub("version")) {
      val = reply.find_sub("version")->find(UPDATE_VERSION_KEY);
      if(val && strcmp(VERSION_STRING, val) != 0)
	config.info3.new_version = true;
    }

    if(*last_mod)
      snprintf(config.info3.last_modified, sizeof(config.info3.last_modified),
	       "%s", last_mod);

    config.info3.last_update = time(NULL);
    config.write();

    msgbox("update: done\n");
  } else
    msgbox("update: failed\n");
}

AutoUpdaterImpl::~AutoUpdaterImpl() {
  updater = NULL;
  delete req;
}

