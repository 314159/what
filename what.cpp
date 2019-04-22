/*
 * This program will search through the files to find sccs identification
 * strings.  The identification string begins with @(#) and continues until
 * a non-printable character is found, or one of the following characters
 * are found: ", >, \, or new-line.
 *
 *  Copyright (C) 2011-2017 Scott Nelson
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include <iostream>

using namespace std;

bool process_file(bool stop_on_first);
bool process_file(bool stop_on_first, const string& filename);
bool process_file(bool stop_on_first, istream& strm, const string& filename);
int main(int argc, char** argv);

bool
process_file(bool stop_on_first)
{
  return process_file(stop_on_first, cin, "(stdin)");
}

bool
process_file(bool stop_on_first, const string& filename)
{
  fstream strm{filename, ios_base::in};
  return process_file(stop_on_first, strm, filename);
}

bool
process_file(bool stop_on_first, istream& strm, const string& filename)
{
  enum class state {
    got_nothing,
    got_at,
    got_open,
    got_hash,
    got_all};
  auto st = state::got_nothing;
  auto found = false;

  while (strm) {
    auto c = strm.get();
    if ((st == state::got_nothing) && (c == '@')) {
      st = state::got_at;
    } else if ((st == state::got_at) && (c == '(')) {
      st = state::got_open;
    } else if ((st == state::got_open) && (c == '#')) {
      st = state::got_hash;
    } else if ((st == state::got_hash) && (c == ')')) {
      st = state::got_all;
      found = true;
      cout << '\t';
    } else if ((st == state::got_all) && (
			    (not isprint(c))
			    || (c == '"')
			    || (c == '>')
			    || (c == '\n')
			    || (c == '\\')
			    || (c == '\0'))) {
      cout << '\n';
      st = state::got_nothing;
      if (stop_on_first) break;
    } else if (st == state::got_all) {
      cout << static_cast<unsigned char>(c);
    } else {
      st = state::got_nothing;
    }
  }

  return found;
}

void
usage()
{
  cerr << "Usage: what [-h] [-s] [filename ...]" << endl;
  cerr << "  -h  output this help end end" << endl;
  cerr << "  -s  stop on first sccs token" << endl;
}

int
main(int argc, char** argv)
{
  if (argc < 1) return 1;

  --argc;
  ++argv;

  auto stop_on_first = false;

  while (argc and argv[0][0] == '-') {
    if (string{argv[0]} == "-s") {
      stop_on_first = true;
    } else if (string{argv[0]} == "-h") {
      usage();
      return 0;
    } else {
      usage();
      return 1;
    }
    --argc;
    ++argv;
  }

  auto found = false;

  if (argc) {
    while (argc) {
      found |= process_file(stop_on_first, argv[0]);
      --argc;
      ++argv;
    }
  } else {
    found = process_file(stop_on_first);
  }
  return found ? 0 : 1;
}
