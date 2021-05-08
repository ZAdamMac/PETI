# Understanding PETI Fonts
PETI, by design, is using a character-based graphics system to drive its display with the minimum amount of memory dedicated to recalling the display's state. As a result of this, the definition of fonts for PETI is extremely important. For convenience, a [modified version of fontedit](https://github.com/zadammac/fontedit) is included in the PETI Helpers repo. This version of fontedit will be maintained alongside the original but includes special exporters designed specifically for exporting each PETI font.

At time of writing, PETI has three fonts: 8x8, 8x12, and 16x16. In addition to the exported versions of the fonts being available in `font.h`, the FontEdit source files are included in `lib/display/fontedit source files` for convenience.

## Understanding the 8-pixel-wide fonts
Both the 8x8 and 8x12 fonts are structurally similar and are provided as seperate exporters for convenience only. Each of these fonts is an array of strings. The index of a particular string in the array corresponds to the UTF8-encoded byte value of the character in question (for convenience). Using the exporter, a comment is included in each line indicating which string is which value. This allows for the full range of ASCII-printable characters to be implemented as their "usual" values while giving over the remaining space normally used for non-printable characters for other purposes. In this implementation, each byte of each string represents one row of pixel state for that character, starting from the top to the bottom, from *right to left*. High bits (1) represent an unfilled cell on the display.

*A note on intention*: the 8x8 font will not be widely used within PETI, mostly for debug information. It's not necessary to implement more characters than basic alphanumerics at this time. 8x12 however is the primary "menu" font and must include both alphanumerics, punctuation, and a number of icons.

## Understanding the 16-Pixel wide "Large" Font.
The 16x16 font is *structurally similar* to the 8x8 fonts, with one major exception: instead of each row of the character being represented by one byte, it is instead represented by two. Addressing this correctly is handled in the code for `printTextLarge()` of `display.c` if you want a visualization.
 
