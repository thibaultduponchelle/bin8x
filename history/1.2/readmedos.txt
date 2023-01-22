Bin8x v1.2 for Dos
Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

Thanks to: Solignac Julien for optimizing the code and making it look more 
like devpac, and for the autodetection of filetypes.  


Here it is, the new bin8x squisher for dos, I've been busy and made a few updates:

- devpac compatibility (bin8x binfile-with-no-extension
- autodetection of filetype (83/83+) by checking if you inserted 
	".org progstart-2"
	".db $BB,$6D" (the TI-83-Plus-OS-executable bytes)
-quite a few options to ensure usefulness:

-o   or --output      : specific outputfile ([binfile].8?p if none given.
-n   or --calcname    : specific filename on the calculator ([binfile] if
                        none given.
-i   or --input       : specific inputfile, you must use -o and -n if you
                        use it. Also give the complete filename with extension.
-83  or --ti83        : force ti-83 file (normally bin8x will autodetect).
-83p or --ti83plus    : force ti-83 plus file (normally bin8x will autodetect).
-u   or --uppercase   : convert calculator file name to uppercase.
-x   or --executable  : force  "TIOS-executable" bytes to the TI-83 Plus 
                        program.
-e   or --unprotected : generate unprotected files (I don't see any use for
                        that, but you might...).
-?   or --help        : display help screen.

syntax:

bin8x binfile [-options]

 or

bin8x -i infile -o outfile -n name [options]

binfile/infile	: the binary output from your assembler, binfile is in 8.3 dos
		  format, infile can be anything below 255 bytes


I hope all of you have use for this program.

BTW. It's open source, so don't hesitate to send me updates or bug fixes.


Compiling the source:

with bcc use: bcc bin8x.c -o bin8x
