# File lookup using concordance database, binary search

This program builds a concordance database over the given input text file and enables fast word lookup via binary search.
The database points to byte positions in the specified text file _on disk_ and binary search is performed over these positions rather than in the file directly, resulting in fast word lookup without having to load the (potentially large) file into memory once the initial single-pass preprocessing of the text file has been performed. 

## Usage

The program can be built using the included makefile:

* 'make short-index' builds a concordance database over the file 'korpus-short'. In order to use another text file as lookup source, the tokenizer and constructor needs to be built separately using 'make tokenizer' and 'make constructor'. Once built, first process the text file using the tokenizer and then pass the output to the constructor. 

* 'make konkordans' builds the concordance database lookup program (currently only available in swedish). 

'./konkordans [word] [textfile]' performs binary search over [textfile] searching for occurrences of [word] and prints out all occurrences to stdout with leading and trailing words. Note that even though 'korpus-short' is a small textfile, similar performance will be had with much larger textfiles (> 1 gb) due to the O(log n) time complexity of binary search.