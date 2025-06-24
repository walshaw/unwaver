/*					argument.c
					----------

SOFTWARE LICENSE for UNWAVER versions 0.1 to 0.7	24-7-7 to 7-4-8
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
    
The file you are reading is version 0.1 of argument.c, part of UNWAVER
versions 0.1 - 0.7.


*/

/* This function is used to set 'flags' (which are either on or off) and
	'parameters' (which have a value) used by the main function of a
	C program. The user executing the program can specify which flags
	are set, and the parameter values, like this:
		> cprogname -a -blah -c 20 -val thingy
	- the above would set the flags called 'a' and 'blah' to true, and
	would give the parameter 'c' the value '20', and 'val' the value
	'thingy' .
	How these flags and parameters are referenced from within your program
	is partly a matter of choice, but a way of doing it is described
	below.
	Currently, all parameter *values* are strings (arrays of characters).
	If you want to use them in a different context, then you must convert
	them yourself in your C program using the atoi(), atof() or atol()
	functions for example.
	Later versions of this routine may allow specification of the desired
	type of each parameter, to do the conversion automatically.

	To use this function, you should include the following in the main
	program	file:
		#define MAX_QUAL_NAME_LENGTH <m>
		#define FLAGS <f>
		#define PARS <p>
	-where m is the maximum character length of the qualifiers that are used;
	f is the number of different flags recognized by the program;
	p is the number of different parameters recognized by the program.

	In addition, you need to specify your flag an parameter names as
	enumerated types in the main program file, like this:
		enum flags {flag_e,flag_v};
		enum pars {par_f,par_l,par_p,par_s};
	- for example. This just means that 'flag_e' is equivalent to 0,
	'flag_v' to 1, 'par_f' to 0, 'par_l' to 2 etc .

		enum boolean {false,true};


*/

#include <stdlib.h>
#include "preproc.h"
#include "global.h"

int set_arguments(int argc, char *argv[], char *flagname[], char *parname[], char *implicit[],
                  char *flaglabel[], char *parlabel[], enum boolean setflag[], char *par[])
	{
	/*enum boolean {false,true};GLOBAL*/
	enum boolean found;

	char qualifier[MAX_QUAL_NAME_LENGTH];
	int i,j;
	int implicit_index[IMPLICIT_PARS];
	int implicit_args; /* the number of 'unlabelled' arguments so far encountered */
        char *unnamed_par[MAX_UNNAMED_PARS];
        int unnamed_par_count;

        /* before any of the command-line is checked, the consistency of the *parname[]
	   and *implicit[] arrays is confirmed, rather than on-the-fly when any leftover
	   parameters are processed later on; this is so that this sanity check isn't
	   conditional upon any leftover arguments being specified; if the check fails,
	   then the source code is wrong, irrespective of any command-line arguments */
	for (i = 0; i < IMPLICIT_PARS; i++)
	    {
	    j = 0;
	    while (strcmp(parname[j],implicit[i]) && ( j < PARS ))
	        {
	        j++;
                }

	    if (j < PARS)
	        {
		implicit_index[i] = j;
		continue;
		}
	    else
	        {
		printf("ERROR: array '*implicit[]' is incompatible with array '*parname[]'\n");
		printf("       - value of implicit[%d] ('%s') is absent from *parname[]\n",
		       i,implicit[i]);
		exit(1);
		}
	    }


        unnamed_par_count = 0;
        implicit_args = 0;

	i = 1;

	for (j = 0; j < FLAGS; j++) {

            setflag[j] = false;

	}

	for (j = 0; j < PARS; j++) {

            par[j] = "";

	}

	while (i < argc)
		{
		found = false;
		for (j = 0; j < FLAGS; j++)
			{
			if (found) continue;
			*(qualifier) = '-';
			*(qualifier+1) = '\0';
			strcat(qualifier,flagname[j]);
			if (strcmp(qualifier,argv[i]) == 0)
				{
				setflag[j] = true; 
				found = true;
				}
			}
		for (j = 0; j < PARS; j++)
			{
			if (found) continue;
			*(qualifier) = '-';
			*(qualifier+1) = '\0';
			strcat(qualifier,parname[j]);
			if ((strcmp(qualifier,argv[i]) == 0) && (i < argc - 1))
				{
				/* note, this is NOT a strcpy!
				par[j] now just points to the same place as argv[i+1] */

				par[j] = argv[++i]; 

				found = true;
				}				
			}

                /* any leftover arguments are taken to be 'parameters' whose switches
		   were omitted; these are saved here, so that once it is known which
		   parameters have been explicitly stated (which could be AFTER this
		   current 'implicit' parameter), their order will be used to determine
		   their meaning */
		if (found == false)
		    {
		    unnamed_par[unnamed_par_count++] = argv[i];
		    }

		i++;
		}

		if (setflag[flag_debug])
		    {
		    printf("%d argument(s) read; %d of these are implicit \n",i,unnamed_par_count);
		    /* 'debugging' output always includes 'verbose' output (and more) */
		    setflag[flag_v] = true;
		    }

                /* any leftover arguments are taken to be 'parameters' whose switches
		   were omitted; so, their meaning is determined by their order */

                
                i = 0; /* indexes the list of pars allowed to be specified implicitly;
		          i.e. indexes implicit_index[] */
	        implicit_args = 0;
                while (implicit_args < unnamed_par_count)
		    {
		    j = implicit_index[i];
    		    while (strcmp(par[j],"") && (i < (IMPLICIT_PARS - 1)) ) /* e.g. the first implicit arg is to be treated
		                                 as what's defined by '-i'; but '-i' has already
						 been defined, so treat it as the SECOND implicit
						 arg, and so on */
		        {
			j = implicit_index[++i];
			}
		    if (strcmp(par[j],""))
		        {
			printf("ERROR on command line: the following are surplus arguments :\n\n");
			while (implicit_args < unnamed_par_count)
			    {
			    printf("\t%s\n",unnamed_par[implicit_args++]);
			    }
			exit(1);
			}
		    else
		        {
			par[j] = unnamed_par[implicit_args++];
		        }
                    }

		if (setflag[flag_v])
		    {
		    for (i = 0; i < FLAGS; i++)
		        {
	                if (setflag[flag_debug]) printf("setflag[%2d] ",i);
			printf("(-%s)\t%20s is %s\n",flagname[i],flaglabel[i],boolean_name[setflag[i]]);
                        }

                    for (i = 0; i < PARS; i++)
		        {
          	        if (setflag[flag_debug]) printf("par[%2d]     ",i);
			printf("(-%s)\t%20s is '%s'\n",parname[i],parlabel[i], par[i]);
                        }

		    }

        if (setflag[flag_debug]) printf("set_arguments() end\n");

		return 0;
        }

int check_arguments(enum boolean setflag[], char *parname[], char *parlabel[], char *par[], char *partype[])
	{
	/* note that par is an array of pointers to STRINGS */
	int i,j,c,l;
        int problem_par_count;
        enum boolean legitimate, result;
        char ch;

        if (setflag[flag_debug]) printf("check_arguments() begin\n");

        problem_par_count = 0;

	for (i = 0; i < PARS; i++)
	    {
            legitimate = true;
            l = strlen(par[i]);
	    j = 0;
	    while (j < l)
	        {
		ch = par[i][j];
		if (setflag[flag_debug]) printf("par[%d] char '%c'\n",i,ch);
		if (strcmp(partype[i],"int") == 0)
		    {
		    if ( ( (ch < '0') || (ch > '9') )
		         && (ch != '+') && (ch != '-')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"posint") == 0)
		    {
		    if ( ( (ch < '0') || (ch > '9') )
		         && (ch != '+')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"negint") == 0)
		    {
		    if ( ( (ch < '0') || (ch > '9') )
		         && (ch != '-')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"nonzeroint") == 0)
		    {
		    if ( ( (ch < '1') || (ch > '9') )
		         && (ch != '+')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"decimal") == 0)
		    {
		    if ( ( (ch < '0') || (ch > '9') )
		         && (ch != '+') && (ch != '-') && (ch != '.')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"float") == 0)
		    {
		    if ( ( (ch < '0') || (ch > '9') )
		         && (ch != '+') && (ch != '-')
			 && (ch != '.') && (ch != 'E')
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"alphabetic") == 0)
		    {
		    if ( !( (ch >= 'a') && (ch <= 'z') )
		         &&  !( (ch >= 'A') && (ch <= 'Z') )
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"alphanumeric") == 0)
		    {
		    if ( !( (ch >= 'a') && (ch <= 'z') )
		         &&  !( (ch >= 'A') && (ch <= 'Z') )
		         &&  !( (ch >= '0') && (ch <= '9') )
		       )
		       {
		       legitimate = false;
		       }
		    }
		else if (strcmp(partype[i],"string") == 0)
		    {
		    /* anything goes...*/
		    }
                j++;
		}

            if ((strcmp(partype[i],"negint") == 0) && (par[i][0] != '-'))
                {
                legitimate = false;
		}

	    if (!legitimate)
	        {
		printf("ERROR in command line: ");
       	        if (setflag[flag_debug]) printf("par[%2d]     ",i);
		printf("(-%s)\t%20s is '%s' but must be of type %s\n",parname[i],parlabel[i], par[i], partype[i]);
		problem_par_count++;
		}
	    }
	if (problem_par_count)
	    {
	    result = false;
	    }
	else {
	    result = true;
	    }
        if (setflag[flag_debug]) printf("check_arguments() end\n");
        return result;
        }


void default_arguments(enum boolean setflag[], char *parname[], char *parlabel[], char *par[],
                    char *pardefault[])
	{
	int i,j;
	if (setflag[flag_debug]) printf("default_arguments() begin\n");

	for (i = 0; i < PARS; i++)
	    {

	    if (!strlen(pardefault[i]))
	        {
		continue;
		}

            if (setflag[flag_debug]) printf("    par[%d]=\"%s\"\n",i,par[i]);

	    if (!strlen(par[i]))
	        {
		if (setflag[flag_debug])
		    {
		    printf("par[%2d] %s (-%s) is unspecified\t",i, parlabel[i], parname[i]);
		    printf("setting par[%d] (%s, -%s) to %s\n",
		       i, parlabel[i], parname[i], pardefault[i]);
                    printf("par[%d] is %s; par[%d] is %s; length of pardefault[%d] is %ld\n",
		        i, par[i], i+1, par[i+1], i, strlen(pardefault[i]));
                    }

		else {
		    if (setflag[flag_v]) printf("setting %s to default (%s)\n", parlabel[i], pardefault[i]);
		    }

		/* note, this is NOT a strcpy!
		par[i] now just points to the same place as pardefault[i] */

		par[i]=pardefault[i];
	            
                if (setflag[flag_debug]) printf("    par[%d] is now \"%s\" (addr %s)\n",i,par[i],par[i]);
		}
	    }
	if (setflag[flag_debug]) printf("default_arguments() end\n");
        }


int limit_arguments(enum boolean setflag[], char *parname[], char *parlabel[], char *par[],
                    char *partype[], char *parmin[], char *parmax[], char *pardefault[])
	{
	enum boolean legitimate,result;
	int min_int, max_int, par_int, adjusted_par_int, i;
	float min_float, max_float, par_float, adjusted_par_float;

	if (setflag[flag_debug]) printf("limit_arguments() begin\n");

        result = true;

        /* this might instead be called directly by the client code (assuming that
	   limit_arguments() isn't called at all */

	default_arguments(setflag, parname, parlabel, par, pardefault);

        /* note that the client code can just call check_arguments() directly, instead
	   of limit_arguments, if it is preferred that no limits (in parmin[] and
	   parmax[]) are defined */

	if (check_arguments(setflag, parname, parlabel, par, partype))
	    {

	    for (i = 0; i < PARS; i++)
	        {
                legitimate = true;

		if (    (strcmp(partype[i],"int")        == 0)
                     || (strcmp(partype[i],"posint")     == 0)
                     || (strcmp(partype[i],"negint")     == 0)
                     || (strcmp(partype[i],"nonzeroint") == 0)
		   )
		    {

		    sscanf(par[i],"%d",&par_int);
		    sscanf(parmin[i],"%d",&min_int);
		    sscanf(parmax[i],"%d",&max_int);

		    if (par_int > max_int)
		        {
			legitimate = false;
			adjusted_par_int = max_int;

		        /* note, this is NOT a strcpy!
		        par[i] now just points to the same place as parmax[i] */

			par[i] = parmax[i];
			}
                    else if (par_int < min_int)
		        {
			legitimate = false;
			adjusted_par_int = min_int;

		        /* note, this is NOT a strcpy!
		        par[i] now just points to the same place as parmin[i] */

			par[i] = parmin[i];
			}

                    /* flag 'v' is always set if flag 'debug' is set */
		    if (setflag[flag_v] || !legitimate)
		        {
			printf("%s (-%s)\t%d\t",parlabel[i], parname[i], par_int);
			}

		    if ((!legitimate) || (setflag[flag_debug]))
		        {
			printf("must lie between %d and %d inclusive\t",
			        min_int, max_int);
			}

                    if (legitimate)
		        {
			if (setflag[flag_v])
			    {
			    printf("in range");
			    }
			}
		    else
		        {
			printf("adjusting value to %d\t",adjusted_par_int);

			/* N.B. the next line does NOT produce a runtime
			error; but doing the same thing but sprintf-ing
			a float, WOULD (see notes below); in case this lack of
			an error with an integer is just luck, for consistency
			the "alteration" of the string is just done with
			pointers in the same way as for floats (i.e. there
			is no alteration of strings at all, just a change of
			the place to which the pointer points) */

			/* sprintf(par[i],"%d",adjusted_par_int); */
			}

                    if (setflag[flag_v] || !legitimate)
		        {
			printf("\n");
			}

		    }
		/* "decimal" means a (positive or negative) number with a decimal point;
		   "float" is supposed to mean, "decimal" OR standard form;
		   these may need different treatment */
		else if (    (strcmp(partype[i],"decimal") == 0)
		          || (strcmp(partype[i],"float")   == 0)
			)
		    {
		    sscanf(par[i],"%f",&par_float);
		    sscanf(parmin[i],"%f",&min_float);
		    sscanf(parmax[i],"%f",&max_float);

		    if /* (strlen(parmax[i]) && */ (par_float > max_float) /*)*/
		        {
			legitimate = false;
			adjusted_par_float = max_float;

		        /* note, this is NOT a strcpy!
		        par[i] now just points to the same place as parmax[i] */

			par[i] = parmax[i];
			}
                    else if /* (strlen(parmin[i]) && */ (par_float < min_float) /*)*/
		        {
			legitimate = false;
			adjusted_par_float = min_float;

		        /* note, this is NOT a strcpy!
		        par[i] now just points to the same place as parmin[i] */

			par[i] = parmin[i];
			}

                    /* flag 'v' is always set if flag 'debug' is set */
		    if (setflag[flag_v] || !legitimate)
		        {
			printf("%s (-%s)\t%f\t",parlabel[i], parname[i], par_float);
			}

		    if ((!legitimate) || (setflag[flag_debug]))
		        {
			printf("must lie between %f and %f inclusive\t",
			        min_float, max_float);
			}

                    if (legitimate)
		        {
			if (setflag[flag_v])
			    {
			    printf("in range");
			    }
			}
		    else
		        {
			printf("adjusting value to %f\t",adjusted_par_float);

			/* !! The tampering with the string below results in
			a segmentation fault; curiously, that ISN'T the case
			if you try to sprintf an integer (note that sprintfing
			%3.1f, or any %x.yf fails just like %f); therefore,
			the "changing" of the string is done by changing the
			pointer instead (see above) - it just points to the
			string held in parmin[i] or parmax[i] as the case
			may be; note that the sprintf below would be trying
			to alter the memory pointed to by parmin[i] and
			parmax[i]; they are global static variables, but if
			that is the problem, then why doesn't it cause the
			same problem if trying to sprintf and integer to
			exactly the same piece of memory? */
			
			/* sprintf(par[i],"%f",adjusted_par_float); */
			}

                    if (setflag[flag_v] || !legitimate)
		        {
			printf("\n");
			}
		    }
		else if (strcmp(partype[i],"alphabetic") == 0)
		    {
		    /* no limits */
		    }
		else if (strcmp(partype[i],"alphanumeric") == 0)
		    {
		    /* no limits */
		    }
		else if (strcmp(partype[i],"string") == 0)
		    {
		    /* no limits */
		    }
                if (setflag[flag_debug]) printf("par[%d] is now \"%s\"\n",i,par[i]);
		}
	    }
	else {
	    result = false; /* this is if check_arguments() fails */
	    }
	if (setflag[flag_debug]) printf("limit_arguments() end\n");
        return result;
	}
