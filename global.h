/*  global.h
    --------

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

The file you are reading is version 0.2 of global.h, part of UNWAVER
versions 0.2 - 0.7.

*/

#ifndef GLOBAL
#define GLOBAL
enum boolean {false,true};

static char *boolean_name[2] = {"false","true"};

enum flags {flag_debug,flag_sskip,flag_h,flag_n,flag_q,flag_v};

/* MODIFIED 25-7-7 0.1->0.2. 16th value (equivalent to 15) of enum pars, par_firstseq, added. */

enum pars {par_scomment, par_ac,    par_id,       par_ignore, par_on,
           par_off,      par_valid, par_wstats,   par_from,   par_to,
	   par_peaks,    par_stats, par_maxsigma, par_target, par_range,
	   par_firstseq, par_b,     par_c,        par_f,      par_i,
	   par_l,        par_r,     par_s,        par_w,      par_x};

#endif

/*

Change log.

MODIFIED 25-7-7 0.1->0.2. 16th value (equivalent to 15) of enum pars, par_firstseq, added.

*/
