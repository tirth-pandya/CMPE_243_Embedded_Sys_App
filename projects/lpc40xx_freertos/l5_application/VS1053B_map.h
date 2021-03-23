#pragma once

// VS105B SCI Registers
// mode control
#define SCI__MODE 0x0

// Status of VS1053b
#define SCI__STATUS 0x1

// Built-in bass/treble control
#define SCI__BASS 0x2

// Clock freq + multiplier
#define SCI__CLOCKF 0x3

// Decode time in seconds
#define SCI__DECODE_TIME 0x4

// Misc. audio data
#define SCI__AUDATA 0x5

// RAM write/read
#define SCI__WRAM 0x6

// Base address for RAM write/read
#define SCI__WRAMADDR 0x7

// Stream header data 0
#define SCI__HDAT0 0x8

// Stream header data 1
#define SCI__HDAT1 0x9

// Start address of application
#define SCI__AIADDR 0xA

// Volume control
#define SCI__VOL 0xB

// Application control register 0 to 4
#define SCI__AICTRL0 0XC
#define SCI__AICTRL1 0XD
#define SCI__AICTRL2 0XE
#define SCI__AICTRL3 0XF