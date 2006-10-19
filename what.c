/*
 * This program will search through the files to find sccs identification
 * strings.  The identification string begins with @(#) and continues until
 * a non-printable character is found, or one of the following characters
 * are found: ", >, \, or new-line.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *_filename;
static char revision[]="@(#)$Header: /cvsroot/what/src/what.c,v 1.3 2006/10/19 20:56:52 sbnelson Exp $";

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
void
process_file(FILE *stream, int stop_on_first)
{
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
}

int
main(int argc, char **argv)
{
	int i;
	FILE *fd;
	int stop_on_first = 0;

	if (argc < 1)
		exit(1);
	else if (argc == 1) {
		_filename = "(stdin)";
		process_file(stdin, stop_on_first);
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
				process_file(fd, stop_on_first);
				Fclose(fd);
			}
		}
	}
	exit(0);
}
