/*
  Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is also distributed with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have included with MySQL.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
#endif

#include "mysql_server_mock.h"

const unsigned DEFAULT_MOCK_SERVER_PORT = 3306;

using namespace server_mock;

void print_usage(const char* name) {
  std::cout << "Usage: \n";
  std::cout << name << " <expected_json_file_name> [port] [dbg_mode=0|1]\n";
  exit(-1);
}

int main(int argc, char* argv[]) {
  std::string queries_filename;
  unsigned port = DEFAULT_MOCK_SERVER_PORT;

#ifdef _WIN32
  WSADATA wsaData;
  int result;
  result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cerr << "WSAStartup failed with error: " << result << std::endl;
    return -1;
  }
#endif

  if (argc < 2 || argc > 4) {
    print_usage(argv[0]);
  }

  queries_filename = argv[1];
  if (argc > 2) {
    try {
      port =  static_cast<unsigned>(std::stoul(std::string(argv[2])));
    }
    catch (...) {
      print_usage(argv[0]);
    }
  }

  bool debug_mode = true;
  if (argc > 3) {
    debug_mode = std::string(argv[3]) == std::string("1");
  }

  try {
    MySQLServerMock mock(queries_filename, port, debug_mode);
    std::cout << "Starting MySQLServerMock" << std::endl;
    mock.run();
    std::cout << "MySQLServerMock::run() exited" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << "MySQLServerMock ERROR: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
