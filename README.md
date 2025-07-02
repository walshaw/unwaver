# unwaver
Finds targeted repeat periodicities in protein or DNA sequences.

## Introduction

UNWAVER calculates periodicities of biological sequence repeats, and reports
instances of periodicities within a specified target range. The periodicity
arises from some property of each letter in the repeat. UNWAVER is principally
meant for protein sequences, and the periodicity might be the hydrophobicity
of the amino acids, but it can be based on anything. The periodicity is
calculated by a Fourier transform (FT), in a sliding window.

The property can be binary, in which case, different amino acids (or bases) are
'on' (1) or 'off' (0); this is user specified. Alternatively, different values
can be used, i.e. each amino acid (or base) has a value on a user-defined
'scale'.

The default settings are geared towards finding the kinds of repeats
(especially hydrophobic amino acids) which occur in alpha-helices of proteins,
particularly in coiled-coil proteins.
By default, FTs are calculated for periodicities in the range 3.0 to 4.0 and
the results then filtered to find any instances of the specified period within a particular range.

## Motivation

The software was written in 2007 - 2008 to search protein sequence databases
(principally UniProtKB) for sequences with unusual hydrophobic repeat
periodicities - one in particular; see below. For a collaboration I was
involved in, I had been using scripts to calculate FTs of protein sequences
to determine whether a coiled-coil repeat nature occurred, and any
peculiarities in the repeats. However, these scripts were too slow to analyse a
primary protein sequence database such as UniProt, and also would have
involved a significant post-processing stage to filter for rpeats within the
particular periodicity range of interest.

The REPPER software had been available for a couple of years (Gruber *et al.*,
2005), and was very useful. However, it was a web-based platform requiring sequences of interest to
be uploaded for analysis, or nominated sequences from UniProt (IIRC), but in
any case not geared towards a very high volume of query protein sequences, and
involved multiple software components. Even obtaining and installing them
locally would have required some significant post-processing of results to
filter for proteins of interest.

Therefore, UNWAVER was written as a relatively efficient steamroller to seek
hyrophobic (or any other property) sequence repeats in proteins, within a
user-defined range of periodicity. The name is a reference to FTs representing
the deconvoluting of a signal into a sum of 'waves' representing different
repeat periods of different amplitudes.

### Project

The specific scientific motivation was a collaboration with
Gabriella Kelemen's group at the University of East Anglia, who were
investigating a particular protein of *Streptomyces coelicolor*, named Scy.
This is a long protein ( > 1,300 amino acids), and the sequence was mostly
predicted to be an alpha-helical coiled coil, indicating a possible
structural role. Gabriella and her team had found that deletion of the *scy*
gene resulted in quite dramatic morphological phenotypes (later published
in Holmes *et al.*, 2013).

I analysed the Scy amino acid sequence, and it appeared to exhibit an unusual
period of hydrophobic repeat. This period is consistent with a right-handed coiled coil, but
distinctly different to a 'normal' right-handed coiled-coil repeat and with
core-position amino acids of a particular character. The especially notable
property of the repeat is that it is relatively invariant over a very long
domain of the protein, over 1,000 amino acids, indicating a genuine
structural and presumably functional uniqueness compared to 'normal'
right-handed and other coiled coils. The mean hydophobic period indicated 14
turns over a repeat of 51 amino acid residues of alpha-helices in a
right-handed coiled-coil conformation.

It was therefore natural to suppose that this repeat sequence/structure may
be key to its function in *Streptomyces* and to ask if other proteins
existed with this rather precise, sustained repeat existed in other organisms,
and with what potential functions. It transpired that the protein (FilP) encoded by
the neighbouring gene of *Streptomyces* was a very similar, but much shorter,
kind of protein as Scy. I also found that a previously-characterised *S.
coelicolor* coiled-coil protein involved in polar growth and morphogenesis,
DivIVA, was of a somewhat similar nature (Walshaw *et al.*, 2010), although less similar than FilP. It
was then a case of analysing various other coiled-coil or probable coiled-coil
proteins implicated in the literature for various reasons. However, this
approach was limited and it seemed
useful to be able to perform a systematic, exhaustive search of all protein
sequences in the UniProtKB database. The results of the search for the
'Scy-like repeat' are in Walshaw *et al.* (2010), which is also the
reference to cite for UNWAVER. 

## References
* Cornette J.L., Cease K.B., Margalit H., Spouge J.L., Berzofsky J.A., DeLisi C. (1987) Hydrophobicity scales and computational techniques for detecting amphipathic structures in proteins. *J. Mol. Biol.* **195**(3):659-685.
* Cowan R., Whittaker R.G. (1990) *Peptide Research* **3**:75-80. 
* Eisenberg D., Schwarz E. Komaromy M. Wall R. (1984) Analysis of membrane and surface protein sequences with the hydrophobic moment plot. *J. Mol. Biol.* **179**(1):125-142.
* Engelman D.M., Steitz T.A., Goldman A. (1986). Identifying nonpolar transbilayer helices in amino acid sequences of membrane proteins.
*Annu. Rev. Biophys. Biophys. Chem.* **15**:321-353.
* Fauchere J.-L., Pliska V.E. (1983) *Eur. J. Med. Chem.* **18**:369-375. 
* Gruber M., Söding J., Lupas A.N. (2005) *Nucleic Acids Res.* **33**(suppl_2):W239-W243. doi:10.1093/nar/gki405
* Holmes N.A., Walshaw J., Leggett R.M., Thibessard A., Dalton K.A., Gillespie M.D., Hemmings A.M., Gust B., Kelemen G.H. (2013) Coiled-coil protein Scy is a key component of a multiprotein assembly controlling polarized growth in *Streptomyces*. *Proc. Natl. Acad. Sci. USA* **110**(5):E397-E40. doi:10.1073/pnas.1210657110* Hopp T.P., Woods K.R. (1983) A computer program for predicting protein antigenic determinants. *Mol. Immunol.* **20**(4):483-489. 
* Janin J. (1979) Surface and inside volumes in globular proteins. *Nature* **277**(5696):491-492. 
* Kyte J., Doolittle R. F. (1982) A simple method for displaying the hydropathic character of a protein. *J. Mol. Biol.* **157**(1):105-132.
* Qiagen Bioinformatics. Hydrophobicity scales. https://resources.qiagenbioinformatics.com/manuals/clcgenomicsworkbench/650/Hydrophobicity_scales.html
* Rose G.D., Geselowitz A.R., Lesser G.J., Lee R.H., Zehfus M.H. (1985) Hydrophobicity of amino acid residues in globular proteins. *Science* **229**(4716):834-838. 
* Walshaw J., Gillespie M.D., Kelemen G.H. (2010) A novel coiled-coil repeat variant in a class of bacterial cytoskeletal proteins. *J. Struct. Biol.* **170**(2):202–215. doi:10.1016/j.jsb.2010.02.008

* UniProtKB record for Scy (accession Q9L2C3). https://www.uniprot.org/uniprotkb/Q9L2C3/

## Manual

    unwaver -i <FASTA_FILE> -b <CHARACTERS>
    unwaver -i <FASTA_FILE> -s <SCALE_FILE> [ -l <COLUMN_NO.> -c <COLUMN_NO.> ]

There are two modes, **binary** and **scale**. For binary, all characters
(one-letter codes for amino acids, or nucleotides) have either the value 1
('on') or 0 ('off'), by default (the on and off values can be changed).
To enable a more detailed specification of values, use scale mode, which
requires a file specifying the score for each character (e.g. on a
hydrophobicity scale for amino acids).

### Example usage

    unwaver -i uniprot.fasta -b AVILMFYW > uniprot_unwaver.txt
    unwaver -i uniprot.fasta -s aa\_hydrophobicity -l 1 -c 4  > uniprot\_unwaver.txt
    unwaver -i uniprot.fasta -s aa\_hydrophobicity -l 1 -c 4,7  > uniprot\_unwaver.txt

### Format of scale file

The scale file is a table, where each row corresponds to a letter (amino acid
code or nucleotide code). There must be a minimum of two columns. One column
must contain the letters, and at least one column must contain scores for all
letters. **It is permissible to use more than one set of scores (e.g. two
different amino-acid hydrophobicity scales) simultaneously;** in this case,
the calculations will be done independently for each.

By default, if -s is used (to specify a scale file), then one scale will be
used, and by default that will be read from the second column, with the first
column specifying the letters. Lines beginning with a comment character will
be ignored. The default comment character is '#' but this can be changed with
-scomment.

### Calculation of periods: FT windows

Calculation of the periodicities is performed by Fourier Transforms (FTs). The
rest of this section assumes that the input sequences are proteins, but the
principles generally apply to nucleic acid sequences. It also assumes that the
property being analysed is hydrophobicity.

The general question is, what is the periodicity of hydrophobic amino acids in
each input sequence? To be more accurate, the question is, what is the
*strongest* periodicity? The FT calculates the magnitude of each value of
periodicity, over a given range of periods (e.g. 3.00 to 4.00, in a hundred
steps). The output of the FT is the magnitude of each period. The periodicity
with the greatest magnitude is then what is recorded.

For a single protein, this periodicity can be calculated for the full length of
the polypeptide sequence, giving a single value of strongest periodicity. This
is often a useful thing to do. However, given the multidomain nature of proteins
generally, it is often preferable to calculate the (strongest) periods for each
part of the protein. Most protein sequences do not consist entirely of repeats,
and still fewer consist entirely of a single repeat periodicity. Therefore one
particular domain of a protein might contain a strong repeat signal, but this
may be diluted or smeared when calculating the periodicities of the entire
sequence.

Therefore, UNWAVER takes a sliding-window approach. E.g. if the window is of
size 100 amino acids (or nucleotides) then the FT is performed on the first
100 amino acids, then on the segment consisting of amino acids 2-101, then on
3-102, etc. An odd-numbered size of window enables slightly easier handling of
the output; each window can be represented by the central amino acid (or
nucleotide), and the strongest periodicity value for the window then assigned
to that position. E.g. for a window of size 101, the centre of the first
window is amino acid 51.

The default window size is **129**, meaning the strongest-period value is
assigned to the 65th amino acid (or nucleotide) of each window.

For example, using the default window size of 129, a protein sequence of
length 540 has 475 different windows: the first is 1 - 129, and the last is
412 - 540. The centre of the first is amino acid 65, and the centre of the
last is 476, so a periodicity value (i.e. strongest in each window) is
assigned for each of positions 65 - 476, thus 412 in all. No value is defined
for amino acids 1 - 64 or 477 - 540.

The result, for a single sequence, is therefore a readout of the value of the
strongest periodicity versus each position in the input sequence (omitting the
positions at the end, i.e. those which are not the centres of any window).
This might be represented on a plot where the x-axis is amino acid position
(starting at 33 and ending at 508 in this example) and the y-axis is the
strongest periodicity.

### Averaging periods: statistics windows

The calculation of the strongest period for each amino acid position
(representing a 'window' of amino acids) is not fully sufficient for the aim
of UNWAVER, i.e. to find repeats of a particular periodicity in a segment or
segments of many protein sequences. Within a domain containing a hydrophobic
repeat (for example), the value of the strongest period will very rarely be
constant from one window to the next. Even hypothetical, perfect repeat
sequences will generally not have a completely 'flat' period, even with
some quite large window sizes.

Given that the aim is to find periodicities close to a defined target value,
one approach would be to report all segments consisting of
consecutive windows whose strongest periodicity is equal to a specified
target, +/- some range. However, given the non-flat, 'wavering' nature of
the periods, this would usually lead to quite fragmented segments,
separated by other segments whose strongest period lies outside the range.
This could be mitigated by using a relatively large range, but a consequence
would be lots of additional short segments, likely not of interest, whose
strongest periodicities briefly entered that broad range.

Therefore, the approach taken with UNWAVER is to simply average the values
of the strongest periodicities, over another sliding window. These are named
'**stats windows**', and the value of the central position of each is the
mean of *n* consecutive **FT windows**' central value (strongest period in
FT window), where *n* is the width of the stats window. The default width
of the stats window is **65**.

The same principle of windows being represented by their central values
applies. Therefore by default, the first position with an FT period value is
65 (the centre of the 129-residue FT window), and so the first stats window
by default consists of positions 65 - 129 (length 65), the centre of which is
amino acid 97. So with default settings, the stats window output begins at
position 97.

The reporting of segments of interest is thus specified by the average
(**mean**) value of the strongest periodicity lying with a +/- range of the
target periodicity over one or more consecutive stats windows. This permits
an additional constraint, i.e. how much deviation there is from the mean
within the stats window, whatever that mean is. The standard deviation is
the constraint used.

The default **target** periodicity value is effectively meaningless, i.e.
0.000; therefore -target should always be specified. The default 
periodicity **range** (-range) is 0.1 (which is quite liberal). The default
maximum permissible **standard deviation** is 1.0, which is very liberal.

Files containing the FT window values and stats window values as tables
(one row per amino acid position) may be output by using -peaks and -stats
respectively.

### Command-line options

#### Commentary options

    -q	          quiet output mode (off by default)
    -v	          verbose mode (off by default)
    -debug        debug mode (off by default)
    -r INTEGER    frequency  (number of sequences processed so far) of progress report

#### Input sequence record options
    -i FILENAME      input sequence (FASTA) file name
    -valid STRING    string containing all valid single-letter codes
    -ac INTEGER      number of field to use as AC (sequence accession) (default is 2)
    -id INTEGER      number of field to use as ID (sequence identifier) (default is 1)
    -n               assume NCBI FASTA header format (off by default)
    -ignore STRING   string containing all characters to ignore in sequences
    -firstseq INTEGER    first sequence to process from input file; numbering starts at 1

#### Output options
    -peaks    name of output file of frequencies (and periods) with highest magnitude
    -stats    name of output file of statistics of frequencies with highest magnitude

#### Amino acid (or base) property options
    -b STRING      single-letter codes assigned the 'on' value; all others are 'off' (no default)
    -on INTEGER    'on' value for binary treatment of bases/residues (default is 1)
    -off INTEGER   'off' value for binary treatment of bases/residues (default is 0)
    -s STRING      name of scale file (no default)
    -x	FLOAT      value to assign to unrecognized single-letter codes, if using scoring scale (default is -100.0)
    -sskip         skip blank lines in scale file (off by default)
    -h	           help mode (off by default); **NB** this is not yet implemented.
    -scomment      comment symbol in scale file (default is '#')
    -c	INTEGER[,INTEGER...[,INTEGER ...]]data column number(s) in scale file (separate with commas if more than one value) (default is 2)
    -l INTEGER     label (single-letter code) column number in scale file (default is 1)

#### Periodicity-calculation options
    -from FLOAT    lowest period for which to calculate FT (default is 3.000)
    -to   FLOAT    highest period for which to calculate FT (default is 4.000)
    -f    INTEGER  number of frequencies (or periods) to calculate (default is sequence length)
    -target FLOAT  only report segments within range of this target PERIOD (and with sigma <= maxsigma) (default is 0.0)
    -range  FLOAT  see target (default is 0.100)
    -w    INTEGER  window size (residues) for calculating FT (default is 129)

#### Statistics options
    -wstats INTEGER	size of window (in residues) for calculating stats (default is 65)
    -maxsigma FLOAT	only segments with sigma <= this value will be reported (default is 1.000)



