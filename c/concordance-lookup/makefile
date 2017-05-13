SHELL = /bin/bash

error:
	@echo "Ange mål (short-index, tokenizer etc)."

konkordans: hash.c read.c konkordans.c
	gcc -o konkordans konkordans.c hash.c read.c -std=c99

short-index: tokenizer constructor
	mkdir bin
	export LC_ALL=C && \
	./tokenizer < korpus-short | sort > bin/sorted.txt
	./constructor < bin/sorted.txt

tokenizer: tokenizer.c
	gcc tokenizer.c -o tokenizer -std=c99

constructor: constructor.c hash.c
	gcc constructor.c hash.c -o constructor -std=c99

clean:
	rm -f tokenizer constructor bin/words.txt bin/occurrences.bin \
		bin/occurrence_index.bin bin/word_index.bin konkordans bin/hash.bin \
		bin/sorted.txt bin/hash_end.bin bin/hash_start.bin
	rmdir bin

# to not confuse make in case of files named "clean" or "error" etc
.PHONY: clean error
