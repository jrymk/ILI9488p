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

# Wiring for 8bit Parallel TFT   

No fancy low level pin manipulation for now. You can use any pin.

Default:
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

----

# Benchmark using ILI9341(240x320)

\*Teensy 4.1 results are divided in half because it is tested on a 320x480 display (with just normal "slow" API like digitalWrite())
|Benchmark|Teensy 4.1|STM32F103|STM32F303|STM32F401|STM32F407|STM32F446|ATmega328|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|Screen fill            |464461|2241101|1854703|1208997|647337 |604184 |1379560|
|Text                   |12969|138955 |106085 |69792  |38303  |35748  |344024 |
|Lines                  |259279|1337689|999535 |660532 |361830 |337721 |3390180|
|Horiz/Vert Lines       |38418|185661 |152992 |99924  |53548  |49978  |144664 |
|Rectangles (outline)   |21069|120109 |98346  |64325  |34627  |32311  |104260 |
|Rectangles (filled)    |1126463|4756300|3935313|2583183|1386162|1293716|3267476|
|Circles (filled)       |117194|610890 |471781 |310337 |167938 |156716 |1211484|
|Circles (outline)      |111071|588951 |439863 |290416 |159225 |148621 |1475108|
|Triangles (outline)    |51507|302008 |225153 |148665 |81285  |75894  |1075596|
|Triangles (filled)     |360553|1550456|1262459|821918 |440290 |410928 |1721636|
|Rounded rects (outline)|46397|262224 |201583 |132647 |72231  |67431  |506428 |
|Rounded rects (filled) |1119823|4711541|3899510|2555561|1372596|1281003|3795228|