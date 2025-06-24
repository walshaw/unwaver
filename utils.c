/*  utils.c
    -------

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

The file you are reading is version 0.2 of utils.c, part of UNWAVER
versions 0.4-0.6.

*/

#include <stdlib.h>

#include "global.h"
#include "preproc.h"
#include "prototype.h"


FILE *open_file(char file_name[], char mode[], int report) {

    /* report == 1 means, report only a failure to open the file;
       report == 2 means, report success or failure */

    FILE *file_handle;
    char rw_string[2]; /* note, no operations of more than 1 character are
			  available, e.g. "rw" ; "Read/write modes are
			  currently not supported." (in C) */
    char read_or_write[10];

    rw_string[0] = mode[0];
    rw_string[1] = 0;

    if (!strcmp(rw_string,"r"))
        {
	strcpy(read_or_write, "reading");
	}
    else if (!strcmp(rw_string,"w"))
        {
	strcpy(read_or_write, "writing");
	}
/* MODIFIED 6-11-7 0.1->0.2. Corrected erroneous 'mode' char 'r', supplied to fopen(), to 'a'. */
    else if (!strcmp(rw_string,"a"))
        {
	strcpy(read_or_write, "appending");
	}
    else {
        printf("FAILED to open file '%s': unrecognized file-open mode '%s'\n", file_name, mode);
	return 0;
        }

    if ( (file_handle = fopen(file_name, rw_string) ) == NULL) {
        if (report) {
	    printf("FAILED to open file '%s' (for %s)\n",file_name,read_or_write);
	}
        return 0;
    }
    else {
        if (report > 1) {
	    printf("successfully opened file '%s' (for %s)\n",file_name,read_or_write);
	}

        return file_handle;
    }
}


void fatal(char fatal_string[]) {

    printf("\nFATAL ERROR: %s\n\n", fatal_string);
    exit(1);

}


void dumpstring(char string[MAX_DUMP_STRING_LENGTH])
    {
    int i;
    printf("dumping string: char no. 'char' (ord)\n");
    for (i = 0; i <= strlen(string); i++)
        {
	printf("\t%3d, '%c', %3d\n", i, string[i], string[i]);
	}
    }


int trim(char string[MAX_TRIM_STRING_LENGTH], char result_string[MAX_TRIM_STRING_LENGTH])
    {
    char trimmed_string[MAX_TRIM_STRING_LENGTH];
    int i, first_non_whitespace, final_non_whitespace, trimmed_chars;

    /* E.g. a string of length 5 has the string itself 4 characters long (characters
       0-3) and the fifth character (character 4) is '\0' */

    final_non_whitespace = strlen(string) - 1;

    while ((string[final_non_whitespace] < 33) && final_non_whitespace)
        final_non_whitespace--;

    trimmed_chars = strlen(string) - 1 - final_non_whitespace;

    first_non_whitespace = 0;

    while ((string[first_non_whitespace] < 33) && (first_non_whitespace < final_non_whitespace)) first_non_whitespace++;
    trimmed_chars += first_non_whitespace;

    for (i = first_non_whitespace; i <= final_non_whitespace; i++)
        {
	trimmed_string[i - first_non_whitespace] = string[i];
	}

    trimmed_string[i] = 0;

    strcpy(result_string, trimmed_string);
    return trimmed_chars;
    }

int split(char delimiter,
          char string[MAX_SPLIT_STRING_LENGTH],
	  int n_substrings,
	  int start_char,	/* first char of a string is char 0 */
	  enum boolean plus,    /* if true, then multiple consecutive delimiters are treated as a single delimiter */
	  char substring[MAX_SPLIT_SUBSTRINGS][MAX_SPLIT_SUBSTRING_LENGTH]	/* results, i.e. an array of strings */
	  )
    {
    int string_char_pos, substring_char_pos, substring_index;

    string_char_pos = start_char - 1;
    substring_char_pos = 0;
    substring_index = 0;	/* first substring is substring 0, etc */


    while (++string_char_pos < strlen(string))
        {
 
        if ( (substring_index < (n_substrings - 1)) && (string[string_char_pos] == delimiter))
	    {
	    substring[substring_index++][substring_char_pos] = '\0';

	    substring_char_pos = 0;

	    if (plus)  /* skip over any immediately following delimiters */
	        {
		while (string[string_char_pos+1] == delimiter) string_char_pos++;
		}
	    }
	else {
	    if (string[string_char_pos] != 10) {

	        substring[substring_index][substring_char_pos++] = string[string_char_pos];

	        }
	    }
        }


    substring[substring_index][substring_char_pos] = '\0';

    if (strlen(substring[substring_index])) /* this means that the original string DIDN'T end with a delimiter character;
                                               which means that substring_index currently has the value of
					       the index of the final string - i.e. one less than the total number of
					       strings read (e.g. if 5 strings have been read, they are 0,1,2,3 and 4,
					       and substring_index will be set to 4;
					       if the original string on the other hand DID end with a delimiter, then
					       substring_index will be set to one greater, anticipating the characters
					       for that string (which never arrived); in the above example, substring_index
					       would be set to 5, but substring[5] would be empty */
        {
	substring_index++;
	}
    return substring_index;
    }


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
	)
    {
    int line_no, column_no, max_column, row_no, i;
    char error_string[MAX_ERROR_STRING_LENGTH],
         line[DEFAULT_READ_LINE_LENGTH],
         string_data[MAX_FLOAT_ROWS + 1][MAX_SPLIT_SUBSTRING_LENGTH]; /* first, string-format copies of each row are obtained;
	                                     the float data and string label are read from them */


    /* determine the highest column number to be read */

    max_column = label_column;
    for (i = 0; i < READ_COLUMNS; i++) if (read_columns[i] > max_column) max_column = read_columns[i];

    row_no = 0; /* the row in the table (data[][]), not the line in the file */

    /* skip start_at_line - 1 lines */

    for (line_no = 1; line_no < start_at_line; line_no++) fgets(line, DEFAULT_READ_LINE_LENGTH, data_file);

    /* start processing the remaining lines */

    while ((!end_at_line || line_no <= end_at_line) && fgets(line, DEFAULT_READ_LINE_LENGTH, data_file))
        {
	if ((ignore_blank) && (blank_line(line)))
	    {
	    if (verbose) printf("IGNORING BLANK LINE\n");
	    }
	else 
	    {
	    if (comment_line(line,comment_symbol)) { if (verbose) printf("IGNORING COMMENT LINE\n");}
	    else {
	        trim(line,line);  /* remove leading and trailing spaces from line, returning
		                    the trimmed string as line */

                split(' ',        /* delimiter; currently, only whitespace-delimited columns are readable */
		      line,       /* to be split */
		      20, /* the number of split fields to be obtained */
		      0,          /* start at character 0 of line string */
		      true,          /* treat multiple consecutive delimiters as a single delimiter */
		      string_data /* results get put in here (2D array of strings) */
		      );

		if (!strcmp(string_data[0], termination_string))
		    break; /* abort while loop, i.e. abort reading of file */

		strcpy(row_label[row_no],string_data[label_column]);

                for (i = 0; i < READ_COLUMNS; i++) {

                    /* check that the maximum number of rows isn't being exceeded */
		    if (row_no >= MAX_FLOAT_ROWS)
		        {
                        sprintf(error_string,
			        "maximum number of rows (%d) exceeded (you might need to use -sskip?)",
			        MAX_FLOAT_ROWS);
                      	fatal(error_string);
			}

		    sscanf(string_data[read_columns[i]], "%f", &data[i][row_no]);


		    }
	        row_no++;
	        }
	    }
	line_no++;
	}
    return row_no;
    }


int blank_line(char line[])
    {
    int result, i;
    result = 1;

    if (strlen(line) > 1) /* string length includes the final '\0' character; so an empty string has length 1 */
        {
	i = 0;
	while ((line[i] <= 32) && (i < strlen(line)-1)) i++;
	if (line[i] > 32) result = 0;
	}
    return result;
    }


int comment_line(char line[], int comment_symbol)
/* if comment_symbol is declared as a char, not an int, then the compiler says something like:
utils.c:147: warning: type mismatch with previous implicit declaration
utils.c:114: warning: previous implicit declaration of `blank_line'
*/
    {
    int result, i;

    result = 0;

    i = 0;

    while ( (i < strlen(line)-1) && ((line[i] == 32) || (line[i] == 8) ) ) 
	i++;

    if ((i < strlen(line)) && (line[i] == comment_symbol)) result = 1;

    return result;
    }


void table_float_to_int(float fdata[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
                        int n_columns,
			int n_rows,
                        int idata[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
			int column_exponent[MAX_FLOAT_COLUMNS])
    {
    int i, j;
    float scaled_up;

    /* set all the column-scale factor-exponents to default (i.e., means no change to values) */

    for (j = 0; j < n_columns; j++) column_exponent[j] = 0;

    for (i = 0; i < n_rows; i++)
        {
	for (j = 0; j < n_columns; j++)
	    {
	    /* determine required scale factor (expressed as exponent to the base 10) to
		turn the value into an integer */

            scaled_up = fdata[j][i] * pow(10,column_exponent[j]);

/* you CAN'T just test for:
  (scaled_up != floorf(scaled_up) )
  or
  (scaled_up != roundf(scaled_up) )
  or
  (scaled_up != rintf(scaled_up) )
  - because treating these as floats or indeed doubles, you get things like 119.000 != 119.000
  (because they are
   actually different if you look at a lot more decimal places; even if you don't use pow()
   and just multiply up by 1, then 10, then 100 etc, the same problem occurs */

            /* DEBUGGING  printf("\tdifference: %20.10f\n", fabs(scaled_up - rintf(scaled_up))); */

            while (fabs(scaled_up - rintf(scaled_up)) > 0.00001) /* due to float/double
		                                                       arithmetic weirdness;
								       actually 0.00001 may be too small,
								       but so far I've only seen a difference
								       of 'zero' come to about 0.0000075 */
		{
		/* DEBUGGING printf("* column_exponent[%d] = %d; %8.3f versus %8.3f\n",
		        j, column_exponent[j],scaled_up, rintf(scaled_up)); */

		scaled_up = fdata[j][i] * pow(10,++column_exponent[j]);

		                          /* Note the increment of column_exponent[j]
					     - so, this will be the FIRST power tried for the NEXT row of
		                             data in this column; point is, the exponent has to make
					     the datum with the most non-zero decimal places into an integer,
					     so it doesn't matter if it is unnecessarily large for another
					     datum in the same column */

                /* DEBUGGING printf("\tcolumn_exponent[%d] increased to %d\n", j, column_exponent[j]);
                printf("\tdifference: %20.10f\n", fabs(scaled_up - rintf(scaled_up)));
		*/
	        }

                /* DEBUGGING printf("\tFINAL column_exponent[%d] = %d; %12.8f versus %12.8f\n",
		            j, column_exponent[j],scaled_up, rintf(scaled_up));
                      printf("\tFINAL difference: %20.10f\n", fabs(scaled_up - rintf(scaled_up)));
                */

	    /* DEBUGGING printf("\n"); */
	    }
        }

    /* now scale up the float data table to produce the int data table */

    for (i = 0; i < n_rows; i++)
        {
	for (j = 0; j < n_columns; j++)
	    {
	    idata[j][i] = rintf(fdata[j][i] * pow(10,column_exponent[j]));
	    /* DEBUGGING printf("idata[%2d][%2d] is now %3d\n",j,i,idata[j][i]); */
            }
	}
    }

int char_in_string(char string[], char c)
    {
    /* Surely reinventing a wheel? Perhaps not - bizarrely, the GNU C compiler on this linux
       box (2.2.17-21mdk) doesn't have a strpos() function, in any string.h or strings.h
       header anywhere.
       It does however have a strchr() function, where what is returned is a string marking
       the substring beginning with the character position where the specified character is
       first found; e.g. strchr("napoleon",'l') returns "leon". By using the integer values
       of the string and substring (after all they are just pointers) the character position
       can be returned, or NULL if it is not present anywhere.

       You must include <string.h> for strchr to work correctly!
    */

    char *s;
    int pos;

    s = strchr(string,c);

    pos = s - string;
    if (s == NULL)
        {
	return -1;
	}
    else {
        return pos;
	}
    }

void clean_int_column(int data[], int size)
    {
    int i;
    for (i = 0; i < size; i++)
        {
	data[i] = 0;
	}
    }

void clean_float_column(float data[], int size)
    {
    int i;
    for (i = 0; i < size; i++)
        {
	data[i] = 0.0;
	}
    }


/*

Change log.

MODIFIED 6-11-7 0.1->0.2. Corrected erroneous 'mode' char 'r', supplied to fopen(), to 'a'.

*/
