# Concordance Lookup

This program builds a concordance database over the given input text file and enables fast word lookup via binary search.
The database points to byte positions in the specified text file _on disk_ and binary search is performed over these positions rather than in the file directly, resulting in fast word lookup without having to load the (potentially large) file into memory once the initial single-pass preprocessing of the text file has been performed. The concordance database is tightly packed and read using byte offsets, meaning no space is wasted on disk when constructing the database.

## Usage

The program can be built using the included makefile:

* _make short-index_ builds a concordance database over the file _korpus-short_. In order to use another text file as lookup source, the tokenizer and constructor needs to be built separately using 'make tokenizer' and 'make constructor'. Once built, first process the text file using the tokenizer and then pass the output to the constructor. 
* _make konkordans_ builds the concordance database lookup program (currently only available in swedish). 

_./konkordans [word] [text file]_ performs binary search over _[text file]_ searching for occurrences of _[word]_ and prints out all occurrences to stdout with leading and trailing words. Note that even though _korpus-short_ is a small text file, similar performance will be had with much larger text files (> 1 gb) due to the O(log n) time complexity of binary search. 