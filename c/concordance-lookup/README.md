# ADK Lab 1

För komplett lydelse, se https://www.kth.se/social/course/DD1352/subgroup/ht-2016-463/page/labb-1-20/

## Todo

- ~~`LC_COLLATE` sätts nog aldrig i vår makefile. Sort verkar funka som den ska ändå?~~
- ~~BUG: å ä ö vid hashning -> segfault~~
- I senaste buggfixen ändrade jag hashfunktionen till bas 30, och ökade hashtabellens storlek till 28000. Det var nog onödigt.
- Se till att allt som allokeras med `malloc` också deallokeras med `free`.

Om problem uppstår:

- Vi har använt signed/unsigned lite inkonsekvent.
