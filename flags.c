/*
 * cpio - copy file archives in and out
 *
 * Gunnar Ritter, Freiburg i. Br., Germany, April 2003.
 */
/*
 * Copyright (c) 2003 Gunnar Ritter
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*	Sccsid @(#)flags.c	1.6 (gritter) 3/26/07	*/

#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cpio.h"

void
cpio_flags(int ac, char **av)
{
	const char	optstring[] =
		"iopaAbBcC:dDeE:fH:I:kKlLmM:O:PrR:sStTuvV6";
	int	i, illegal = 0;

	if (getenv("SYSV3") != NULL)
		sysv3 = printsev = 1;
	while ((i = getopt(ac, av, optstring)) != EOF) {
		switch (i) {
		case 'i':
		case 'o':
		case 'p':
			if (action && action != i)
				illegal = 1;
			action = i;
			break;
		case 'a':
			aflag = 1;
			break;
		case 'A':
			Aflag = 1;
			break;
		case 'b':
			bflag = 1;
			break;
		case 'B':
			blksiz = 5120;
			Bflag = 1;
			break;
		case 'c':
			fmttype = sysv3 ? FMT_ODC : FMT_ASC;
			cflag = 1;
			break;
		case 'C':
			if ((blksiz = atol(optarg)) <= 0) {
				warnx("Illegal size given for -C option.");
				usage();
			}
			Cflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 'D':
			Dflag = 1;
			break;
		case 'e':
			/*
			 * This option is accepted for compatibility only,
			 * -Hdec should be used instead.
			 */
			fmttype = FMT_DEC;
			eflag = 1;
			break;
		case 'E':
			Eflag = optarg;
			break;
		case 'f':
			fflag = 1;
			break;
		case 'H':
			if (setfmt(optarg) < 0)
				illegal = 1;
			Hflag = 1;
			break;
		case 'I':
			Iflag = optarg;
			break;
		case 'k':
			kflag = 1;
			break;
		case 'K':
			/*
			 * This option is accepted for compatibility only,
			 * -Hsgi should be used instead.
			 */
			fmttype = FMT_SGIBE;
			Kflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'L':
			Lflag = 1;
			break;
		case 'm':
			mflag = 1;
			break;
		case 'M':
			Mflag = oneintfmt(optarg);
			break;
		case 'O':
			Oflag = optarg;
			break;
		case 'P':
			Pflag = 1;
			break;
		case 'r':
			rflag = 1;
			break;
		case 'R':
			if (setreassign(Rflag = optarg) < 0)
				illegal = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case 'S':
			Sflag = 1;
			break;
		case 't':
			tflag = 1;
			break;
		case 'u':
			uflag = 1;
			break;
		case 'v':
			vflag++;
			break;
		case 'V':
			Vflag = 1;
			break;
		case '6':
			sixflag = 1;
			fmttype = FMT_BINLE;
			break;
		default:
			if (sysv3)
				usage();
			illegal = 1;
		}
	}
	switch (action) {
	case 'i':
		if (Oflag || Kflag || eflag || Lflag || lflag || aflag ||
				Aflag || Pflag)
			illegal = 1;
		for (i = optind; i < ac; i++)
			addg(av[i], 0);
		break;
	case 'o':
		if (Iflag || dflag || fflag || kflag || mflag ||
				rflag || tflag || uflag ||
				sixflag || Eflag || Rflag)
			illegal = 1;
		if (optind != ac)
			illegal = 1;
		break;
	case 'p':
		if (Iflag || Oflag || blksiz || Eflag || fmttype != FMT_NONE ||
				Mflag || bflag || fflag || kflag || sflag ||
				tflag || Sflag || sixflag)
			illegal = 1;
		if (optind + 1 != ac)
			illegal = 1;
		break;
	default:
		if (sysv3 == 0)
			warnx("One of -i, -o or -p must be specified.");
		else if (ac > 1)
			warnx("Options must include one: -o, -i, -p.");
		illegal = 1;
	}
	/*
	 * Sanity checks. No check for multiple occurences of options
	 * since they can make sense, behave as other programs and use
	 * the latter one.
	 */
	/*if (aflag && mflag) {
		msg(3, 0, "-a and -m are mutually exclusive.\n");
		illegal = 1;
	} why? */
	/*if (cflag && (Hflag || Kflag || eflag)) {
		msg(3, 0, "-c and -H are mutually exclusive.\n");
		illegal = 1;
	} allow overriding -c with -H and vice versa */
	if ((vflag || tflag) && Vflag) {
		warnx("-v and -V are mutually exclusive.");
		illegal = 1;
	}
	/*if (Bflag && Cflag) {
		msg(3, 0, "-B and -C are mutually exclusive.\n");
		illegal = 1;
	} allow overriding of block sizes */
	if ((Hflag || cflag || Kflag || eflag) && sixflag) {
		warnx("-H and -6 are mutually exclusive.");
		illegal = 1;
	}
	if (!sysv3 && Mflag && Oflag == NULL && Iflag == NULL) {
		warnx("-M not meaningful without -O or -I.");
		illegal = 1;
	}
	if (!sysv3 && Aflag && Oflag == NULL) {
		warnx("-A requires the -O option");
		illegal = 1;
	}
	if (illegal)
		usage();
}

void
cpio_usage(void)
{
	if (sysv3)
		fprintf(stderr, "\
Usage:  %s -o[acvVABL] [-Csize] [-Hhdr] [-Mmsg] <name-list >collection\n\
\t%s -o[acvVABL] -Ocollection [-Csize] [-Hhdr] [-Mmsg] <name-list\n\
\t%s -i[bcdkmrsStuvVfB6] [-Csize] [-Efile] [-Hhdr] [-Mmsg] [-Rid] [pattern ...] <collection\n\
\t%s -i[bcdkmrsStuvVfB6] -Icollection [-Csize] [-Efile] [-Hhdr] [-Mmsg] [-Rid] [pattern ...]\n\
\t%s -p[adlmruvVL] [-Rid] directory <name-list\n",
			progname, progname, progname, progname, progname);
	else
		fprintf(stderr, "USAGE:\n\
\t%s -i[bcdfkmrstuvBSV6] [-C size] [-E file] [-H hdr] [[-I file] [-M msg]] \
[-R id] [patterns]\n\
\t%s -o[acvABLV] [-C size] [-H hdr] [[-M msg] [-O file]]\n\
\t%s -p[adlmuvLV] [-R id] directory\n",
			progname, progname, progname);
	exit(1);
}
