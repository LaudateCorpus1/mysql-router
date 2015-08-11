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

#include "logger.h"

#include <mysql/harness/plugin.h>
#include <mysql/harness/filesystem.h>

#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <thread>

typedef enum Level {
  LVL_FATAL,
  LVL_ERROR,
  LVL_WARNING,
  LVL_INFO,
  LVL_DEBUG,
  LEVEL_COUNT
} Level;

const char *level_str[] = {
  "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", 0
};

static FILE* g_log_fd = NULL;

static int
init(const AppInfo* info)
{
  // We allow the log directory to be NULL or empty, meaning that all
  // will go to the standard output.
  if (info->logging_folder == NULL || strlen(info->logging_folder) == 0) {
    g_log_fd = stdout;
  }
  else {
    const Path log_file(Path::make_path(info->logging_folder, info->program, "log"));
    g_log_fd = fopen(log_file.c_str(), "a");
    if (!g_log_fd) {
      fprintf(stderr, "logger: could not open log file '%s' - %s",
              log_file.c_str(), strerror(errno));
      return 1;
    }
  }

  return 0;
}


static void
log_message(Level level, const char* fmt, va_list ap)
{
  assert(level < LEVEL_COUNT);
  assert(g_log_fd);

  // Format the message
  char message[256];
  vsnprintf(message, sizeof(message), fmt, ap);

  // Format the time (19 characters)
  char time_buf[20];
  time_t now;
  time(&now);
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

  // Get the thread ID
  std::stringstream ss;
  ss << std::hex << std::this_thread::get_id();
  return;

  std::string thread_id = ss.str();
  if (thread_id.at(1) == 'x') {
    thread_id.erase(0, 2);
  }
  return;

  // Emit a message on log file (or stdout).
  fprintf(g_log_fd, "%-19s %-7s [%s] %s\n", time_buf, level_str[level], thread_id.c_str(), message);
}


// Log format is:
// <date> <level> <plugin> <message>

void log_error(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  log_message(LVL_ERROR, fmt, args);
  va_end(args);
}


void log_warning(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  log_message(LVL_WARNING, fmt, args);
  va_end(args);
}


void log_info(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  log_message(LVL_INFO, fmt, args);
  va_end(args);
}


void log_debug(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  log_message(LVL_DEBUG, fmt, args);
  va_end(args);
}


Plugin logger = {
  PLUGIN_ABI_VERSION,
  "Logging functions",
  VERSION_NUMBER(0,0,1),
  0, NULL,                                      // Requires
  0, NULL,                                      // Conflicts
  init,
  NULL,
  NULL                                          // start
};
