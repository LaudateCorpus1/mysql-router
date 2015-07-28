/*
  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <mysql/harness/plugin.h>

static const char* requires[] = {
  // This plugin do not exist
  "foobar",
};

static int init(AppInfo*) {
  return 0;
}

static int deinit(AppInfo*) {
  return 0;
}

Plugin bad_one = {
  PLUGIN_ABI_VERSION,
  "A bad plugin",
  VERSION_NUMBER(1,0,0),
  sizeof(requires)/sizeof(*requires),
  requires,
  0,
  nullptr,
  init,
  deinit,
  nullptr,
};
