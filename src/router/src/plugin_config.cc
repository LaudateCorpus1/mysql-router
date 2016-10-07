/*
  Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.

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

#include "mysqlrouter/plugin_config.h"

#ifndef _WIN32
# include <sys/un.h>
# include <unistd.h>
#endif

using std::invalid_argument;
using std::string;

namespace mysqlrouter {

string BasePluginConfig::get_section_name(const mysql_harness::ConfigSection *section) const noexcept {
  auto name = section->name;
  if (!section->key.empty()) {
    name += ":" + section->key;
  }
  return name;
}

string BasePluginConfig::get_option_string(const mysql_harness::ConfigSection *section, const string &option) {
  bool required = is_required(option);
  string value;

  try {
    value = section->get(option);
  } catch (const mysql_harness::bad_option &exc) {
    if (required) {
      throw invalid_argument(get_log_prefix(option) + " is required");
    }
  }

  if (value.empty()) {
    if (required) {
      throw invalid_argument(get_log_prefix(option) + " is required and needs a value");
    }
    value = get_default(option);
  }

  return value;
}

string BasePluginConfig::get_log_prefix(const string &option) const noexcept {
  return "option " + option + " in [" + section_name + "]";
}

TCPAddress BasePluginConfig::get_option_tcp_address(const mysql_harness::ConfigSection *section,
                                                    const string &option,
                                                    bool require_port,
                                                    int default_port) {
  std::string value = get_option_string(section, option);

  if (value.empty()) {
    return TCPAddress{};
  }

  try {
    std::pair<string, uint16_t> bind_info = mysqlrouter::split_addr_port(value);

    uint16_t port = bind_info.second;

    if (port <= 0) {
      if (default_port > 0) {
        port = static_cast<uint16_t >(default_port);
      } else if (require_port) {
        throw std::runtime_error("TCP port missing");
      }
    }

    return mysqlrouter::TCPAddress(bind_info.first, port);

  } catch (const std::runtime_error &exc) {
    throw invalid_argument(get_log_prefix(option) + " is incorrect (" + exc.what() + ")");
  }

}

int BasePluginConfig::get_option_tcp_port(const mysql_harness::ConfigSection *section,
                                          const string &option) {
  auto value = get_option_string(section, option);

  if (!value.empty()) {
    char *rest;
    errno = 0;
    auto result = std::strtol(value.c_str(), &rest, 0);

    if (errno > 0 || *rest != '\0' || result > UINT16_MAX || result < 1) {
      std::ostringstream os;
      os << get_log_prefix(option) << " needs value between 1 and 65535 inclusive";
      if (!value.empty()) {
        os << ", was '" << value << "'";
      }
      throw std::invalid_argument(os.str());
    }

    return static_cast<int>(result);
  }

  return -1;
}

mysql_harness::Path BasePluginConfig::get_option_named_socket(const mysql_harness::ConfigSection *section,
                                                              const string &option) {
  std::string value = get_option_string(section, option);

  #ifndef _WIN32
  if (value.size() > (sizeof(sockaddr_un().sun_path)-1)) {
    throw invalid_argument("Socket file path can be at most " + to_string(sizeof(sockaddr_un().sun_path)-1) + " characters (was " + to_string(value.size()) + ")");
  }
  #endif

  if (value.empty()) {
    return mysql_harness::Path();
  }

  mysql_harness::Path socket_path(value);
  if (socket_path.exists()) {
    throw std::invalid_argument(get_log_prefix(option) + " Socket file '" + value + "' already exists, cannot start");
  }

  return socket_path;
}

} // namespace mysqlrouter
