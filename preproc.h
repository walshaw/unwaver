/*  preproc.h
    ---------

SOFTWARE LICENSE for UNWAVER versions 0.1 to 0.7	24-7-7 to 17-4-8
© John Walshaw

This file is part of UNWAVER.

UNWAVER is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

UNWAVER is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
UNWAVER. If not, see <https://www.gnu.org/licenses/>.

This program was written by John Walshaw using his own computer and open
source software, 100% in his own time, between May 2007 and April 2008,
solely using hardware owned by him.

UNWAVER is released under GPLv3
(http://en.wikipedia.org/wiki/GNU_General_Public_License).

The source files covered by this license are:

    unwaver.c	version 0.5, 20th May 2007 to 6th April 2008	copyright (C) John Walshaw
    sequence.c	version 0.1, 20th May 2007 to 24th July 2007	copyright (C) John Walshaw
    utils.c	version 0.2, 20th May 2007 to 6th November 2007	copyright (C) John Walshaw
    argument.c	version 0.1, 20th May 2007 to 24th July 2007	copyright (C) John Walshaw
    prototype.h	version 0.2, 20th May 2007 to 25th July 2007	copyright (C) John Walshaw
    statvar.h	version 0.4, 20th May 2007 to 6th November 2007	copyright (C) John Walshaw
    preproc.h	version 0.5, 20th May 2007 to 17th April 2008	copyright (C) John Walshaw
    global.h	version 0.2, 20th May 2007 to 25th July 2007	copyright (C) John Walshaw

The file you are reading is version 0.5 of preproc.h, part of UNWAVER
versions 0.5 - 0.7.

*/

#ifndef PREPROC
#define PREPROC

/* PRE-PROCESSOR STANDARD LIBRARY 'INCLUDE'S: ------------------------------- */


#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>


/* PRE-PROCESSOR CONSTANTS:  ------------------------------------------------ */

#define MAX_QUAL_NAME_LENGTH 20		/* used by argument.h - the
					MAX_imum number of characters in the
					name of a command-line argument;
					ought to be specified in that header
					file, instead of here */

#define MAX_FILENAME_LENGTH 100           /* what it says... */

#define MAX_ERROR_STRING_LENGTH 255        /* what it says... */

#define DEFAULT_READ_LINE_LENGTH 9999

#define MAX_FASTA_HEADER_LENGTH 5000

#define MAX_SEQUENCE_AC_LENGTH 10
#define MAX_SEQUENCE_ID_LENGTH 10
#define MAX_SEQUENCE_NCBI_LENGTH 30     /* the max length of a 'compound field' in an
                                        NCBI-style FASTA header */
#define MAX_SEQUENCE_DES_LENGTH  4990    /* DES= description */

#define MAX_SEQUENCE_LENGTH 99999

#define MAX_CODES 75                    /* the max number of single-letter codes which
                                        may be dealt with (because the valid codes are
					stored in an array) */

#define MAX_IGNORED_CHARACTERS 200      /* the max number of residue/base codes which
                                           can be ignored in a single sequence while
					   still allowing it to be processed */

/* #define DEFAULT_VALID_CODES "ABCDEFGHIKLMNPQRSTUVWXYZ/-_*"
                                        /* a string (array of char) is set to this value */

#define MAX_N_FREQUENCIES 5000          /* the maximum number of different frequencies that
                                           may be calculated in a single FT */

#define MAX_WINDOW_SIZE 129             /* the maximum size (in residues or bases) of the
                                           sliding window used for the FT */

#define MAX_STATS_WINDOW_SIZE 1000      /* the maximum size (in residues or bases) of the
                                           sliding window used to calculate stats of the
					   position and size of the maximum peak in the FT */

#define MAX_SPLIT_SUBSTRINGS 50		/* used by split() of utils.c */
#define MAX_SPLIT_STRING_LENGTH 1000	/* used by split() of utils.c */
#define MAX_SPLIT_SUBSTRING_LENGTH 1000	/* used by split() of utils.c */
#define MAX_TRIM_STRING_LENGTH 1000	/* used by trim() of utils.c */
#define MAX_DUMP_STRING_LENGTH 1000	/* used by dumpstring() of utils.c */

#define MAX_FLOAT_COLUMNS    10         /* used by read_table_float() of utils.c */
#define MAX_FLOAT_ROWS       24         /* used by read_table_float() of utils.c */
#define READ_COLUMNS          3         /* used by read_table_float() of utils.c;
                                           the number of different columns of float data to read in */
#define MAX_LABEL_LENGTH    100         /* used by read_table_float() of utils.c */
#define MAX_TERM_STRING_LENGTH    100   /* used by read_table_float() of utils.c */

#define FLAGS 6				/* the number of available 'flags' i.e.
					binary command-line arguments, specified
					by '-<flagName>' */

                                        /* MODIFIED 25-7-7 0.1->0.2. PARS changed from 24 to 25. */
#define PARS 25				/* the number of 'parameters', i.e.
					command-line arguments coupled with
					values, specified by
					'-<parameterName> <parameterValue>' */

#define IMPLICIT_PARS 2                  /* the number of unspecified args whose
                                        meaning is determined implicitly, by
					their order; e.g., first implicit arg
					(if any) is taken to be an input file
					name */

#define MAX_UNNAMED_PARS 10		/* the MAX_imum number of parameters (i.e.
                                        strings which are not switches, or values
					following a switch designating a parameter)
					which may be specified on the command-line */

#endif

/* Change log.

MODIFIED 25-7-7 0.1->0.2. PARS changed from 24 to 25.
MODIFIED  6-4-8 0.2->0.3. MAX_FASTA_HEADER_LENGTH changed from 2000 to 4000.
MODIFIED  7-4-8 0.3->0.4. MAX_SEQUENCE_DES_LENGTH changed from 512 to 3990.
MODIFIED 17-4-8 0.4->0.5  MAX_FASTA_HEADER_LENGTH changed from 4000 to 5000.
MODIFIED 17-4-8 0.4->0.5. MAX_SEQUENCE_DES_LENGTH changed from 3990 to 4990.
*/
