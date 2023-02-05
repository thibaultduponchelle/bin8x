# 31-03-2011 - Version 2.0
- Command line a la bin2var

- Getopt for command line parsing (-o -i, no order, defaults to 83p
There's a lot of new ways to use `bin8x` and lesser possibility to crash it.
You can use `-o` alone, `-i` alone, or do not use options.
You can give arguments in the order you want, `bin8x` detect wich one is output/input.
If no extension given and no (-2 -3 -p), it uses 83p by default.

- Unsquisher for ti83 noshell
This is really useful for *TI83 regular* designed to be used without shell (with `Send(9pgrmNAME` )
But the program will be really *bigger* (2 times bigger) and *slower*.

- By default, convert to uppercase the calc name
There's no reason to keep lowercase because the program will not launch on calc but you can specify to keep lowercase (`-l` or `-lowercase`)

- Tests 
I've added some script to test the tool but you don't need it. Just ignore them :)

- Bugfix
I've found and fixed a little bug (8xp recognition was failing sometimes). 
Sorry for this issue, I will continue to fix bugs if possible in the futur.

# 22-12-2022 - Version 2.1 
- Fix name padding issue reported by **Stefan Lennartsson** with null padding and `-n` option
- More tests
- Fix -i when -o command line parsing issue

# 05-02-2023 - Version 2.2
- Unsquish without temp file
- Fix fgetc approach (adding extra byte in progamData not used later)
- Do not add ti83 footer when unsquishing if the target model is not ti83
