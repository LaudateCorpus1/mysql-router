# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# add_harness_plugin - Add a new plugin target and set install
#                      location
#
# add_harness_plugin(name [NO_INSTALL]
#                    SOURCES file1 ...
#                    INTERFACE directory
#                    DESTINATION_SUFFIX string
#                    REQUIRES plugin ...)
#
# The add_harness_plugin command will set up a new plugin target and
# also set the install location of the target correctly.
#
# Plugins that are normally put under the "lib" directory of the build
# root, but see the caveat in the next paragraph.
#
# If NO_INSTALL is provided, it will not be installed, which is useful
# if the plugin is only for testing purposes. These plugins are also
# left in their default location and not moved to the "lib"
# directory. If you want to move the plugin to some specific
# directory, you have to set the target property
# LIBRARY_OUTPUT_DIRECTORY yourself.
#
# If DESTINATION_SUFFIX is provided, it will be appended to the
# destination for install commands. DESTINATION_SUFFIX is optional and
# default to ${HARNESS_NAME}.
#
# Files provided after the SOURCES keyword are the sources to build
# the plugin from, while the files in the directory after INTERFACE
# will be installed alongside the header files for the harness.

function(ADD_HARNESS_PLUGIN NAME)
  set(_options NO_INSTALL)
  set(_single_value INTERFACE DESTINATION_SUFFIX)
  set(_multi_value SOURCES REQUIRES)
  cmake_parse_arguments(ADD_HARNESS_PLUGIN
    "${_options}" "${_single_value}" "${_multi_value}" ${ARGN})

  if(ADD_HARNESS_PLUGIN_UNPARSED_ARGUMENTS)
    message(AUTHOR_WARNING
      "Unrecognized arguments: ${ADD_HARNESS_PLUGIN_UNPARSED_ARGUMENTS}")
  endif()

  # Set default values
  if(NOT ADD_HARNESS_PLUGIN_DESTINATION_SUFFIX)
    set(ADD_HARNESS_PLUGIN_DESTINATION_SUFFIX ${HARNESS_NAME})
  endif()

  # Add the library and ensure that the name is good for the plugin
  # system (no "lib" before). We are using SHARED libraries since we
  # intend to link against it, which is something that MODULE does not
  # allow. On OSX, this means that the suffix for the library becomes
  # .dylib, which we do not want, so we reset it here.
  # 
  # TODO: This need to be fixed properly when we move to Windows
  # support, since it is using a different suffix.
  add_library(${NAME} SHARED ${ADD_HARNESS_PLUGIN_SOURCES})
  set_target_properties(${NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".so")

  # Declare the interface directory for this plugin, if present. It
  # will be used both when compiling the plugin as well as as for any
  # dependent targets.
  if(ADD_HARNESS_PLUGIN_INTERFACE)
    target_include_directories(${NAME}
      PUBLIC ${ADD_HARNESS_PLUGIN_INTERFACE})
  endif()

  # Add a dependencies on interfaces for other plugins this plugin
  # requires.
  target_link_libraries(${NAME}
    harness-library
    ${ADD_HARNESS_PLUGIN_REQUIRES})
  
  # Need to be able to link plugins with each other
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${NAME} PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  set_target_properties(${NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${HARNESS_PLUGIN_OUTPUT_DIRECTORY})

  # Add install rules to install the interface header files and the
  # plugin correctly.
  if(NOT ADD_HARNESS_PLUGIN_NO_INSTALL AND HARNESS_INSTALL_PLUGINS)
    install(TARGETS ${NAME}
      LIBRARY DESTINATION lib/${ADD_HARNESS_PLUGIN_DESTINATION_SUFFIX})
    if(ADD_HARNESS_PLUGIN_INTERFACE)
      file(GLOB interface_files ${ADD_HARNESS_PLUGIN_INTERFACE}/*.h)
      install(FILES ${interface_files}
        DESTINATION ${HARNESS_INSTALL_INCLUDE_PREFIX}/${ADD_HARNESS_PLUGIN_DESTINATION_SUFFIX})
    endif()
  endif()
endfunction(ADD_HARNESS_PLUGIN)
