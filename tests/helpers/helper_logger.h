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


#ifndef ROUTING_TESTS_HELPER_LOGGER_INCLUDED
#define ROUTING_TESTS_HELPER_LOGGER_INCLUDED

#include <fstream>

namespace mysql_harness {

namespace logging {

#ifdef __cplusplus
extern "C" {
#endif

void log_error(const char* name, const char *fmt, ...);
void log_warning(const char* name, const char *fmt, ...);
void log_info(const char* name, const char *fmt, ...);
void log_debug(const char* name, const char *fmt, ...);

void set_log_fd(FILE *fp);

#ifdef __cplusplus
}
#endif

}  // namespace logging

}  // namespace mysql_harness

#endif // ROUTING_TESTS_HELPER_LOGGER_INCLUDED
