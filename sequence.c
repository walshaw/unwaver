/*  sequence.c
    ----------

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

The file you are reading is version 0.1 of sequence.c, part of UNWAVER
versions 0.1 - 0.6.

*/

#include "preproc.h"
#include "global.h"
#include "prototype.h"

int next_sequence(FILE *sequence_file,		/* input */
                  enum boolean ncbi_format,	/* input */
                  int acc_field,		/* input; if ncbi_format == false, then refers to number
		                                   of field in a space-delimited set of fields;  if
						   ncbi-format == true, then refers to number of 'subfield'
						   of field 0, where the subfields are delimited by '|' */
		  int id_field,			/* input; as acc_field */
                  char acc[],			/* output; accession 'number' */
		  char id[],			/* output; identifier string */
		  char description[],		/* output; description text */
		  char sequence[]		/* output; (DNA or protein) sequence */
		  ) {

    char textstring[DEFAULT_READ_LINE_LENGTH],
         error_string[MAX_ERROR_STRING_LENGTH],
	 field[3][MAX_SPLIT_SUBSTRING_LENGTH],
	 compound_field[MAX_SEQUENCE_NCBI_LENGTH],
	 nulchar;

    static char fasta_header[MAX_FASTA_HEADER_LENGTH];

    enum boolean eof;

    int i, header_char_pos, field_char_pos;

    while (    ( (strlen(fasta_header) == 0) || (fasta_header[0] != '>') )
            && (fgets(fasta_header,MAX_FASTA_HEADER_LENGTH,sequence_file) != NULL) ) {

    }

    /* parse FASTA header line; various styles are catered for, FOR EXAMPLE:
       (i)   >compound_AC_ID description (if -n is set)
       (ii)  >ID description             (if -n is not set, and -id=1)
       (iii) >ID AC description		 (if -n is not set, -ac=2 and -id=1)
    */

    if (ncbi_format)
        {
	split(' ', fasta_header, 2, 1, true, field);
	strcpy(compound_field, field[0]);
	strcpy(description, field[1]);
	split('|', compound_field, 3, 0, false, field);
	strcpy(acc, field[acc_field-1]);    /* the user specifies (sub)fields starting from 1, not 0 */
	strcpy(id, field[id_field-1]);     /* the user specifies (sub)fields starting from 1, not 0 */
	}
    else
        {
	split(' ', fasta_header, 3, 1, true, field);
	strcpy(acc, field[acc_field-1]);    /* the user specifies (sub)fields starting from 1, not 0 */
	strcpy(id, field[id_field-1]);     /* the user specifies (sub)fields starting from 1, not 0 */
	strcpy(description, field[2]);     /* THIS SHOULD BE MADE USER-DEFINABLE */
        }

    strcpy(textstring,"");
    strcpy(sequence,"");

    while ( (textstring[0] != '>')
        && (fgets(textstring,DEFAULT_READ_LINE_LENGTH,sequence_file) != NULL) )
	{

	if (textstring[0] == '>')
	    {
	    strcpy(fasta_header,textstring);
	    }
	else {
	     if (strlen(sequence) + strlen(textstring) > MAX_SEQUENCE_LENGTH)
	        {
		sprintf(error_string,
		            "sequence length exceeds the maximum of %d residues or bases",
			    MAX_SEQUENCE_LENGTH);
	        fatal(error_string);
		}
	    strcat(sequence,textstring);

	    /* trim any end-of-line characters from the end of the sequence string */
	    while (sequence[strlen(sequence)-1] == 10) {
	        sequence[strlen(sequence)-1] = '\0';
		}
	    }
	}

    if (textstring[0] != '>') /* must be EOF */
        {
	strcpy(fasta_header,""); /* reset FASTA header (of next sequence) to null */
	}

    /* trim any terminal asterisk from the sequence string */
    while (sequence[strlen(sequence)-1] == 42)
        {
	sequence[strlen(sequence)-1] = '\0';
	printf("\tremoved asterisk from the end of the sequence\n");
        }

    return (strlen(sequence));

}


int invalid_sequence(char sequence[], char valid_code[])
    {
    int i,j,invalid_code,n_invalid_codes;

    n_invalid_codes = 0;

    for (i = 0; i < strlen(sequence); i++)
        {
	invalid_code = 1;
	for (j = 0; j < strlen(valid_code); j++)
	    {
	    if (sequence[i] == valid_code[j])
	        {
		invalid_code = 0;
		break;
		}
	    }
        if (invalid_code) n_invalid_codes++;
	}

    return n_invalid_codes;
    }


int clean_sequence(char new_sequence[],
                    char sequence[],
		    char ignored_characters[],
		    enum boolean report_ignored_chars)
    {

    /* returns 0 if it fails; otherwise it returns the length of the
       cleaned sequence */

    int sequence_length, i, j, n_ignored;
    char ignored_char[MAX_IGNORED_CHARACTERS];

    sequence_length = strlen(sequence);

    strcpy(new_sequence,"");
    n_ignored = 0;
    j = 0;

    for (i = 0; i < sequence_length; i++)
        {
	if (char_in_string(ignored_characters, sequence[i]) < 0)
	    {
	    /* the current residue/base code is not in the 'ignored' list */
	    new_sequence[j++] = sequence[i];
	    }
	else {
	    if (n_ignored == MAX_IGNORED_CHARACTERS)
	        {
		printf("\t!!! sequence contains too many non-processed characters:\n\n\t%s\n\n",
		       ignored_char);
		strcpy(new_sequence,"");
		return 0;
		}
	    ignored_char[n_ignored++] = sequence[i];
	    }
        }

    ignored_char[n_ignored] = '\0';
    new_sequence[j] = '\0';

    if (n_ignored && (report_ignored_chars == true))
        {
	printf("\tignored: %s\n", ignored_char);
	}

    return j;
    }


void binary_score_sequence(char sequence[],
                           char on_code[],                            /* string representing 'on' codes */
			   int on_value,
			   int off_value,
                           int binary_res_score[]
                          )
    {
    int i, sequence_length;

    sequence_length = strlen(sequence);

    for (i = 0; i < sequence_length; i++)
        {
	if (char_in_string(on_code, sequence[i]) < 0)
	    {
	    binary_res_score[i] = off_value;
	    }
	else {
	    binary_res_score[i] = on_value;
	    }
	}
    }

void scale_score_sequence(char sequence[],
                          char label_code[],
			  int hydrophobicity_int[MAX_FLOAT_COLUMNS][MAX_FLOAT_ROWS],
                          int n_columns,
			  int default_score,
			  int alpha_to_table_lookup[],
                          int scale_res_scores[MAX_FLOAT_COLUMNS][MAX_SEQUENCE_LENGTH]
                          )
    {
    int c,i, row, sequence_length;

    sequence_length = strlen(sequence);

    for (i = 0; i < sequence_length; i++)
        {
        for (c = 0; c < n_columns; c++)
	    {
	    row = alpha_to_table_lookup[sequence[i]];
	    if (row < 0)
	        {
		scale_res_scores[c][i] = default_score;
		}
	    else {
	        scale_res_scores[c][i] = hydrophobicity_int[c][row];
		}
	    }

	}
    
    }
