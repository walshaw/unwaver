/*  prototype.h
    -----------

SOFTWARE LICENSE for UNWAVER versions 0.1 to 0.6	24-7-7 to 17-4-8
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

The file you are reading is version 0.2 of prototype.h, part of UNWAVER
versions 0.2 - 0.7.

*/

#ifndef PROTOTYPE
#define PROTOTYPE

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>

FILE *open_file(char file_name[], char mode[], int report);

void fatal(char fatal_string[]);

int next_sequence(FILE *sequence_file, enum boolean ncbi_format, int acc_field, int id_field, char acc_no[], char id[], char description[], char sequence[]);

int set_arguments(int argc, char *argv[], char *flagname[], char *parname[], char *implicit[],
                  char *flaglabel[], char *parlabel[], enum boolean setflag[], char *par[]);

int limit_arguments(enum boolean setflag[], char *parname[], char *parlabel[], char *par[],
                    char *partype[], char *parmin[], char *parmax[], char *pardefault[]);

int check_arguments(enum boolean setflag[], char *parname[], char *parlabel[], char *par[], char *partype[]);

/* MODIFIED 25-7-7 0.1->0.2. Added 25th argument (int *ignore_sequences) to interpret_arguments() prototype. */

void interpret_arguments(enum  boolean setflag[],
                         char  *par[],
			 char  infile_hyd_name[],
			 int   *commenter,
                         int   *label_column,
			 int   *n_columns,
			 int   column_order[],
			 /* float *default_value, */
			 int   *default_value,
			 char  valid_code[],
			 char  ignored_code[],
			 char  on_code[],
			 int   *on_value,
			 int   *off_value,
                         int   *n_frequencies,
                         int   *window_size,
                         float *min_period,
                         float *max_period,
                         int   *stats_window_size,
                         float *sigma_max,
                         float *target_period,
                         float *target_period_range,
                         char  infile_seq_name[],
                         int   *acc_field,
			 int   *id_field,
                         int   *ignore_sequences, 
                         char  outfile_peaks_name[],
                         char  outfile_stats_name[],
                         int   *output_logic,
			 int   *report_frequency   
			);

void show_int_col(int column[], int n_rows);

void calculate_coeffs(float coeff_cos[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
                      float coeff_sin[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
		      int w, /* window size; = n in the FT */
		      int d  /* number of different frequencies to calculate */
		      );

int slidingFT(int res_scores[MAX_FLOAT_COLUMNS+1][MAX_SEQUENCE_LENGTH],
               int col_exp[MAX_FLOAT_COLUMNS+1],
	       int n_cols,
	       int n_observations,
               float coeff_cos[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
               float coeff_sin[MAX_N_FREQUENCIES][MAX_WINDOW_SIZE],
	       int window_size, /* window size = n in each FT */
	       int d,  /* number of different frequencies to calculate */
	       float min_period, /* lowest end of range and ...                                      */
	       float max_period,  /* ... highest end of range defining which periods to perform FT on */ 
               int stats_window_size,
               float sigma_max,
	       float target_period,
	       float target_period_range,
	       FILE *peaks_file,
	       FILE *stats_file,
	       int output_logic,
	       char sequence_label[MAX_SEQUENCE_AC_LENGTH]
              );

int split(char delimiter,
          char string[MAX_SPLIT_STRING_LENGTH],
	  int n_substrings,
	  int start_char,	/* first char of a string is char 0 */
	  enum boolean plus,    /* if true, then multiple consecutive delimiters are treated as a single delimiter */
	  char substring[MAX_SPLIT_SUBSTRINGS][MAX_SPLIT_SUBSTRING_LENGTH]	/* results, i.e. an array of strings */
	  );

int read_table_float(FILE *data_file,	/* input; file containing whitespace-delimited columns */
               int start_at_line,	/* input; the number of the first NON-COMMENT line of the file to be processed;
	                                   note that numbering starts from 1 */
               int end_at_line,		/* input; the number of the last NON-COMMENT line of the file to be processed;
	                                   note that numbering starts from 1; a value of 0 means that the
					   processing will continue until the last line of the file */
               int read_columns[],        /* input; array specifying which column(s) of the data file are to be read,
	                                   and in what order; see data[]; columns are specified by the client
					   code and not ultimately by the user, so column numbering starts at 0 */
	       int label_column, 	/* input; the column number (first column is column 0) to be read as a string */
               char comment_symbol,     /* any row which begins with 0 or more whitespace character(s) followed by
	                                   this character is treated as a COMMENT line and is ignored */
               enum boolean ignore_blank, /* */
	       char termination_string[MAX_TERM_STRING_LENGTH],   /* if string this is the FIRST NON WHITESPACE-CONTAINING STRING of any line
	                                    then that line and all lines after it are ignored */
	       float data[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
	                                /* output; array containing the columns of data read in; these
	                                   are present in data[] in the order specified by read_columns; e.g.
					   if read_columns = {4,6,1} then data from column 4 of the file is
					   read as data[0][...], column 6 of the file is read as data[1][...],
					   and column 1 of the file is read as data[2][...]; remember, column
					   numbering always starts at 0! */
               char row_label[MAX_FLOAT_ROWS][MAX_LABEL_LENGTH], /* output; array containing the string label of each processed row */
	       int verbose              /* input; if non-zero, produces some commentary re skipped lines etc */
	);

int blank_line(char line[]);

int comment_line(char line[], int comment_symbol);

void table_float_to_int(float fdata[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
                        int n_columns,
			int n_rows,
                        int idata[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
			int column_exponent[MAX_FLOAT_COLUMNS]);
			
int char_in_string(char string[], char c);

void clean_int_column(int data[], int size);

void clean_float_column(float data[], int size);

int invalid_sequence(char sequence[], char valid_code[]);

int clean_sequence(char new_sequence[],
                    char sequence[],
		    char ignored_characters[],
		    enum boolean report_ignored_chars);

void binary_score_sequence(char sequence[],
                           char on_code[],  /* string representing 'on' codes */
			   int on_value,
			   int off_value,
                           int binary_res_score[]
                          );

void scale_score_sequence(char sequence[],
                          char label_code[],
			  int hydrophobicity_int[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
                          int n_columns,
			  int default_score,
			  int alpha_to_table_lookup[],
                          int scale_res_scores[MAX_FLOAT_COLUMNS][MAX_SEQUENCE_LENGTH]
                          );
void display_license(
    /* MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from display_license() prototype. 
       MODIFIED 25-7-7 0.1->0.2. Added new 4th argument 'char author_email[]' to display_license() prototype. */
    char program_name[],
    char program_date[],
    char program_author[],
    /* ,char copyright */
    char author_email[]
    );

#endif

/*

Change log.

MODIFIED 25-7-7 0.1->0.2. Added 25th argument (int *ignore_sequences) to interpret_arguments() prototype.
MODIFIED 25-7-7 0.1->0.2. Removed 4th argument 'char copyright' from display_license() prototype.

*/
