/*  unwaver.c
    ---------

SOFTWARE LICENSE for UNWAVER versions 0.1 to 0.6	24-7-7 to 7-4-8
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

The file you are reading is version 0.4 of unwaver.c, part of UNWAVER
versions 0.4 - 0.6.

*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "preproc.h"
#include "global.h"
#include "prototype.h"

int main(int argc, char *argv[]) {

    /* sets STATIC variables, e.g. names of command-line switches */
    #include "statvar.h"

    enum boolean setflag[FLAGS];
    char *par[PARS];

    char /* filenames */
         sequence_file_name[MAX_FILENAME_LENGTH],
         hydrophobicity_scale_file_name[MAX_FILENAME_LENGTH],
         peaks_file_name[MAX_FILENAME_LENGTH],
         stats_file_name[MAX_FILENAME_LENGTH],

         /* miscellaneous strings */
         error_string[MAX_ERROR_STRING_LENGTH],
	 textstring[DEFAULT_READ_LINE_LENGTH],

         /* string treated as array of char, specifying valid one-character codes */
	 valid_code[MAX_CODES],

         /* string treated as array of char, specifying IGNORED one-character codes;
	 these will not abort the processing of the sequence (unlike anything which
	 isn't in valid_code[] */
	 ignored_code[MAX_CODES],

         /* string treated as array of char, specifying one-character codes which
	 represent 'on' in the binary on versus off treatment of each residue/base */
	 on_code[20], /* unlikely you would want to specify as many as 20 codes as 'on' */

         /* attributes specifying data to read from hydrophobicity scale file */
         row_label[MAX_FLOAT_ROWS][MAX_LABEL_LENGTH],

         /* the same as row_label, but it's an array of single char (i.e. a string!), not
	    an array of strings */
         label_code[MAX_FLOAT_ROWS],

         /* strings read from each FASTA-format sequence record */
         acc_no[MAX_SEQUENCE_AC_LENGTH],
         id[MAX_SEQUENCE_ID_LENGTH],
	 description[MAX_SEQUENCE_DES_LENGTH],

	 cleaned_sequence[MAX_SEQUENCE_LENGTH] /* any ignored characters have been removed */

	 /* program_name[20] /* includes version 'number'; a float or int isn't used to store
	                     the version, because it isn't necessarily only numeric
			     (e.g. "0.1b") */
	 ;

         /* string read from each FASTA-format sequence record */
	 static char sequence[MAX_SEQUENCE_LENGTH];           /* this might be very large */


    FILE *fasta_sequence_file, *scale_file, *peaks_file, *stats_file;

    /* I think most of the MAX_FLOAT_COLUMNS used as array-size specifiers below should really
       be READ_COLUMNS - because that's the maximum number which can be read by utils.c
       table_float_to_int() in any case
    */

    int i, j,
        acc_field,
	id_field,
	seq_length,
	n_invalid_characters, /* an invalid character is one which causes processing of the
	                         sequence to be aborted (c.f. and IGNORED character) */
	on_value,         /* for binary treatment of each base/residue; on and off values   */
	off_value,        /*                                                                */
	comment_symbol,
	label_column,    /* the index no. of the column (in the scale file) containing the string-label to identify
	                    the row; internally, column numbers begin at 0, but the user specifies them starting
			    at 1 */
        n_columns,       /* the number of (float-type) DATA columns to be read from the scale file */
	column_order[MAX_FLOAT_COLUMNS], /* the index no.s of the columns (and their order) to be read from the
	                                    scale file; only the first n_columns elements are ever used */
	rows_read,       /* the number of rows of DATA read from the scale file */
        use_binary,      /* boolean; was a set of 'on' codes (for Binary treatment) specified or not? */
        use_scales,      /* boolean; was a scales file specified or not? */
        alpha_to_table_row[255], /* for speed; this maps any character (hopefully the ones encountered will
	                           be letters of the alphabet) to the row number in the table read in
				   (represented by label_code, hydrophobicity_int, etc) */
        default_residue_score,   /* note this is type INT, not float; assuming the scale-file derived scoring,
	                         this is the default score for any letter encountered which is not represented
				 by a row in that table; a drawback is that this will be the same value for
				 all columns used (different columns may use different integer-attaining
				 scale factors); but then again, usually it will be 0 */
        n_frequencies,
	window_size,             /* size of the window in which each FT is performed */
	stats_window_size,        /* size of window in which stats (of mean of maximum peak of all periods
	                            between a specified range; see min_period and max_period below) are
				    calculated */
	output_logic     /* 0 if write to no files; 1 if write peaks file; 2 if write stats file;
                                   3 if write both; 4 if write both sets of data to THE SAME FILE */
	;

    float hydrophobicity[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS];
    /* float default_residue_score; */

    float scaled_up, /* for calculating scaled-up data and checking that they are integers - see below */
          min_period, /* these represent the range of periods (and so implicitly frequencies) for */
	  max_period, /* which FT will be applied; this is because the 'strongest' period (within
	                     a particular range) is what is of interest                           */
          sigma_max,  /* the reporting of 'hits' (segment(s) of each input sequence) uses this as one
	                 of (or the only) criterion: sigma (S.D.) must be equal to or less than sigma_max
			 for the entire segement */
	  target_period, /* possibly, the only hits of interest have a mean period within some specified
	                 range; if the user has specified a target period, then the segment (as well as
			 meeting the sigma_max criterion, see above) must have a mean period equal to
			 target_period, +/- target_period_range (see below); do not confuse this with
			 the period range specified by min_period and max_period (see above); the
			 min_period .. max_period range is that for which corresponding frequencies
			 in the FT are CALCULATED; therefore, the peak of highest magnitude is guaranteed
			 to be for a frequency corresponding to a period in this range; if the user is
			 interested in highlighting segments with a particular period, then they should
			 specify this with target_period and target_period_range; (i.e. the question is,
			 "within the periodicity range min_period .. max_period, does the strongest
			 period occur within the range target_period +/- target_period_range?"
			 For example, the object might be, "within the range of periods representing
			 coiled-coil repeats (say, 3.0 to 4.0 to be liberal), is the strongest signal
			 in the range representing a classic 11-residue repeat (3.667 +/- 0.1)?" If
			 the strongest signal was for a canonical 7-residue repeat, say 3.5 +/- 0.1,
			 then this would be largely or totally missed if you specified, for example,
			 min_period = 3.567 and max_period = 3.767.
			 */
	  target_period_range,
	  hours;

    /* these need to be static as MAX_N_FREQUENCIES * MAX_WINDOW_SIZE can be very large       */
    static float coeff_cos[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE], /* precalculated to save time */
                 coeff_sin[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE]; /* precalculated to save time */

    int hydrophobicity_int[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS], /* see below */
        column_exponent[MAX_FLOAT_COLUMNS]; /* exponent (to base 10) to produce scale-factor by
	                                       which to multiply the float-type hydrophobicity[][]
					       scores (to make them integer, without losing any
					       precision); (in other words, the number of decimal
					       places to shift to the right); the resulting integers
					       go in hydrophobicity_int[][]; note that each column
					       may have a different value (and therefore scale
					       factor); reason for using integers is speed, natch */

        static int binary_res_scores[MAX_SEQUENCE_LENGTH], /* this array stores, for each residue (or base) of
	                                       a single sequence, the
	                                       score derived from the 'binary' scoring scheme, in
					       which the 'on' residues (here, hydrophobic), defined
					       by the -b command-line argument, all share the same
					       value (specified by -on), while all the other residues
					       have a different value (specified by -off) */

	scale_res_scores[MAX_FLOAT_COLUMNS][MAX_SEQUENCE_LENGTH], /* for each user-specified column
	                                       (read from the scale file specified by -s; in
					       scale_res_scores the column numbering always starts
					       from 0 irrespective of what the column number was
					       in the original file), there is an array of scores,
					       one score per residue for a single sequence; these
					       scores are derived from the scale file (and may have
					       been scaled up in order to produce integers) */

	all_res_scores[MAX_FLOAT_COLUMNS+1][MAX_SEQUENCE_LENGTH], /* for speed, the binary_res_scores
	                                       and scale-derived scale_res_scores are combined into
					       a single table */

        all_column_exponent[MAX_FLOAT_COLUMNS+1],                 /* as are the exponents of the scale
	                                                             factors */
	n_all_columns,                         /* this is the total number of scoring schemes (columns
	                                       in all_res_scores and all_column_exponent) */
	sequence_hit_segments,                 /* the number of hit segments (by ANY scoring scheme
	                                          used) for the CURRENT sequence */
        total_hit_segments,                    /* the total number of hit segments (by ANY scoring scheme
	                                          used) for ALL sequences */
        start_time,      /* in seconds, set immediately prior to reading first sequence */
	elapsed_time,    /* in seconds, reset after reading report_frequency sequences */
        report_frequency,/* units are sequences */
	n_sequences,     /* number of sequences read so far */
        n_hit_sequences, /* the total number of sequences which contain one or more hit segments */

        /* MODIFIED 25-7-7 0.1->0.2. Added int variable, ignore_sequences. */
        ignore_sequences; /* this number of sequences will be ignored from the beginning of the file;
	                    when non-zero, its value is decremented by 1 immediately prior to reading
			    each sequence; each sequence is not analyzed if */

    time_t *t; /* is this needed? */

    /* MODIFIED 25-7-7 0.1->0.2. Removed 4th arg 'copyright' from display_license() call. */
    /* MODIFIED 25-7-7 0.1->0.2. Addeded new 4th arg 'author_email' to display_license() call. */

    display_license(program_name,program_date,program_author,author_email);

    start_time = time(NULL);

    /* MEMORY CLEANING!! - more might be required...*/ 

    strcpy(hydrophobicity_scale_file_name,"");
    strcpy(sequence_file_name,"");

        /* clean various tables */

        for (i = 0; i < MAX_FLOAT_COLUMNS; i++)
	    {
	    column_exponent[i] = 0;
	    clean_float_column(hydrophobicity[i],   MAX_FLOAT_ROWS);
	    clean_int_column(hydrophobicity_int[i], MAX_FLOAT_ROWS);
	    }

    /* begin proper */

    set_arguments(argc, argv, flagname, parname, implicit, flaglabel, parlabel, setflag, par);

    if (! limit_arguments(setflag, parname, parlabel, par, partype, parmin, parmax, pardefault)) {
        sprintf(error_string, "Error in specified parameters.");
        fatal(error_string);
    }

    /* MODIFIED 25-7-7 0.1->0.2. Added 25th argument (&ignore_sequences) to interpret_arguments() call. */

    interpret_arguments(setflag,
                        par,
			hydrophobicity_scale_file_name,
			&comment_symbol,
                        &label_column,
			&n_columns,
			column_order,
			&default_residue_score,
			valid_code,
			ignored_code,
			on_code,
			&on_value,
			&off_value,
			&n_frequencies,
			&window_size,
			&min_period,
			&max_period,
			&stats_window_size,
			&sigma_max,
			&target_period,
			&target_period_range,
			sequence_file_name,
			&acc_field,
			&id_field,
			&ignore_sequences,
			peaks_file_name,
			stats_file_name,
			&output_logic,
			&report_frequency
		       );

    printf("\nSummary of parameters:\n\n\tFT will be calculated for sliding windows of size %d residues (or bases)\n",window_size);
    printf("\tFT will be calculated only for frequencies corresponding to periodicities in the range %8.4f ... %8.4f\n",min_period,max_period);
    printf("\tstatistics of strongest peaks will be calculated for groups of %d consecutive FT-windows\n",stats_window_size);
    printf("\tsegments (stats windows) of each sequence which have a S.D. of the strongest frequency of <= %8.4f will be reported\n",sigma_max);
    if (target_period > 0.0)
        {
	printf("\t- but only where the mean of the strongest frequency corresponds to a periodicity of %8.4f +/- %8.4f\n",target_period,target_period_range);
	}
    else
        {
	printf("\t - irrespective of the mean of the strongest frequency\n");
	}
    printf("\nThe above will be calculated for EACH specified scoring scheme independently\n\n");

    if (strlen(hydrophobicity_scale_file_name))
        {
	use_scales = 1;
	printf("scales file: \"%s\"\n\n",hydrophobicity_scale_file_name);
	}
    else {
        use_scales = 0;
	}

    if (strlen(on_code))
        {
	use_binary = 1;
	}
    else {
        use_binary = 0;
	}


    /* i temporarily stores the total number of scoring schemes to use */
    i = use_binary;
    if (use_scales) i += n_columns;

    printf("\nSummary of scoring scheme(s): %d different scheme(s) will be used\n\n", i);
    if (use_binary)
        {
	printf("\t1 binary scoring scheme (%s = %d, all other characters = %d)\n", on_code, on_value, off_value);
	}
    if (use_scales)
        {
	printf("\t%d scheme(s) from table in file \"%s\"\n",n_columns,hydrophobicity_scale_file_name);
	}
    printf("\n\n");

    /* precalculate coefficients */

    printf("pre-calculating coefficients for %d frequencies using a window of %d residues ...",
            n_frequencies, window_size);

    calculate_coeffs(coeff_cos, coeff_sin, window_size, n_frequencies);

    printf("done\n\n");

    if (use_scales && (scale_file = open_file(hydrophobicity_scale_file_name, "read", 2)))
        {
        printf("opened hydrophobicity scale file\n");

	rows_read = read_table_float(
	                   scale_file,
			   1,			/* start at this line in the hydrophobicity scale file */
			   0,			/* end at this line in the hydrophobicity scale file */
			   column_order, 	/* 1D array specifying order in which column data is to be read
			                    		into hydrophobicity[][] table (see below) */
			   label_column,			/* column number (first column is column 0) to be used as string label */
			   comment_symbol,
			   setflag[flag_sskip], /* boolean; whether to ignore blank lines, or not */
			   "//",
			   hydrophobicity,	/* 2D array (table of float data) */
			   row_label,		/* 1D array (table of string data); each row corresponds to the same
			                           row of hydrophobicity */
			   setflag[flag_v]      /* boolean: verbose, or not */
			);

        printf("%d rows read from %s\n", rows_read, hydrophobicity_scale_file_name);

        table_float_to_int(hydrophobicity, n_columns, rows_read, hydrophobicity_int, column_exponent);

        if (setflag[flag_v])
	    {
	    printf("data columns will be turned into integers by scaling by, respectively:\n\t");
            for (j = 0; j < n_columns; j++)
	        {
	        if (j) printf(", ");
	        printf("%8.0f",pow(10,column_exponent[j]));
	        }
	    printf("\n - and then the results will be appropriately rescaled\n\n");
	    }

        printf("%d scoring scheme(s) from hydrophobicity scale file will be used:\n\n       ", n_columns);

        for (j = 0; j < n_columns; j++)
	    {
	    printf("%8d",j);
	    }

        printf("\n\n");

        for (i = 0; i < rows_read; i++)
	    {
	    printf("%8s",row_label[i]);
	    for (j = 0; j < n_columns; j++)
	        {
	        /* printf("%8d",hydrophobicity_int[j][i]); */
	        printf("%8.3f",hydrophobicity[j][i]);
	        }
	    printf("\n");

            /* convert row-labels (strings) to single-characters */
            label_code[i] = row_label[i][0];
	    }

        printf("\n"); 

        /* populate lookup table, used later for speed of conversion of sequence
	   to a value (or values, if more than one column is being used) from the
	   hydrophobicity table */

        for (i = 0; i < 256; i++)
	    {
	    alpha_to_table_row[i] = -1;
	    j = 0;
	    while ((j < rows_read) && (label_code[j] != i)) { j++; }
	    if (label_code[j] == i) { alpha_to_table_row[i] = j; }
	    if (setflag[flag_debug]) printf("\t\talpha_to_table_row[%d] = %d\n",i, alpha_to_table_row[i]);
	    }

        } /* end of block conditional on a hydropathy scale file being specified */


        /* open output files */

        if ((output_logic > 0) && (output_logic != 2))
            {
	    if (peaks_file = open_file(peaks_file_name,"write",2))
	        {
		printf("For ALL sequences, all the FT-windows' top peaks (magnitude, frequency and period) will be written to file \"%s\"\n\n"
		        , peaks_file_name);
		fprintf(peaks_file,"#\n");
		fprintf(peaks_file,"# %s\t%s\t%s\n",program_name,program_date,program_author);
		fprintf(peaks_file,"#\n");
		fprintf(peaks_file,"# FT-WINDOW PEAKS\n");
		fprintf(peaks_file,"#\n");
		fprintf(peaks_file,"# The first column is the central residue/base of the sliding window in which\n");
		fprintf(peaks_file,"# each FT is calculated.\n");
		fprintf(peaks_file,"#\n");
/* MODIFIED 6-11-7 0.3->0.4. Changed 2 paragraphs of explanatory text written to peaks file, re the new 'phase' column(s)
   MODIFICATION BEGIN */

		fprintf(peaks_file,"# There then follow 4 columns for EACH scoring-scheme you specified. If you\n");
		fprintf(peaks_file,"# specified a 'binary' scoring scheme (with -b) then that scheme is always the\n");
		fprintf(peaks_file,"# first. Following that come data for the other schemes you specified, in the\n");
		fprintf(peaks_file,"# same order in which you specified them with -c (which is not necessarily the\n");
		fprintf(peaks_file,"# same order that they appear in the scores file).\n");
		fprintf(peaks_file,"#\n");
		fprintf(peaks_file,"# Each group of 4 columns represents the strongest Fourier peak in the window.\n");
		fprintf(peaks_file,"# The first is the FREQUENCY with the strongest peak. The second is the\n");
                fprintf(peaks_file,"# frequency expressed as a PERIOD, i.e. the FT window size divided by the\n");
                fprintf(peaks_file,"# frequency. The third column is the MAGNITUDE of the strongest peak. The\n");
                fprintf(peaks_file,"# fourth column is the PHASE of the peak.\n");
                fprintf(peaks_file,"#\n\n\n");
/* MODIFICATION END */
	        }
            else {
	       exit(1);
	        }
	    }

	if ((output_logic > 1) && (output_logic < 4)) /* if 4, then the stats file and the peaks file
	                                                    are the same */
            {
	    if (stats_file = open_file(stats_file_name,"write",2))
	        {
		printf("For ALL sequences, all the stats-windows' results for strongest frequency (mean frequency, mean period, S.D. of frequency) will be written to file \"%s\"\n\n"
		        , stats_file_name);
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# %s\t%s\t%s\n",program_name,program_date,program_author);
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# FT STATISTICS\n");
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# The first column is the central residue/base of the sliding window in which\n");
		fprintf(stats_file,"# STATISTICS (of mean strongest peaks from FT-windows) are calculated.\n");
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# There then follow 3 columns for each scoring-scheme you specified. If you\n");
		fprintf(stats_file,"# specified a 'binary' scoring scheme (with -b) then that scheme is always the\n");
		fprintf(stats_file,"# first. Following that come data for the other schemes you specified, in the\n");
		fprintf(stats_file,"# same order in which you specified them with -c (which is not necessarily the\n");
		fprintf(stats_file,"# same order that they appear in the scores file).\n");
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# Each group of 3 columns represents the average of the strongest Fourier peak\n");
		fprintf(stats_file,"# from each of a number of consecutive FT-windows. Each FT window may be\n");
		fprintf(stats_file,"# represented by its central residue/base, and by the FT FREQUENCY of greatest\n");
		fprintf(stats_file,"# magnitude. That frequency is more usefully expressed as a PERIOD. Each stats\n");
		fprintf(stats_file,"# window therefore contains of a number of (strongest) frequencies (and\n");
		fprintf(stats_file,"# periods).\n");
		fprintf(stats_file,"#\n");
		fprintf(stats_file,"# The first column of each group of 3 is the MEAN of all these FT FREQUENCIES.\n");
                fprintf(stats_file,"# The second column is this same mean frequency expressed as a PERIOD, i.e.\n");
                fprintf(stats_file,"# the FT-WINDOW size (not stats window size) divided by the mean frequency.\n");
                fprintf(stats_file,"# The third column is the STANDARD DEVIATION of the FREQUENCY (not of the\n");
		fprintf(stats_file,"# period).\n");
                fprintf(stats_file,"#\n\n\n");
			          /*12345678901234567890123456789012345678901234567890123456789012345678901234567890*/
	        }
            else {
	       exit(1);
	        }
            }


    printf("sequence_file_name is '%s'\n",sequence_file_name);

    if (setflag[flag_v]) printf("\nindex numbers of fields: acc = %d, id = %d\n\n", acc_field, id_field);

    /* open the FASTA sequence file */

    if (fasta_sequence_file = open_file(sequence_file_name, "read", 2))
        {
        if (setflag[flag_v]) printf("file opened ok\n");
        }
    else {
        sprintf(error_string, "unable to read file '%s'", sequence_file_name);
	fatal(error_string);
        }

    elapsed_time = time(NULL); /* for only this use, 'elapsed_time' isn't elapsed time */
    printf("\n#\n# preliminaries took %d second(s)\n#\n",elapsed_time - start_time );
    start_time = elapsed_time; /* this is the 'real' start time, i.e. just before the
                                  first sequence gets processed */

/* MODIFIED 25-7-7 0.1->0.2. Added 16 lines: if (ignore_sequences) {...} else {...}
   MODIFICATION BEGIN */

   if (ignore_sequences)
       {
       printf("# The first ");
       if (ignore_sequences > 1)
           {
	   printf("%d sequences",ignore_sequences);
	   }
       else {
           printf("sequence");
           }
       printf(" in the input file will be ignored.\n#\n\n");
       }
   else
       {
       printf("# All the sequences in the input file will be analyzed.\n#\n\n");
       }

/* MODIFICATION END */

    n_sequences = 0;

    /* loop one sequence at a time through the FASTA file */

    while (seq_length = next_sequence(fasta_sequence_file, setflag[flag_n], acc_field, id_field,
                                      acc_no, id, description, sequence))
        {

/* MODIFIED 25-7-7 0.1->0.2. Added 5 lines: if (ignore_sequences) {...}
   MODIFICATION BEGIN */

	if (ignore_sequences)
	    {
	    ignore_sequences--;
	    continue;
	    }

/* MODIFICATION END */

        if (setflag[flag_v]) printf("\nretrieved id '%s', acc '%s', length %d, description '%s'\n",
	        id, acc_no, seq_length, description);
 
        if ((output_logic > 0) && (output_logic != 2))
	    {
	    fprintf(peaks_file, "# %s (%s) length %d; %s\n", id, acc_no, seq_length, description);
	    }
        if ((output_logic > 1))
	    {
	    fprintf(stats_file, "# %s (%s) length %d; %s\n", id, acc_no, seq_length, description);
	    }
 
             { /* void block?? */
	    n_invalid_characters = invalid_sequence(sequence, valid_code);
	    if (n_invalid_characters)
	        {
		printf("ABORTING sequence %s %s: contains %d invalid characters\n",
		       id, acc_no, n_invalid_characters);
		continue;
		}

	    if (setflag[flag_v]) printf("\tsequence ok\n");

	    seq_length = clean_sequence(cleaned_sequence, sequence, ignored_code, 1);

	    if (!seq_length)
	        {
		printf("ABORTING sequence %s %s\n", id, acc_no);
		continue;
		}
	    if (setflag[flag_v]) printf("cleaned sequence length: %d\n",seq_length);

            n_all_columns = 0;

            if (use_binary)
	        {
                /* convert the sequence to an array of 'on' or 'off' values (both are integers) */
		binary_score_sequence(cleaned_sequence, on_code, on_value, off_value, binary_res_scores);
		n_all_columns = 1;
		}

            if (use_scales)
	        {
                /* convert the sequence to n_columns arrays of integer values  */
		scale_score_sequence(cleaned_sequence, label_code, hydrophobicity_int,
		                     n_columns, default_residue_score, alpha_to_table_row, scale_res_scores);
		n_all_columns += n_columns;
		}

            for (i = 0; i < seq_length; i++)
		{

		if (use_binary)
		    {
		    all_res_scores[0][i] = binary_res_scores[i];
		    all_column_exponent[0] = 0;
		    }

		if (use_scales)
		    {
		    for (j = 0; j < n_columns; j++)
		        {
		        if (use_binary) {
			    all_res_scores[j+1][i] = scale_res_scores[j][i];
                            all_column_exponent[j+1] = column_exponent[j];
			    }
		        else {
			    all_res_scores[j][i] = scale_res_scores[j][i];
                            all_column_exponent[j] = column_exponent[j];
			    }
			}
		    }

		if (setflag[flag_v]) {
		    printf("\t%3d %c %2d\t", i, cleaned_sequence[i], binary_res_scores[i]);
		    if (use_scales)
		        {
		        for (j = 0; j < n_columns; j++)
		            {
		            printf("%5d", scale_res_scores[j][i]);
			    }
		        }
		    printf("\n");
		    }
		}

            if (use_binary)
	        {
		if (setflag[flag_debug]) show_int_col(binary_res_scores, seq_length);
		}

            if (use_scales)
		{
		if (setflag[flag_debug]) for (j = 0; j < n_columns; j++)
		    {
		    show_int_col(scale_res_scores[j], seq_length);
		    }
		}


            if (setflag[flag_debug])
	        printf("about to call slidingFT(); period range =  %8.3f ... %8.3f\n", min_period, max_period);

            sequence_hit_segments = slidingFT(
	              all_res_scores,
	              all_column_exponent,
		      n_all_columns,
		      seq_length,
		      coeff_cos,
		      coeff_sin,
		      window_size,
		      n_frequencies,
		      min_period,
		      max_period,
		      stats_window_size,
		      sigma_max,
		      target_period,
		      target_period_range,
		      peaks_file,
		      stats_file,
		      output_logic,
		      acc_no
		     ); /* the function returns the number of 'hit segments' from ANY scoring scheme */

            if (sequence_hit_segments)    {
                printf("HIT (%3d segments) %s (%s), length %d, description '%s'\n",
	        sequence_hit_segments, acc_no, id, seq_length, description);
		total_hit_segments += sequence_hit_segments;
		n_hit_sequences++;
		}

	    } /* end of void block ?? */

        if (output_logic)
	    {
	    if (output_logic != 2) fprintf(peaks_file, "\n\n"); /* for the benefit of GnuPlot! */
	    if (output_logic >  1) fprintf(stats_file, "\n\n"); /* for the benefit of GnuPlot! */
	    }

        ++n_sequences;

	/* a zero report_frequency means, don't provide progress reports (except for a
	   final one at the end) */

	if ( (report_frequency) && (! (n_sequences  % report_frequency) ) )
	    {
	    elapsed_time = time(NULL) - start_time;
	    hours = 1.0 * elapsed_time / 3600;
	    printf("#\n# %7d sequences processed in %6d seconds (%8.2f hours); %5d hit segments in %5d sequences\n#\n",
	           n_sequences, elapsed_time, hours, total_hit_segments, n_hit_sequences);
	    }

        } /* end of loop which iterates over sequences (i.e. 1 loop per sequence) */

	elapsed_time = time(NULL) - start_time;
	hours = 1.0 * elapsed_time / 3600;
	printf("#\n# Final: %7d sequences processed in %6d seconds (%8.2f hours); %5d hit segments in %5d sequences\n#\n",
	       n_sequences, elapsed_time, hours, total_hit_segments, n_hit_sequences);
        printf("\nCOMPLETED\n");

        return 0;

} /* end of main() */


/* this function is always application-specific */

void interpret_arguments(
/* MODIFIED 25-7-7 0.1->0.2. Added 25th argument (int *ignore_sequences) to interpret_arguments() definition. */
    enum boolean setflag[],
    char *par[],
    char infile_hyd_name[],	/* name of file containing hydrophobicity scores */
    int *symbol,                /* really a CHAR (but using a pointer to a char gives a stupid
                                   compiler warning, even if all other declarations and calls are
				   consistent with a pointer to a char; so just use a pointer to an
				   integer instead, and treat the integer to which it points as a char;
				   the char signifies a comment-line in above file */
    int *label_column,          /* the index no. of the column containing the row-identifying string-label */
    int *n_columns,             /* the number of columns specified on the command-line */
    int column_order[],         /* specifies from which column(s) float-data are to be read, and in what order */
    /* float */ int *default_value,       /* the value assigned to any single-letter code which is
                                   not specified by the scale file (if a scale file has been specified at all) */
    char valid_code[],          /* string - treated as array of char - specifying all the valid one-character codes */
    char ignored_code[],        /* string - treated as array of char - specifying all the IGNORED one-character codes */
    char on_code[],             /* string - treated as array of char - specifying all the 'on' one-character codes */
    int *on_value,              /* for Binary (on v off) treatment of each residue, this is the 'on' value */
    int *off_value,             /* for Binary (on v off) treatment of each residue, this is the 'off' value */
    int *n_frequencies,         /* number of different frequencies for which to calculate Fourier coefficients */
    int *window_size,           /* window size in bases or residues; if an even number has been specified, 1 is added */
    float *min_period,          /* lowest bound of the period range in which to find the highest Fourier amplitude */
    float *max_period,          /* highest bound of the period range in which to find the highest Fourier amplitude */
    int *stats_window_size,     /* size of window in which statistics of 'mean max peak' are calculated */
    float *sigma_max,            /* */
    float *target_period,        /* */
    float *target_period_range,  /* */
    char infile_seq_name[],     /* name of file containing FASTA-format sequences */

    int *acc_field,		/* which field of the FASTA header is the accession no. */
    int *id_field,		/* which field of the FASTA header is the identifier */
    int *ignore_sequences,      /* the number of seqs to ignore from the beginning of the input file; this is one less
                                   than the value of the -firstseqs argument */
    char outfile_peaks_name[],  /* name of file of data of 'freqs (and periods) with max peaks' versus window */
    char outfile_stats_name[],  /* name of file of data of stats (mean, SD) of freqs-with-max-peaks */
    int *output_logic,          /* 0 if write to no files; 1 if write peaks file; 2 if write stats file;
                                   3 if write both; 4 if write both sets of data to THE SAME FILE */
    int *report_frequency       /* a progress report is made every time this no. of seqs has been processed */
    ) {

    char column_order_string[MAX_FLOAT_COLUMNS][MAX_SPLIT_SUBSTRING_LENGTH],
         error_string[MAX_ERROR_STRING_LENGTH];
    int i,j, w_size;

/* for the various integers, should really be using atoi instead of sscanf( .. , "%d", ...) */

    sscanf(par[par_s], "%s", infile_hyd_name);

    *symbol = par[par_scomment][0];

    sscanf(par[par_l], "%d", label_column); /* N.B. 3rd arg is NOT &label_column, because *label_column
                                              is a pointer to an int, and label_column is the location (address)
					      it points to */

    if (setflag[flag_v]) printf("column %d specified as label column\n", *label_column);

    /* users specify columns where the first column is column 1; internally, the numbering starts at 0 */

    (*label_column)--;

    *n_columns = split(',', par[par_c], MAX_FLOAT_COLUMNS, 0, true, column_order_string);

    if (setflag[flag_v]) printf("%d data column(s) specified\n",*n_columns);

    for (i = 0; i < *n_columns; i++)
       {
       sscanf(column_order_string[i], "%d", &column_order[i]);

       if (setflag[flag_v]) printf("\t(%d) column %d\n", i+1, column_order[i]);

       if (column_order[i] < 1)
           {
	   sprintf(error_string, "column %d is meaningless (column numbers must be between 1 and %d", column_order[i],MAX_FLOAT_COLUMNS);
	   fatal(error_string);
	   }
       column_order[i]--;
       if (column_order[i] == *label_column)
           {
	   sprintf(error_string, "a specified data column clashes with the label column");
	   fatal(error_string);
	   }
       for (j = 0; j < i; j++)
           {
	   if (column_order[i] == column_order[j])
	       {
 	       sprintf(error_string, "a data column has been specified more than once");
	       fatal(error_string);
	       }
	   }
       }

    /* sscanf(par[par_x], "%f", default_value); */
    sscanf(par[par_x], "%d", default_value);

    sscanf(par[par_valid], "%s", valid_code);

    sscanf(par[par_ignore], "%s", ignored_code);

    sscanf(par[par_b], "%s", on_code); /* it's par 'b' for Binary (on versus off) */

    /* check that at least one scoring scheme to use has been specified! */
    if ( !strlen(on_code) && !strlen(infile_hyd_name))
        {
        sprintf(error_string, "A list of 'on' bases/residues (using -b), OR a base/residue scoring file (using -s), OR both, must be specified.");
        fatal(error_string);
	}

    sscanf(par[par_on], "%d", on_value); 

    sscanf(par[par_off], "%d", off_value); 

    sscanf(par[par_f], "%d", n_frequencies); 

    if (*n_frequencies > MAX_N_FREQUENCIES)
        {
 	sprintf(error_string, "the specified number of frequencies (%d) is too large; maximum allowed is %d",
	        *n_frequencies, MAX_N_FREQUENCIES);
	fatal(error_string);
	}

    sscanf(par[par_w], "%d", &w_size); 

    *window_size = (w_size / 2);
    *window_size *= 2;
    *window_size += 1;

    if (*window_size != w_size)
        {
	printf("Window size corrected to %d (an even number should not be used)\n", *window_size);
	}

    if (*window_size > MAX_WINDOW_SIZE)
        {
 	sprintf(error_string, "the specified window size (%d) is too large; maximum allowed is %d",
	        *window_size, MAX_WINDOW_SIZE);
	fatal(error_string);
	}

    sscanf(par[par_from], "%f", min_period);

    sscanf(par[par_to], "%f", max_period);

  /* should really be doing it like this...  *min_period = atof(par[par_from]); */

    sscanf(par[par_wstats], "%d", stats_window_size);

    sscanf(par[par_maxsigma], "%f", sigma_max);

    sscanf(par[par_target], "%f", target_period);

    sscanf(par[par_range], "%f", target_period_range);

    /* Note that a target_period of zero means that we don't worry about target (mean) periods;
       all mean periods will be considered worthy of reporting */

    if (*target_period > 0.0)
        {

        if ( (*target_period < *min_period) || (*target_period > *max_period) )
            {
            sprintf(error_string, "The specified target period (%10.6f) lies outside the range for which FT will be calculated (%8.3f ... %8.3f)",
	            *target_period, *min_period, *max_period);
	    fatal(error_string);
	    }

        if ( (*target_period - *target_period_range) <= *min_period)
            {
	    if ( (*target_period + *target_period_range) >= *max_period)
	        {
	        sprintf(error_string,"The specified target period range (%10.6f +/- %10.6f) is too large, because ALL calculated mean strongest periods will be between %8.3f and %8.3f",
                        *target_period, *target_period_range, *min_period, *max_period);
                fatal(error_string);
	        }
	    else
	        {
		printf("WARNING: the lower bound of the target period range (%10.6f - %10.6f) falls outside the minimum period for which FT will be calculated (%8.3f)",
                       *target_period, *target_period_range, *min_period);
		}
	    }
	else
	    {
	    if ( (*target_period + *target_period_range) >= *max_period)
	        {
		printf("WARNING: the upper bound of the target period range (%10.6f + %10.6f) falls outside the maximum period for which FT will be calculated (%8.3f)",
                       *target_period, *target_period_range, *max_period);
		}
	    }
	}

    if (strcmp(par[par_i],""))
        {
	sscanf(par[par_i], "%s", infile_seq_name);
	}
    else {
        sprintf(error_string, "no input sequence file was specified; use -h to get help");
	fatal(error_string);
        }

    sscanf(par[par_ac], "%d", acc_field);  /* N.B. 3rd arg is NOT &acc_field, because *acc_field
                                              is a pointer to an int, and acc_field is the location
					      it points to */
    sscanf(par[par_id], "%d", id_field);   /* see above */

    /* MODIFIED 25-7-7 0.1->0.2. Added 2 lines to function interpret_arguments(), to determine *ignore_sequences. */
    sscanf(par[par_firstseq], "%d", ignore_sequences);
    (*ignore_sequences)--; /* -firstseq specifies which number sequence (minimum = 1) to BEGIN processing */

    *output_logic = 0;

    if (strcmp(par[par_peaks],""))
        {
        sscanf(par[par_peaks], "%s", outfile_peaks_name);
	/* MODIFIED 25-7-7 0.1->0.2. Parenthesized *output_logic in (*output_logic) += 1. */
	(*output_logic) += 1;
	}

    if (strcmp(par[par_stats],""))
        {
        sscanf(par[par_stats], "%s", outfile_stats_name);
	/* MODIFIED 25-7-7 0.1->0.2. Parenthesized *output_logic in (*output_logic) += 2. */
	(*output_logic) += 2;
	if (*output_logic == 3)
	    {
	    /* in this case, both sets of data (peaks and stats) will be written as
	       different columns in the SAME FILE */
	    if (!strcmp(outfile_peaks_name,outfile_stats_name))
	        {
		*output_logic = 4;
	    /* however, this is not yet supported, due to the difficulty in simultaneously
	       writing to file the 'freq of max peak' for an FT window (represented by
	       it's centre, which is a residue or base number) and the stats (mean and sigma)
	       for the same residue or base number; for residue r representing an FT
	       window centre, the stats for the stats window which has the same centre
	       will not yet be known; so to get round this, the 'freqs of max peaks'
	       would have to be stored; this isn't yet done, because the approach is to
	       do as much on-the-fly as possible, without storage and subsequent
	       lookup operations to slow things down */

	        sprintf(error_string, "sorry, the option of writing the window-FT results and running stats thereof to the same file, isn't currently supported");
	        fatal(error_string);
		}
	    }
	}

    sscanf(par[par_r], "%d", report_frequency);

}


void show_int_col(int column[], int n_rows)
    {
    int i;
    for (i = 0; i < n_rows; i++)
        {
	printf("%4d: %4d\n",i, column[i]);
        }
    }

void calculate_coeffs(float coeff_cos[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
                      float coeff_sin[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
		      int w, /* window size; = n in the FT */
		      int d  /* number of different frequencies to calculate */
		      )
    {
    /* This calculates the coefficients cos theta of the real components, and sin theta of
       the imaginary components, of the complex numbers which are multiplied by the
       per-residue (or per-base) scores, the mean of which produces the complex number
       representing the frequency k .

       That is, suppose that the FTs are to be done in a WINDOW of size w residues (or bases),
       irrespective of the sequence length. For these purposes, n = w, where n is the
       length of the sub-sequence.

       Therefore, if j iterates over all values of f, where f are the 'observations' (i.e.
       scores, each represented by f[j]), then the complex number for frequency k, x[k], is the
       mean of the following:

       		f[j].exp(2PI.ijk/n)

       - that is, sum all f[j].exp(2PI.ijk/n) for j = 0...n-1, and divide the result by n;
       the result is xk. This can be represented by:

       		( SUM (j = 0, j = n-1) f[j].exp(2PI.ijk/n) ) / n

       Remember this is for a * single frequency, k *. For a given frequency k, the only
       variable in f[j].exp(2PI.ijk/n) is j.

       Eulers formula means that f[j].exp(2PI.ijk/n) is equivalent to:

       		f[j].(cos (2PI.jk/n) + i.sin (2PI.jk/n))

       so the sum becomes:

       		( SUM (j = 0, j = n-1) f[j].(cos (2PI.jk/n) + i.sin (2PI.jk/n) ) ) / n

       So, the result is produced by independently summing the real parts (cos (2PI.jk/n))
       and the imaginary parts (sin (2PI.jk/n)). We aren't too bothered about the fact
       that i.sin (2PI.jk/n) is an imaginary number; as far as we are concerned, it - or
       rather, sin (2PI.jk/n) -  is simply a vector orthogonal to cos (2PI.jk/n), and when
       each of the former (sin (2PI.jk/n)) are summed to give a total ('y') vector, and
       each of the latter (cos (2PI.jk/n)) to give an orthogonal ('x') vector, the MAGNITUDE
       of this vector can be calculated with some simple trigonometry (as can its argument,
       which represents the PHASE).

       For * each frequency, k, * each combination of j and k is unique.

       However, because the sum must be done for * many frequencies *, each of the coefficients
       cos (2PI.jk/n) will arise many times; as will each of sin (2PI.jk/n). So, to save on
       calculation time, each value of:

       		cos (2PI.jk/n)
       and
       		sin (2PI.jk/n)

       will be calculated here and put into a table, to be looked up whenever needed instead of
       recalculated.

       So how many values of k will there be? Strictly speaking, k too should iterate over all
       integer values between 0 and n-1. However, that means that the number of frequencies
       calculated will vary with the (sub)sequence length; in this case, the number of different
       frequencies whose Fourier coefficients will be calculated will be equal to n, i.e. the
       size of the window - not a very large number (even if a single FT were being calculated
       for the whole sequence, many sequences would only give a few hundred frequencies).

       That is not nearly high enough resolution; we need periods to several decimal places
       (where the period is the (sub)sequence length divided by the frequency).

       Therefore, the approach is to always use a high (and constant) number of different
       frequencies, i.e. different values of k. There should be several thousand different
       values at least (to date I've usually used 4000 or 5000). The number of different
       values is d.

       This means that there will be fractional values of k; k will increment kDelta at a time,
       where kDelta = n / d.

       Therefore, the total number of different combinations of k and j required will be
       d multiplied by n (because there are d different values of k, and n different values of
       j); and therefore d.n values of cos (2PI.jk/n), and d.n values of sin (2PI.jk/n).

       Typical window sizes might be 30, 64 or 128 (really they ought to be odd-numbered so
       that each window can be represented by it's central residue on a plot). Suppose
       that w = 129 and d = 5000, then that's 645000 different values of EACH of the cosines
       AND sines; so, 1290000 floating-point numbers. If each is represented by a 4-byte
       float type, then that's 2580000 bytes (EACH), or about 2.46 Mb (each) - which isn't
       too bad; about 5 Mb in all.

       Each of coeff_cos[][] and coeff_sin[][] is indexed first by m and then by j; where
       m is an integer between 0 and d-1 inclusive (and k = m.kDelta).

       Note that nothing is done with the coefficients here; the 'observations' fj aren't
       used. Remember also that f[j] isn't the same as S[r], where r = residue number in the
       * whole sequence * (not subsequence), and S is the 'score' (observation) for that
       residue.
       
       That is, in the window starting at residue 0 of the entire sequence ('window 0'),
       f[j] is equivalent to S[j], where j iterates from 0 to n - 1 (i.e. w - 1); but
       in 'window 1', f[0] = S[1], f[1] = S[2], f[2] = S[3], ... f[n-1] = S[n];
       in 'window 2', f[0] = S[2], f[1] = S[3], f[2] = S[4], ... f[n-1] = S[n+1];
       in 'window 3', f[0] = S[3], f[1] = S[4], f[2] = S[5], ... f[n-1] = S[n+2], etc.
     */

    int j, m, n;

    float k,
          kdelta,
          two_pi_j_k_over_n,
          two_pi_over_n; /* constant: 2PI/n */

    n = w;

    two_pi_over_n = 2.0 * M_PI / (1.0 * n);

    kdelta = (1.0 * n) / (1.0 * d);

    k = 0.0;

    for (m = 0; m < d; m++)
        {


	for (j = 0; j < n; j++)
	    {
	    two_pi_j_k_over_n = two_pi_over_n * 1.0 * j * k;
	    coeff_cos[m][j] = cos( two_pi_j_k_over_n );
	    coeff_sin[m][j] = sin( two_pi_j_k_over_n );
	    }

        k += kdelta;

	}

    }

int slidingFT(int res_scores[MAX_FLOAT_COLUMNS+1][MAX_SEQUENCE_LENGTH],
               int col_exp[MAX_FLOAT_COLUMNS+1], /* logs (to base 10) of integer-producing correction factors */
	       int n_cols,
	       int n_observations,
               float coeff_cos[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
               float coeff_sin[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
	       int window_size, /* window size = n in each FT */
	       int d,  /* number of different frequencies to calculate (if none were being excluded by range) */
	       float min_period, /* lowest end of range and ...                                      */
	       float max_period,  /* ... highest end of range defining which periods to perform FT on */
	       int stats_window_size, /* */
	       float sigma_max, /* segments whose mean strongest-frequency has an S.D. <= this number will be reported */
	       float target_period,  /* if specified (i.e. non-zero), then along with target_period_range, this */
	       float target_period_range, /* specifies the frequency (and therefore period) of interest */
	       FILE *peaks_file,
	       FILE *stats_file,
	       int output_logic,
	       char sequence_label[MAX_SEQUENCE_AC_LENGTH] /* sequence 'identifier' (but usually acc_no, not ID!) */
)
    {
    int c,          /* counter for columns; n_cols different FTs are done independently */
        fj,         /* the observation (hydrophobicity score) for residue j in each window */
        i,          /* counter for calculating stats from data stored in running_top_frequency[][] */
        j,          /* counter for position in window */
	m,          /* counter for frequency (each frequency is not necessarily an integer) */
	n_windows,  /* the total number of windows for which FT will be done, for this sequence */
	w,          /* counter for window in sequence */
	window_centre, /* used as the x-coordinate for output */
        half_window_size, /* what it says; to avoid several recalculations */
	stats_window_centre, /* used as the x-coordinate for output */
        stats_table_index, /* the current element no. in the running_top_frequency[][] table */
        segment_start[MAX_FLOAT_COLUMNS+1], /* segment_start and segment_end mark the limits of any segment which conforms */
	segment_end[MAX_FLOAT_COLUMNS+1],   /* to the S.D. (and maybe mean as well) limits; if segment_start < -1, this means
	                  that currently the window is not in a conforming segment */
        n_hit_segments; /* the number of hits from ANY scoring scheme; returned as the result of the function */

    float k,         /* k and kdelta are not needed here to do any calculations, because the */
          kdelta,    /* cos and sin coeffs for each value of k (indexed by the integer m) have */
	             /* been precalculated */
	  period,
	  Xk_cos[MAX_FLOAT_COLUMNS+1],    /* these are the Fourier coefficients for frequency k; */
	  Xk_sin[MAX_FLOAT_COLUMNS+1],    /* separate calculations are done for each hydrophobicity
	                                     scoring scheme  */

          amplitude, /* amplitude of the current column's (scoring scheme's) Fourier coefficient
	                for a particular frequency */
	  phase,     /* phase of the current column's (scoring scheme's) Fourier coefficient
	                for a particular frequency */

                  /* the next three variables represent the frequency ('max_Xk'), within the specified range,
		     (i.e. corresponding period between min_period and max_period) that has the
		     highest Fourier amplitude; of course, as the calculations are independent for
		     each scoring scheme, there is an independent max_Xk for each scheme (column) */
	          max_Xk_k[MAX_FLOAT_COLUMNS+1],    /* max_Xk itself, i.e. the frequency with highest F amplitude */
	  max_Xk_amplitude[MAX_FLOAT_COLUMNS+1],    /* the highest F amplitude, i.e. amplitude of max_Xk          */
	      max_Xk_phase[MAX_FLOAT_COLUMNS+1],    /* the phase of max_Xk (not really of much interest)          */

          running_top_frequency[MAX_STATS_WINDOW_SIZE][MAX_FLOAT_COLUMNS+1],
          running_sum[MAX_FLOAT_COLUMNS+1],
          running_mean,  /* this is the mean FREQUENCY calculated from the current stats window */
	  running_mean_period, /* this is the PERIOD equivalent to running_mean (frequency) */
	  difference_squared_sum,
	  sigma,
	  segment_min_mean_period[MAX_FLOAT_COLUMNS+1], /* a 'hit' segment (min length is one stats window) is where the S.D. of the */
	  segment_max_mean_period[MAX_FLOAT_COLUMNS+1], /* strongest frequency is within sigma_max (and if specified, the target */
	                                                /* period criteria are met); so, a segment effectively is specified by one or
				                        /* more consecutive stats windows; the mean period within each window may of
				                        /* course not be identical, so these two variables specify the observed range */
          segment_min_sigma[MAX_FLOAT_COLUMNS+1],       /* ditto a record of the range of S.D.s observed over a segment */
    /* MODIFIED 4-8-7 0.2->0.3. Added float variable infinite_period . */
	  segment_max_sigma[MAX_FLOAT_COLUMNS+1],
	  infinite_period = 9999.9;

    n_hit_segments = 0; /* this variable is the result of the function */

    kdelta = (1.0 * window_size) / (1.0 * d);

    n_windows = 1 + n_observations - window_size;

    half_window_size = window_size / 2; /* an odd window_centre gives the correct integer answer */

    window_centre = half_window_size; /* window_centre is incremented; this is the initial value */

    /* Each stats window is represented by the residue (or base) in the centre of the entire
    span which contributes to that stats window. The final FT window which contributes to
    that span STARTS at residue w, and ends at residue w + window_size - 1. The FIRST FT window
    which contributes to the span STARTS at residue w + 1 - stats_window_size; that residue
    therefore is the first one which contributes to the current stats window. The current stats
    window's centre is therefore the midpoint between w + 1 - stats_window_size and
    w + window_size - 1. 

    i.e. thats:
    
    w + 1 - stats_window_size + 

    ( w + window_size - 1 - w - 1 + stats_window_size ) / 2 
    
    that is

    w + 1 - stats_window_size + (window_size -2 + stats_window_size) / 2
    
    = w - stats_window_size/2 + window_size/2 

    Perhaps a more intuitive way to consider it, is to use the first residue of the most recent
    FT window (residue w) and subtract half the size of the stats window (i.e. stats_window_size / 2)
    and then add half of the size of the FT window (i.e. window_size/2).

    The easiest way to deal with this is to calculate the value when w = 0, and then just increment
    by 1 each time w increases by 1.  Of course, the early values (when w < stats_window_size - 1)
    will never be used. Also remember that w starts at 0, and so the centre of the FT window, and
    indeed of the stats window, will be such that residue (or base) numbering starts at 0, not 1.
    This is only corrected (i.e. by adding 1) when the data is output.

    */

    /* MODIFIED 4-8-7 0.2->0.3. Added assignment of infinite_period . */

    infinite_period = 9999.9;

    stats_window_centre = ( window_size - stats_window_size ) / 2;

    stats_table_index = 0;

    for (c = 0; c < n_cols; c++)
        {
	running_sum[c]   = 0.0;
        segment_start[c] =  -1;
	}

    for (w = 0; w < n_windows; w++)  /* window loop */
        {

        k = 0.0;
	/* MODIFIED 4-8-7 0.2->0.3. Changed assignment of period from 9999.9 to infinite_period . */
        period = infinite_period; /* represents INFINITY */

        /* we want to know the frequency, within the current window, which has the largest
	   amplitude; this is recorded independently for each scoring-table column */

        for (c = 0; c < n_cols; c++)
            {
	            max_Xk_k[c] = 0.0;
	    max_Xk_amplitude[c] = 0.0;
	        max_Xk_phase[c] = 0.0;
            }


        for (m = 0; m < d; m++)      /* frequency loop */
            {

            if (k > 0.0)
	        {
		period = window_size / k;
		}

            if ( (period < min_period) || (period > max_period))
	        {
		k += kdelta; /* ready for the next frequency iteration */
		continue;
		}

            for (c = 0; c < n_cols; c++)
	        {
	        Xk_cos[c] = 0.0; /* represents the real part of the accumulating complex number      */
                Xk_sin[c] = 0.0; /* represents the imaginary part of the accumulating complex number */
		}

            for (j = 0; j < window_size; j++)    /* observation (hydrophobicity score) loop */
	        {

                for (c = 0; c < n_cols; c++)
	            {
                    fj = res_scores[c][w+j]; /* remember this is an INTEGER */

		    Xk_cos[c] += 1.0 * fj * coeff_cos[m][j];
		    Xk_sin[c] += 1.0 * fj * coeff_sin[m][j];
		    }

		}


            /* at this point, the total (which is a complex number, but here expressed as
	       Xk_cos[c] + i.Xk_sin[c]) should be divided by the number of observations
	       ( i.e. the window size); however, this might as well wait until the
	       magnitude of the complex number (using simple Pythagoras) is determined;
	       the single magnitude can be divided by the window size to give the correct
	       answer. The 'argument' represents the phase, which will be unaffected by
	       the magnitude. (The 'magnitude' is the amplitude of the sine wave.)
	       Also it is necessary to correct for the fact that each set of scores has
	       been SCALED UP so that each score is an integer, to speed up the calculation;
	       again this can be applied at the end, to the magnitude.
	    */

            /* columns are window(centre), frequency, period */

            for (c = 0; c < n_cols; c++)
	        {

		/* this is the as-yet uncorrected amplitude */
                amplitude = sqrt( pow(Xk_cos[c],2) + pow(Xk_sin[c],2) );

		phase = atan2( Xk_cos[c], Xk_sin[c] );

                amplitude /= (window_size * pow(10,col_exp[c]));

                if (amplitude > max_Xk_amplitude[c])
		    {
		    max_Xk_k[c] = k;
		    max_Xk_amplitude[c] = amplitude;
		    max_Xk_phase[c] = phase;
		    }

                }


	    k += kdelta; /* ready for the next frequency iteration */
	    }

        /* All the FT frequencies (within min_period...max_period) have now been calculated for
	   THE CURRENT FT-WINDOW, and the magnitude of the strongest peak has been recorded, along
	   with the frequency at which it occurs; so too has the phase of this peak, although
	   that isn't used for anything. Note that these 3 values have been recorded independently
	   for EACH SCORING SCHEME (indexed by c).
	   
	   Before proceeding, write the appropriate FT window-centre and/or stats window-centre to
	   the appropriate file(s): */


        if (output_logic)
	    {
	    if (output_logic != 2)
                fprintf(peaks_file,"%5d",window_centre + 1); /* internally, the residues (or bases)
	                                                    start at 0, not 1; so correct this
							    for the output */

            if ((output_logic > 1) && (w > stats_window_size - 2))
	        fprintf(stats_file,"%5d",stats_window_centre + 1); /* ditto above re correction */
	    }


       /* Then, review the data (regarding strongest FT peak in the window) FOR EACH SCORING SCHEME
          INDEPENDENTLY. If requested, write out the data row to the appropriate file(s) (one for
	  max-peak of the window, one for the stats of the latest stats window). As the stats
	  window and FT window may well be different sizes, there may be some early FT windows
	  calculated which still do not provide sufficient data points (i.e. observed max-peaks)
	  to perform statistics. */


       for (c = 0; c < n_cols; c++)
	    {

            /* irrespective of what the frequency of the strongest peak is, write it (also
	       expressed as a period) and it's magnitude to the peaks output file, if one
	       has been requested */

            if (output_logic && (output_logic != 2))
	        {
		/* MODIFIED 4-8-7 0.2->0.3. Replaced unconditional fprintf with conditional, depending on zero or non-zero
		score.
		MODIFICATION BEGIN */
/* MODIFIED 6-11-7 0.3->0.4. Used variable max_Xk_phase[c] instead of running_mean in expression. */
		if (max_Xk_phase[c] == 0.0)
		    {
/* MODIFIED 6-11-7 0.3->0.4. Added 4th field to fprintf formatter field, and corresponding 4th argument to print (max_Xk_phase[c]). */
		    fprintf(peaks_file,"\t%8.4f %8.4f %8.4f %8.4f", max_Xk_k[c], infinite_period, max_Xk_amplitude[c], max_Xk_phase[c]);
		    }
		else
		    {
/* MODIFIED 6-11-7 0.3->0.4. Added 4th field to fprintf formatter field, and corresponding 4th argument to print (max_Xk_phase[c]). */
		    fprintf(peaks_file,"\t%8.4f %8.4f %8.4f %8.4f", max_Xk_k[c], window_size / max_Xk_k[c], max_Xk_amplitude[c], max_Xk_phase[c]);
		    }
		/* MODIFICATION END */
		}

  	    running_sum[c] += max_Xk_k[c];

	    if (w > stats_window_size - 2)
                {
		/* can calculate stats */

	        if (w >= stats_window_size)
                    {
		    running_sum[c] -= running_top_frequency[stats_table_index][c];
	            }

                running_mean = running_sum[c] / stats_window_size;

		difference_squared_sum = 0;
		for (i = 0; i < stats_window_size; i++)
		    {
		    difference_squared_sum += pow(running_top_frequency[i][c] - running_mean, 2);
		    }
		sigma = sqrt(difference_squared_sum / stats_window_size);

                /* MODIFIED 4-8-7 0.2->0.3. Replaced unconditional with conditional assignment of running_mean_period. 
		MODIFICATION BEGIN */
                if (running_mean == 0.0)
		    {
		    running_mean_period = infinite_period;
		    }
                else {
                    running_mean_period = window_size / running_mean;
		    }
		/* MODIFICATION END */

                /* irrespective of what the mean and S.D. are (of the frequency of the strongest peak),
		   write them to the stats output file, if one has been requested; the frequency is
		   also expressed as a period */

                if (output_logic > 1)
		    {
		    fprintf(stats_file,"\t%8.4f %8.4f %8.4f", running_mean, running_mean_period, sigma);
		    }

                /* writing details of any sequence segment(s) which conform to the S.D. limits (and to the
		   mean-range limits if imposed) is of course conditional */
		   
                if ( (sigma <= sigma_max)
		    && ( ( target_period == 0.0 ) || 
		         ( fabs(running_mean_period - target_period) <= target_period_range )
		       )
		   )
		   {
		   /* this is part of a 'hit' segment */
		   if (segment_start[c] < 0)
		       {
		       /* it's the start of a new 'hit' segment */
		       /* the limits of the segment are all residues which contribute to the stats
			  window(s) contributing to that segment */
		       segment_start[c] = w + 1 - stats_window_size;
		       /* segment_start = w; */
                       /* end of segment is w + window_size - 1 */
		       segment_min_mean_period[c] = running_mean_period;
		       segment_max_mean_period[c] = running_mean_period;
		       segment_min_sigma[c] = sigma;
		       segment_max_sigma[c] = sigma;
		       n_hit_segments++;
		       }
		   else
		       {
		       /* we are already in a 'hit' segment */
		       if (running_mean_period < segment_min_mean_period[c])
			   {
			   segment_min_mean_period[c] = running_mean_period;
			   }
		       else if (running_mean_period > segment_max_mean_period[c])
			   {
			   segment_max_mean_period[c] = running_mean_period;
			   }
		       if (sigma < segment_min_sigma[c])
			   {
			   segment_min_sigma[c] = sigma;
			   }
		       else if (sigma > segment_max_sigma[c])
			   {
			   segment_max_sigma[c] = sigma;
			   }
		       }
		   }
		else /* this is not a 'hit' segment */ if (segment_start[c] >= 0)
		    {
		    /* this is therefore the END of a segment, which ended at the previous FT-window */
		    /* end of segment is (w + window_size - 1) for the PREVIOUS value
		       of w, i.e. w + window_size - 2 */
		    printf("%s\tscheme %1d\t%5d to %5d (window centres: FT %5d to %5d, stats %5d to %5d): mean period from %10.5f to %10.5f; S.D. from %10.5f to %10.5f\n",
		           sequence_label,
			   c+1,
			   segment_start[c],
			   w + window_size - 1,
			   segment_start[c] + half_window_size + 1,
			   w + half_window_size,
			   segment_start[c] + 1 + stats_window_size/2 + half_window_size,
			   w - stats_window_size/2 + half_window_size,
			   segment_min_mean_period[c],
			   segment_max_mean_period[c],
			   segment_min_sigma[c],
			   segment_max_sigma[c]);
	            segment_start[c] = -1;
		    }
		} /* end of block: if (w > stats_window_size - 2)  - i.e., stats-calculating block */

            running_top_frequency[stats_table_index][c] = max_Xk_k[c];

            } /* end of c loop, i.e. iterates over all the scoring schemes */

        if (output_logic)
	    {
            if (output_logic != 2) fprintf(peaks_file,"\n");
            if ( (output_logic > 1) && (w > stats_window_size - 2) ) fprintf(stats_file,"\n");
	    }


        /*stats_table_index = next_stats_table_index;*/

	if (++stats_table_index == stats_window_size) stats_table_index = 0;

	window_centre++; /* ready for the next window iteration */
	stats_window_centre++; 
	}

    /* it's possible that a 'hit' segment extends right to the end of the sequence, so deal with
       the unfinished hit, if there is one */

    for (c = 0; c < n_cols; c++)
        {
        if (segment_start[c] > -1)
            printf("%s\tscheme %1d\t%5d to %5d (window centres: FT %5d to %5d, stats %5d to %5d): mean period from %10.5f to %10.5f; S.D. from %10.5f to %10.5f\n",
                sequence_label,
		c+1,
                segment_start[c],
                w + window_size - 1,
	        segment_start[c] + half_window_size + 1,
	        w + half_window_size,
	        segment_start[c] + 1 + stats_window_size/2 + half_window_size,
	        w - stats_window_size/2 + half_window_size,
                segment_min_mean_period[c],
                segment_max_mean_period[c],
                segment_min_sigma[c],
                segment_max_sigma[c]);

        /* note that any 'sequence-terminal' hits will already have been counted (in n_hit_segments), because
	   the count is done when a segment begins, not when it ends */
        }

    return n_hit_segments;
    }

void display_license(
    /* MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from display_license() definition. */
    /* MODIFIED 25-7-7 0.1->0.2. Added new 4th argument 'char author_email[]' to display_license() definition. */
    char program_name[],
    char program_date[],
    char program_author[],
    /* ,char copyright */
    char author_email[]
    )
    {
    printf("\n");
    /* MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from printf() call,
                                 and adjusted first argument (formatting string) accordingly. */
    printf("SOFTWARE LICENSE for %s\t%s\t(C) %s\n\n",program_name,program_date,program_author);
    printf("This program is free software: you can redistribute it and/or modify it under\n"
           "the terms of the GNU General Public License as published by the Free Software\n"
           "Foundation, either version 3 of the License, or (at your option) any later\n"
           "version.\n\n"
           "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
           "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
           "PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n"
           "You should have received a copy of the GNU General Public License along with\n"
           "this program. If not, see <https://www.gnu.org/licenses/>.\n\n" 
    );
/* MODIFIED 25-7-7 0.1->0.2. Added 1 printf() call to end of display_license(). */
    printf("John Walshaw, July 2007. \t\t%s\n\n",author_email);
    }

/*

Change log.

MODIFIED 25-7-7 0.1->0.2. Added int variable, ignore_sequences.
MODIFIED 25-7-7 0.1->0.2. Removed 4th arg 'copyright' from display_license() call.
MODIFIED 25-7-7 0.1->0.2. Added new 4th arg 'author_email' to display_license() call.
MODIFIED 25-7-7 0.1->0.2. Added 25th argument (&ignore_sequences) to interpret_arguments() call.
MODIFIED 25-7-7 0.1->0.2. Added 16 lines: if (ignore_sequences) {...} else {...}
MODIFIED 25-7-7 0.1->0.2. Added 5 lines: if (ignore_sequences) {...}
MODIFIED 25-7-7 0.1->0.2. Added 25th argument (int *ignore_sequences) to interpret_arguments() definition.
MODIFIED 25-7-7 0.1->0.2. Added 2 lines to function interpret_arguments(), to determine *ignore_sequences.
MODIFIED 25-7-7 0.1->0.2. Parenthesized *output_logic in (*output_logic) += 1.
MODIFIED 25-7-7 0.1->0.2. Parenthesized *output_logic in (*output_logic) += 2.
MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from display_license() definition.
MODIFIED 25-7-7 0.1->0.2. Added new 4th argument 'char author_email[]' to display_license() definition.
MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from printf() call
                          and adjusted first argument (formatting string) accordingly.
MODIFIED 25-7-7 0.1->0.2. Added erroneously omitted argument, program_name, from printf() call.
MODIFIED 25-7-7 0.1->0.2. Added 1 printf() calls to end of display_license().

MODIFIED 4-8-7 0.2->0.3. Added float variable infinite_period .
MODIFIED 4-8-7 0.2->0.3. Added assignment of infinite_period .
MODIFIED 4-8-7 0.2->0.3. Changed assignment of period from 9999.9 to infinite_period .
MODIFIED 4-8-7 0.2->0.3. Replaced unconditional fprintf with conditional, depending on zero or non-zero score.
MODIFIED 4-8-7 0.2->0.3. Replaced unconditional with conditional assignment of running_mean_period .

MODIFIED 6-11-7 0.3->0.4. Changed 2 paragraphs of explanatory text written to peaks file, re the new 'phase' column(s).
MODIFIED 6-11-7 0.3->0.4. Added 4th field to fprintf formatter field, and corresponding 4th argument to print (max_Xk_phase[c]).
MODIFIED 6-11-7 0.3->0.4. Added 4th field to fprintf formatter field, and corresponding 4th argument to print (max_Xk_phase[c]).
MODIFIED 6-11-7 0.3->0.4. Used variable max_Xk_phase[c] instead of running_mean in expression.

MODIFIED 6-4-8 0.4->0.5  Changed end date of authorship period (in two places).

*/
