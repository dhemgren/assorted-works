SHELL = /bin/bash

error:
	@echo "Please specify a target (long, short, tokenizer, test, etc)."

konkordans: hash.c read.c konkordans.c
	gcc -o konkordans konkordans.c hash.c read.c -std=c99

long-index: tokenizer constructor
	export LC_ALL=C && \
	./tokenizer < /info/adk16/labb1/korpus | sort > /var/tmp/ewdh-sorted.txt
	./constructor < /var/tmp/ewdh-sorted.txt

short-index: tokenizer constructor
	export LC_ALL=C && \
	./tokenizer < korpus-short | sort > /var/tmp/ewdh-sorted.txt
	./constructor < /var/tmp/ewdh-sorted.txt

tokenizer: tokenizer.c
	gcc tokenizer.c -o tokenizer -std=c99

constructor: constructor.c hash.c
	gcc constructor.c hash.c -o constructor -std=c99

# create tests executable
tests: tests.c read.c hash.c
	gcc tests.c read.c hash.c -o tests -std=c99

# run tests
test: short-index tests
	./tests

clean:
	rm -f temp.txt tokenizer constructor words.txt occurrences.bin \
		occurrence_index.bin word_index.bin tests konkordans hash.bin \
		ewdh-sorted.txt

# to not confuse make in case of files named "clean" or "test" etc
.PHONY: clean test error
