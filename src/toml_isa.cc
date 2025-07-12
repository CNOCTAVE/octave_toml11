// Copyright (C) 2024-2025 Yu Hongbo <yuhongbo@member.fsf.org>,
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <fstream>
#include <vector>
#include <toml.hpp>

#include <octave/oct.h>

void segfault_handler(int signum) {
  // toml11 has a lot of segfaults and not stable.
  // So we need to handle segfault.
  error ("Caught segmentation fault (signal %d)\n", signum);
}



DEFUN_DLD (toml_isa, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{ov} = toml_isa(@var{toml_data}, @var{key}, @var{type})\n\
{Loadable Function} @var{ov} = toml_isa(@var{toml_data}, @var{key}, @var{type}, @var{semver})\n\
Input toml string @var{toml_data}, key @var{key} and type string @var{type}.\n\
Judge the data of @var{key} is of @var{type} or not.\n\
Return @var{ov} (true or false).\n\
Support types:\n\
Type    Meaning\n\
empty    Judge the data of @var{key} is empty or not.\n\
boolean    Judge the data of @var{key} is boolean or not.\n\
integer    Judge the data of @var{key} is integer or not.\n\
floating    Judge the data of @var{key} is float or not.\n\
string    Judge the data of @var{key} is string or not.\n\
offset_datetime    Judge the data of @var{key} is offset datetime or not.\n\
local_datetime    Judge the data of @var{key} is local datetime or not.\n\
local_date    Judge the data of @var{key} is local date or not.\n\
local_time    Judge the data of @var{key} is local time or not.\n\
array    Judge the data of @var{key} is array or not.\n\
table    Judge the data of @var{key} is table or not.\n\
@var{semver} is semantic version of TOML. Default is [1, 0, 0] (i.e. TOML 1.0.0).\n\
@var{semver} should be a matrix and should contain 3 elements.\n\
If @var{semver} is greater or equal than TOML 1.1.0, supports TOML 1.1.0 options.\n\
@end deftypefn\n\
")
{
  signal(SIGSEGV, segfault_handler);
  octave_value retval;
  if ((args.length () <= 2) || (args.length () >= 5))
    print_usage ();
  if (! args(0).is_string ())
    error ("toml_isa: toml string should be a string");
  if (args(0).isempty ())
    error ("toml_isa: toml string should not be empty");
  if (! args(1).is_string ())
    error ("toml_isa: key should be a string");
  if (args(1).isempty ())
    error ("toml_isa: key should not be empty");
  if (! args(2).is_string ())
    error ("toml_isa: type string should be a string");
  if (args(2).isempty ())
    error ("toml_isa: type string should not be empty");
  if (args.length() >= 4 && !args(3).is_matrix_type())
    error ("toml_isa: semver should be a matrix");
  Matrix m = args(3).matrix_value();
  if (m.rows() * m.columns() != 3)
    error ("toml_isa: semver should contain 3 elements");
  std::string toml_data = args(0).string_value ();
  std::string key = args(1).string_value ();
  std::string isa_type = args(2).string_value ();
  bool result = false;
  auto spec = toml::spec::default_version();
  if (args.length() == 4)
    spec = toml::spec::v(m(0), m(1), m(2));
  auto data = toml::parse_str(toml_data, spec);
  if (isa_type == "empty")
  {
    result = data.at(key).is_empty();
  }
  else if (isa_type == "boolean")
  {
    result = data.at(key).is_boolean();
  }
  else if (isa_type == "integer")
  {
    result = data.at(key).is_integer();
  }
  else if (isa_type == "floating")
  {
    result = data.at(key).is_floating();
  }
  else if (isa_type == "string")
  {
    result = data.at(key).is_string();
  }
  else if (isa_type == "offset_datetime")
  {
    result = data.at(key).is_offset_datetime();
  }
  else if (isa_type == "local_datetime")
  {
    result = data.at(key).is_local_datetime();
  }
  else if (isa_type == "local_date")
  {
    result = data.at(key).is_local_date();
  }
  else if (isa_type == "local_time")
  {
    result = data.at(key).is_local_time();
  }
  else if (isa_type == "array")
  {
    result = data.at(key).is_array();
  }
  else if (isa_type == "table")
  {
    result = data.at(key).is_table();
  }
  retval = octave_value (result);
  return retval;
}
