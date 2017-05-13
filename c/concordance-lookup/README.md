# File lookup using concordance database, binary search

This program builds a concordance database over the given input text file and enables fast word lookup via binary search.
The database points to byte positions in the specified text file _on disk_ and binary search is performed over these positions rather than in the file directly, resulting in fast word lookup without having to load the (potentially large) file into memory once the initial single-pass preprocessing of the text file has been performed. 