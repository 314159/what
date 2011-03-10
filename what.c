/*
 * This program will search through the files to find sccs identification
 * strings.  The identification string begins with @(#) and continues until
 * a non-printable character is found, or one of the following characters
 * are found: ", >, \, or new-line.
 *
 *  Copyright (C) 2011 Scott Nelson
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *_filename;
static char revision[]="@(#)$Header: /cvsroot/what/src/what.c,v 1.5 2011/03/10 01:15:11 sbnelson Exp $";

/* "wrapper" for standard fputs */
int
Fputs(const char *s, FILE *stream)
{
	int result;

	result = fputs(s, stream);
	if (result == EOF) {
		perror("(stdout)");
	}
	return(result);
}

/* "wrapper" for standard fclose */
int
Fclose(FILE *fd)
{
	int result;

	result = fclose(fd);
	if (result == EOF) {
		perror(_filename);
	}
	return(result);
}

/* "wrapper" for standard putchar */
int
Putchar(int c)
{
	int result;

	if ( (result = putchar(c)) == EOF) {
		perror("(stdout)");
	}
	return(result);
}

/* "wrapper for standard fgetc */
int
Fgetc(FILE *stream)
{
	int c;

	c = fgetc(stream);
	if (ferror(stream)) {
		perror(_filename);
	}
	return(c);
}

/* This is the main logic.  It processes the supplied
 * file stream, and writes its output to stdout.
 */
int
process_file(FILE *stream, int stop_on_first)
{
	int found = 0;

	int c;
	enum state {got_nothing,got_at,got_open,got_hash,got_all};
	enum state st;

	st = got_nothing;
	while ( (c = Fgetc(stream)) != EOF) {
		if (c == '@')
			st = got_at;
		else if ((st == got_at) && (c == '('))
			st = got_open;
		else if ((st == got_open) && (c == '#'))
			st = got_hash;
		else if ((st == got_hash) && (c == ')')) {
			/* got it all! Output tab and it ident string
			 * followed by a new line.
			 */
			found = 1;
			Putchar('\t');
			while ( ((c = Fgetc(stream)) != EOF)
				&& isprint(c)
				&& (c != '"')
				&& (c != '>')
				&& (c != '\n')
				&& (c != '\\')
				&& (c != '\0')) {
				Putchar(c);
			}
			Putchar('\n');
			if (stop_on_first)
				break;
		} else
			st = got_nothing;
	}

	return found;
}

int
main(int argc, char **argv)
{
	int i;
	FILE *fd;
	int stop_on_first = 0;
	int found = 0;

	if (argc < 1)
		exit(1);
	else if (argc == 1) {
		_filename = "(stdin)";
		found = process_file(stdin, stop_on_first);
	} else {
		for (i=1; i < argc; i++) {
			if(strncmp(argv[i], "-s", 2) == 0) {
				stop_on_first = 1;
			}
		}

		for (i=1; i < argc; i++) {
			if(strncmp(argv[i], "-s", 2) == 0) {
				continue;
			}

			_filename = argv[i];
			fd = fopen(argv[i], "r");
			if (fd == NULL)
				perror(argv[i]);
			else {
				Fputs(_filename, stdout);
				Putchar(':');
				Putchar('\n');
				found = found || process_file(fd, stop_on_first);
				Fclose(fd);
			}
		}
	}
	exit(found ? 0 : 1);
}
