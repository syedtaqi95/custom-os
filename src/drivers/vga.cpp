/*
    vga.cpp - VGA class implementation to communicate with stdout

	References:
    - https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
*/

#include "drivers/vga.h"

using namespace morphios::common;
using namespace morphios::drivers;
using namespace morphios::kernel;

VGA::VGA()
  : VGA_AC_INDEX(0x3C0),
	VGA_AC_WRITE(0x3C0),
	VGA_AC_READ(0x3C1),
	VGA_MISC_READ(0x3CC),
	VGA_MISC_WRITE(0x3C2),
	VGA_SEQ_INDEX(0x3C4),
	VGA_SEQ_DATA(0x3C5),
	VGA_GC_INDEX(0x3CE),
	VGA_GC_DATA(0x3CF),
	VGA_CRTC_INDEX(0x3D4),
	VGA_CRTC_DATA(0x3D5),
	VGA_INSTAT_READ(0x3DA)
{}

VGA::~VGA() {}

// Text mode methods

size_t VGA::terminal_row = 0;
size_t VGA::terminal_column = 0;
uint8_t VGA::terminal_color = 0;
uint16_t* VGA::terminal_buffer = 0;
bool VGA::isWelcome = true;

void VGA::terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void VGA::terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void VGA::terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void VGA::terminal_putchar(char c) {
	if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
		if (terminal_row == VGA_HEIGHT)
			terminal_initialize();
		if(!isWelcome) {
			terminal_write("$ ", strlen("$ "));
			terminal_column = 2;
		}
	}
	else if (c == '\0') {}
	else if (c == '\b') {
		if (terminal_column > 2) {
			terminal_column--;
			terminal_putentryat('\0', terminal_color, terminal_column, terminal_row);
		}
		else {
			terminal_column = 2;
		}
	}
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_initialize();
		}
	}	
}
 
void VGA::terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void VGA::print_welcome_msg() {
	kprintf("                               __    _ ____  _____\n");
	kprintf("   ____ ___  ____  _________  / /_  (_) __ \\/ ___/\n");
	kprintf("  / __ `__ \\/ __ \\/ ___/ __ \\/ __ \\/ / / / /\\__ \\ \n");
	kprintf(" / / / / / / /_/ / /  / /_/ / / / / / /_/ /___/ / \n");
	kprintf("/_/ /_/ /_/\\____/_/  / .___/_/ /_/_/\\____//____/  \n");
	kprintf("                    /_/                           \n");
	kprintf("\nWake up, Neo\n");
	kprintf("The Matrix has you\nFollow the white rabbit\n...\nKnock, Knock, Neo.\n");	
}

// Graphics mode methods

void VGA::writeRegisters(common::uint8_t* regs) {
	uint8_t i = 0;
	// Write Misc registers
	VGA_MISC_WRITE.write(*regs);
	regs++;

	// Write Sequencer registers
	for (i = 0; i < VGA_NUM_SEQ_REGS; i++, regs++) {
		VGA_SEQ_INDEX.write(i);
		VGA_SEQ_DATA.write(*regs);
	}

	// Unlock CRTC registers
	VGA_CRTC_INDEX.write(0x03);
	VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() | 0x80);
	VGA_CRTC_INDEX.write(0x11);
	VGA_CRTC_DATA.write(VGA_CRTC_DATA.read() | 0x80);

	// Make sure they remain unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	// Write CRTC registers
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++, regs++) {
		VGA_CRTC_INDEX.write(i);
		VGA_CRTC_DATA.write(*regs);
	}

	// Write Graphics Controller registers
	for(i = 0; i < VGA_NUM_GC_REGS; i++, regs++) {
		VGA_GC_INDEX.write(i);
		VGA_GC_DATA.write(*regs);
	}

	// Write Attribute Controller registers
	for(i = 0; i < VGA_NUM_AC_REGS; i++, regs++) {
		VGA_INSTAT_READ.read();
		VGA_AC_INDEX.write(i);
		VGA_AC_WRITE.write(*regs);
	}

	// Lock 16-colour palette and unblank display
	VGA_INSTAT_READ.read();
	VGA_AC_INDEX.write(0x20);
}

uint8_t* VGA::getFrameBufferSegment() {

}

uint8_t VGA::getColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b) {

}

bool VGA::setMode(common::uint32_t width, common::uint32_t height, common::uint32_t colorDepth) {
	if (!supportsMode(width, height, colorDepth))
		return false;
	
	// register sets
	unsigned char g_320x200x256[] =	{
	/* MISC */
		0x63,
	/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x0E,
	/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
		0xFF,
	/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
	/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00,	0x00
	};

	writeRegisters(g_320x200x256);
	return true;
}

// TODO: add support for other display modes
bool VGA::supportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colorDepth) {
	if (width == 320 && height == 200 && colorDepth == 8)
		return true;
	return false;
}

void VGA::putPixel(common::uint32_t x, common::uint32_t y,  common::uint8_t r, common::uint8_t g, common::uint8_t b) {

}

void VGA::putPixel(common::uint32_t x, common::uint32_t y, common::uint8_t colorIndex) {

}

