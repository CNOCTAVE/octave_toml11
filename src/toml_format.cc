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



DEFUN_DLD (toml_format, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{ov} = toml_format(@var{toml_data})\n\
{Loadable Function} @var{ov} = toml_format(@var{toml_data}, @var{semver})\n\
Input toml string @var{toml_data}. Return formatted toml string @var{ov}.\n\
@var{semver} is semantic version of TOML. Default is [1, 0, 0] (i.e. TOML 1.0.0).\n\
@var{semver} should be a matrix and should contain 3 elements.\n\
If @var{semver} is greater or equal than TOML 1.1.0, supports TOML 1.1.0 options.\n\
@end deftypefn\n\
")
{
  signal(SIGSEGV, segfault_handler);
  octave_value retval;
  if ((args.length () <= 0) || (args.length () >= 3))
    print_usage ();
  if (! args(0).is_string ())
    error ("toml_format: toml string should be a string");
  if (args(0).isempty ())
    error ("toml_format: toml string should not be empty");
  if (args.length() == 2 && !args(1).is_matrix_type())
    error ("toml_format: semver should be a matrix");
  Matrix m = args(1).matrix_value();
  if (m.rows() * m.columns() != 3)
    error ("toml_format: semver should contain 3 elements");
  std::string toml_data = args(0).string_value ();
  auto spec = toml::spec::default_version();
  if (args.length() == 2)
    spec = toml::spec::v(m(0), m(1), m(2));
  auto data = toml::parse_str(toml_data, spec);
  std::string formatted_toml = toml::format(data);
  retval = octave_value (formatted_toml);

  return retval;
}
