# Teensy ILI9488 library (MIPI-DBI Type B 8-bit bus)
8 bit parallel data interface TFT Library for Teensy (tested on Teensy 4.1)

I ported from here.
https://github.com/nopnop2002/Arduino-STM32-8bitTFT
Btw, nice work nopnop for making the library so clear, porting was extremely easy.

He ported from here.   
https://github.com/prenticedavid/MCUFRIEND_kbv   

----

# Software requirement    

- Adafruit GFX Library   
https://github.com/adafruit/Adafruit-GFX-Library   

----

# Wiring for 16bit Parallel TFT   

You can use any pin.

Default:\
D0 - 41\
D1 - 40\
D2 - 39\
D3 - 38\
D4 - 37\
D5 - 36\
D6 - 35\
D7 - 34\
RD - 8\
WR - 7\
DC - 9 (a.k.a RS on some boards)\
CS - 10\
RST - 6

16 bit:\
D8 - 31\
D9 - 30\
D10 - 29\
D11 - 28\
D12 - 27\
D13 - 26\
D14 - 25\
D15 - 24\

----

# Benchmark using ILI9341(240x320)

\*Teensy 4.1 results are measured at 600MHz on a 320x480 display, which is double the other benchmark contestants.
|Benchmark|Teensy 4.1 8bit|Teensy 4.1 16bit|STM32F103|STM32F303|STM32F401|STM32F407|STM32F446|ATmega328|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|Screen fill            |107307|77617|2241101|1854703|1208997|647337 |604184 |1379560|
|Text                   |3781|4673|138955 |106085 |69792  |38303  |35748  |344024 |
|Lines                  |72065|94473|1337689|999535 |660532 |361830 |337721 |3390180|
|Horiz/Vert Lines       |8857|6491| 185661 |152992 |99924  |53548  |49978  |144664 |
|Rectangles (outline)   |5199|3874|120109 |98346  |64325  |34627  |32311  |104260 |
|Rectangles (filled)    |273222|190562| 4756300|3935313|2583183|1386162|1293716|3267476|
|Circles (filled)       |31283|29674|610890 |471781 |310337 |167938 |156716 |1211484|
|Circles (outline)      |33174|43535|588951 |439863 |290416 |159225 |148621 |1475108|
|Triangles (outline)    |13665|17578|302008 |225153 |148665 |81285  |75894  |1075596|
|Triangles (filled)     |83088|66348|1550456|1262459|821918 |440290 |410928 |1721636|
|Rounded rects (outline)|12659|14431|262224 |201583 |132647 |72231  |67431  |506428 |
|Rounded rects (filled) |273115|195157|4711541|3899510|2555561|1372596|1281003|3795228|

*if you have good enough connection, you might be able to run it at 1GHz and get a screen fill score of 46198us without artifacts (62Hz full screen updates)*