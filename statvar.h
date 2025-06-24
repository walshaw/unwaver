/*  statvar.h
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

The file you are reading is version 0.4 of statvar.h, part of UNWAVER
versions 0.4 - 0.7.

*/

#ifndef STATVAR
#define STATVAR

static char *flagname[FLAGS] = {"debug","sskip","h","n","q","v"};

/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parname[], "firstseq", added. */
static char *parname[PARS] = {
                              "scomment", "ac",    "id",     "ignore", "on",
			      "off",      "valid", "wstats", "from",   "to",
			      "peaks",    "stats", "maxsigma","target","range",
			      "firstseq", "b",        "c",     "f",      "i",
			      "l",        "r",        "s",     "w",      "x"
			     };

static char *implicit[IMPLICIT_PARS] = {"i","s"};

static char *flaglabel[FLAGS] = {"debug mode",
                                 "skip blank lines in scale file",
                                 "help mode",
				 "NCBI FASTA header format",
                                 "quiet output mode",
				 "verbose mode"};

/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parlabel[] added. */

static char *parlabel[PARS] = {
                               /* scomment */ "comment symbol in scale file",
			       /* ac       */ "AC field no.",
			       /* id       */ "ID field no.",
			       /* ignore   */ "ignore these characters in sequences",
                               /* on       */ "'on' value for binary treatment of bases/residues",
			       /* off      */ "'off' value for binary treatment of bases/residues",
			       /* valid    */ "string containing all valid single-letter codes",
			       /* wstats   */ "size of window (in residues) for calculating stats",
			       /* from     */ "lowest period for which to calculate FT",
			       /* to       */ "highest period for which to calculate FT",
                               /* peaks    */ "name of output file of frequencies (and periods) with highest magnitude",
			       /* stats    */ "name of output file of statistics of frequencies with highest magnitude",
                               /* maxsigma */ "only segments with sigma <= this value will be reported",
                               /* target   */ "only report segments within range of this target PERIOD (and with sigma <= maxsigma)",
                               /* range    */ "see target",
                               /* firstseq */ "first sequence to process from input file; numbering starts at 1",
                               /* b        */ "single-letter codes assigned the 'on' value; all others are 'off'",
			       /* c        */ "data column number(s) in scale file", /* this is a string - can be one or more comma-delimited
			                                            integers */
			       /* f        */ "number of frequencies (or periods) to calculate; default is sequence length",
			       /* i        */ "input sequence (FASTA) file name",
			       /* l        */ "label (single-letter code) column number in scale file",
                               /* r        */ "frequency (number of sequences processed so far) of progress report",
			       /* s        */ "name of scale file (input)",
			       /* w        */ "window size (residues) for calculating FT",
			       /* x        */ "value to assign to unrecognized single-letter codes, if using scoring scale"
			       };


/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *partype[], "posint", added. */

/* possible values are "posint", "negint", "int", "nonzeroint", "decimal", "float", "alphabetic",
  "alphanumeric","string" */

static char *partype[PARS] = {
                              /* scomment */ "string",
			      /* ac       */ "posint",
			      /* id       */ "posint",
			      /* ignore   */ "string",
			      /* on       */ "int",
			      /* off      */ "int",
                              /* valid    */ "string",
			      /* wstats   */ "posint",
			      /* from     */ "decimal",
			      /* to       */ "decimal",
			      /* peaks    */ "string",
			      /* stats    */ "string",
                              /* maxsigma */ "decimal",
                              /* target   */ "decimal",
                              /* range    */ "decimal",
                              /* firstseq */ "posint",
			      /* b        */ "alphabetic",
			      /* c        */ "string",
			      /* f        */ "posint",
			      /* i        */ "string",
			      /* l        */ "posint",
			      /* r        */ "posint",
			      /* s        */ "string",
			      /* w        */ "posint",
			      /* x        */ "decimal"
			     };

/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parmin[], "posint", added. */

static char *parmin[PARS] = {/* scomment */ "",
                             /* ac       */ "0",
			     /* id       */ "0",
			     /* ignore   */ "",
			     /* on       */ "-1000",
			     /* off      */ "-1000",
			     /* valid    */ "",
			     /* wstats   */ "10",
			     /* from     */ "1.0",
			     /* to       */ "1.0",
			     /* peaks    */ "",
			     /* stats    */ "",
                             /* maxsigma */ "0.0001",
                             /* target   */ "0.0", /* this has a special meaning - see *pardefault[] */
                             /* range    */ "0.0",
			     /* firstseq */ "1",
			     /* b        */ "",
			     /* c        */ "",
			     /* f        */ "10",
			     /* i        */ "",
			     /* l        */ "1",
			     /* r        */ "0", /* means, don't produce progress reports, except on completion */
			     /* s        */ "",
			     /* w        */ "10",
			     /* x        */ "-100"
			    };

/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parmax[], "2147483647", added. */

static char *parmax[PARS] = {
                             /* scomment */ "",
			     /* ac       */ "2",
			     /* id       */ "3",
			     /* ignore   */ "",
			     /* on       */ "1000",
			     /* off      */ "1000",
			     /* valid    */ "",
			     /* wstats   */ "1000",
			     /* from     */ "5000.0", /* should be equivalent to MAX_N_FREQUENCIES */
			     /* to       */ "5000.0", /* should be equivalent to MAX_N_FREQUENCIES */
			     /* peaks    */ "",
			     /* stats    */ "",			     
                             /* maxsigma */ "20.0",
                             /* target   */ "100.0",
                             /* range    */ "200.0",
			     /* firstseq */ "2147483647", /* largest positive value of a 4-byte int */
			     /* b        */ "",
			     /* c        */ "",
			     /* f        */ "10000",
			     /* i        */ "",
			     /* l        */ "99",
			     /* r        */ "1000000",
			     /* s        */ "",
			     /* w        */ "1000",
			     /* x        */ "100"
			    };

/* MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *pardefault[], "1", added. */

static char *pardefault[PARS] = {
                             /* scomment */ "#",
			     /* ac       */ "2",
			     /* id       */ "1",
			     /* ignore   */ "/-_*",
			     /* on       */ "1",
			     /* off      */ "0",
			     /* valid    */ "ABCDEFGHIKLMNPQRSTUVWXYZ/-_*",
			     /* wstats   */ "65",
			     /* from     */ "3.0",
			     /* to       */ "4.0",
			     /* peaks    */ "",
			     /* stats    */ "",			     
                             /* maxsigma */ "1.0",
                             /* target   */ "0.0", /* this signifies, "don't bother with a periodicity range" */
                             /* range    */ "0.1",
			     /* firstseq */ "1",
			     /* b        */ "", /* was "FAMILYV" */
			     /* c        */ "2",
			     /* f        */ "5000",
			     /* i        */ "",
			     /* l        */ "1",
			     /* r        */ "1000",
			     /* s        */ "",
			     /* w        */ "129",
			     /* x        */ "0.0"
			    };

/* in the string representing valid single-letter codes, the order of codes should be most-common first;
   to minimize search time (when checking each letter of a query string for valid codes) */

/* MODIFIED 25-7-7 0.1->0.2. Changed version number in program_name string. */
/* MODIFIED 4-8-7 0.2->0.3. Changed version number in program_name string. */
/* MODIFIED 6-11-7 0.3->0.4. Changed version number in program_name string. */
static char program_name[20]   = "UNWAVER 0.7";
/* MODIFIED 6-11-7 0.3->0.4. Changed date in program_date string. */
static char program_date[20]   = "7-4-2008";
static char program_author[20] = "John Walshaw";
/* MODIFIED 25-7-7 0.1->0.2. Added author_email[25] string. */
static char author_email[25]   = "johnwalshaw (at) iscb.org";
/* MODIFIED 25-7-7 0.1->0.2. This line removed: static char copyright = 169; */
/*

'Flags' (binary switches)

    debug	- writes out loads of very verbose info; this always sets verbose mode
    sskip       - skip blank lines in the hydrophobicity scale file
        h	- displays help, then quits
        q	- 'quiet' output
        v       - verbose mode; the verbosity does not necessarily equal that of -debug
        n       - NCBI FASTA header mode

'Parameters' (arguments with values)

        scomment - the character which signifies (at the beginning of the line or preceded
	           only by whitespace) a comment line in the scale file
        ac      - (integer) number of field to be treated as the ID; note that fields
	          of the FASTA header are delimited by whitespace, and the first field
		  is field 1 (not 0); the '>' character is ignored; default value is 2;
		  BUT if '-n' is set then the number refers to a subfield of field 1
		  (delimited by '|') and the default value is also 2; a value of 0
		  means that no field is to be interpreted as an ACession number
		  (irrespective of '-n')
        id      - (integer) number of field to be treated as the ID; note that fields
	          of the FASTA header are delimited by whitespace, and the first field
		  is field 1 (not 0); the '>' character is ignored; default value is 1,
		  UNLESS '-n' is set, in which case the default value is 3, and the
		  number refers to a subfield (delimited by '|') of field 1; a value of
		  0 means that no field is to be interpreted as an IDentifier
        i	- (string) name of input file; must contain FASTA-format sequences
	b	- (string) with a binary treatment of each residue/base, that is each is
		  assigned a value of 0 or 1, the specified string contains all those
		  letters to be treated as 1; any others will be treated as 0
	s	- (string) name of file containing scoring scale (e.g. hydrophobicity
	          scale) of values for each letter; one letter must be defined on one
		  row of the file, and the letter must be the first non-whitespace
		  column, but multiple subsequent columns may be present; see '-c'
        c	- column(s) to use to read scoring scale from the file defined by '-s';
	          if unspecified, column 2 will be used; note that column numbering
		  starts at 1, not 0. If more than one column is specified, the
		  numbers must be separated by commas (with no spaces)
	x       - !!! ONLY APPLIES TO REAL, not binary, treatment
	specifies the value assigned to any single-letter codes which are
	          not specified by -b (if performing the FT on 'binary' values) or
		  in the scale file (if reading real numbers from that); note that
		  what x specifies will apply to BOTH the binary and real treatments
		  (assuming they are both being performed); if -x specifies a non-integer,
		  then the non-integer part will be ignored for the purpose of the
		  binary treatment (which uses integer arithmetic)
*/

#endif

/* Change log.

MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parname[], "ignore", added.
MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parlabel[] added.
MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *partype[], "posint", added.
MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parmin[], "posint", added.
MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *parmax[], "2147483647", added.
MODIFIED 25-7-7 0.1->0.2. 16th element (element 15) of *pardefault[], "1", added.
MODIFIED 25-7-7 0.1->0.2. Changed version number in program_name string.
MODIFIED 25-7-7 0.1->0.2. This line removed: static char copyright = 169; 
MODIFIED 4-8-7  0.2->0.3. Changed version number in program_name string.
MODIFIED 6-11-7 0.3->0.4. Changed version number in program_name string.
MODIFIED 6-11-7 0.3->0.4. Changed date in program_date string.
*/
