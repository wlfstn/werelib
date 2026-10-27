# ASCII85
ASCII85 represents binary data as printable text. 

It treats four bytes as one big-endian 32-bit number, converts it to five base-85 digits, and maps those digits to characters `!` through `u`. 
Four zero bytes can become `z`; 
A final group of 1-3 bytes uses 2-4 characters. Ordinary full groups expand by 25%.

For example, `Hello world` becomes `87cURD]j7BEbo7`.

## History
It grew from the Unix `btoa` utility; Adobe later adopted a variant for PostScript and PDF.

## Sources
See [Adobe's PostScript Language Reference, ASCII85Encode](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf).
See [Adobe's PDF reference, section 3.3.2](https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/pdfreference1.6.pdf).