#include "../photon/photon.h"

static const unsigned char font_bitmap[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x50, 0x50, 0xa0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x24, 0x24, 0x7e, 0x24, 0x24, 0x24, 0x7e, 0x24, 
	0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3c, 
	0x40, 0x40, 0x20, 0x18, 0x0c, 0x04, 0x04, 0x78, 
	0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x71, 0x00, 0x92, 0x00, 0x94, 0x00, 0x94, 
	0x00, 0x6b, 0x80, 0x0a, 0x80, 0x12, 0x80, 0x22, 
	0x80, 0x23, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x28, 
	0x32, 0x72, 0x4a, 0x44, 0x3a, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x40, 0x40, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x20, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00, 0x00, 
	0x00, 0xc0, 0x40, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 
	0x20, 0xa8, 0x70, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x10, 0x10, 0x20, 0x20, 0x20, 
	0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x10, 
	0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x78, 0x04, 0x04, 0x0c, 0x10, 0x20, 0x40, 0x40, 
	0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 
	0x04, 0x04, 0x04, 0x78, 0x04, 0x04, 0x04, 0x78, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 
	0x28, 0x48, 0x48, 0x88, 0xfc, 0x08, 0x08, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x40, 0x40, 
	0x40, 0x78, 0x04, 0x04, 0x04, 0x78, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x40, 0x40, 0x78, 
	0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7c, 0x04, 0x04, 0x08, 0x08, 
	0x10, 0x10, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x44, 
	0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x3c, 0x04, 
	0x04, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x10, 0x20, 0x60, 0x10, 0x0c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
	0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x08, 
	0x0c, 0x10, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x20, 0x60, 
	0x40, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 
	0x00, 0x61, 0x00, 0xde, 0x80, 0xa6, 0x80, 0xa4, 
	0x80, 0xa5, 0x00, 0xbe, 0x00, 0xc0, 0x00, 0x3c, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x28, 0x28, 0x24, 0x24, 0x7c, 0x46, 0x42, 
	0xc2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 
	0x44, 0x44, 0x44, 0x78, 0x44, 0x44, 0x44, 0x78, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x60, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 0x3c, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x44, 0x42, 
	0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x40, 0x40, 0x40, 
	0x7c, 0x40, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3c, 0x40, 0x40, 0x40, 0x7c, 
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3c, 0x40, 0x40, 0x40, 0x44, 0x44, 
	0x44, 0x64, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 
	0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x46, 0x4c, 
	0x58, 0x50, 0x60, 0x50, 0x58, 0x4c, 0x46, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x3c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
	0x00, 0x63, 0x00, 0x63, 0x00, 0x55, 0x00, 0x5d, 
	0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x62, 0x62, 0x72, 0x52, 0x5a, 0x4a, 
	0x4e, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3c, 0x66, 0x42, 0x42, 0x42, 0x42, 0x42, 
	0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x78, 0x44, 0x44, 0x44, 0x78, 0x40, 0x40, 0x40, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 
	0x66, 0x42, 0x42, 0x42, 0x42, 0x42, 0x64, 0x38, 
	0x10, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x42, 
	0x42, 0x42, 0x7c, 0x48, 0x44, 0x46, 0x42, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x40, 0x40, 
	0x20, 0x18, 0x0c, 0x04, 0x04, 0x78, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xfe, 0x10, 0x10, 0x10, 
	0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 
	0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xc2, 0x42, 0x42, 0x44, 0x24, 0x24, 
	0x2c, 0x38, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc0, 0x80, 0x40, 0x80, 
	0x40, 0x80, 0x4c, 0x80, 0x4c, 0x80, 0x54, 0x80, 
	0x53, 0x00, 0x33, 0x00, 0x33, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 
	0x64, 0x24, 0x18, 0x18, 0x18, 0x24, 0x64, 0x42, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x44, 
	0x6c, 0x28, 0x38, 0x10, 0x10, 0x10, 0x10, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x04, 0x08, 
	0x08, 0x10, 0x20, 0x20, 0x40, 0x7c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x40, 0x40, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x20, 
	0x20, 0x20, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xe0, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0xe0, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x70, 0x08, 0x08, 0x78, 0x48, 0x48, 0x78, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x70, 
	0x48, 0x48, 0x48, 0x48, 0x48, 0x70, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x38, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x08, 0x38, 0x48, 0x48, 
	0x48, 0x48, 0x48, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x7c, 
	0x40, 0x40, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x20, 0x70, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3c, 0x44, 0x44, 0x44, 0x78, 0x40, 
	0x7c, 0x44, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x40, 
	0x40, 0x78, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x4c, 0x58, 
	0x50, 0x60, 0x70, 0x58, 0x4c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x7f, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 
	0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x78, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x70, 0x48, 0x48, 0x48, 0x48, 0x48, 0x70, 0x40, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 
	0x48, 0x48, 0x48, 0x48, 0x48, 0x38, 0x08, 0x08, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x40, 0x40, 
	0x30, 0x08, 0x08, 0x78, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x20, 0x20, 0x78, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xc4, 0x4c, 0x48, 0x48, 0x28, 0x30, 
	0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x80, 
	0x4c, 0x80, 0x55, 0x80, 0x55, 0x00, 0x55, 0x00, 
	0x53, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 
	0x68, 0x30, 0x10, 0x38, 0x48, 0xc4, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x44, 
	0x48, 0x48, 0x28, 0x30, 0x30, 0x10, 0xe0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x08, 0x10, 
	0x30, 0x20, 0x40, 0x78, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x70, 0x40, 0x40, 0x40, 0x40, 0x80, 
	0x40, 0x40, 0x40, 0x40, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0xe0, 0x20, 0x20, 0x20, 0x20, 0x10, 0x20, 0x20, 
	0x20, 0x20, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x74, 0x5c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 
	0xa0, 0xa4, 0xac, 0xa0, 0xa0, 0xa0, 0xa0, 0xe0, 
	0x00, 0x00, 
};

static const font_dimensions_t font_widths[] = {
	{ 3, 0 },
	{ 3, 15 },
	{ 4, 30 },
	{ 8, 45 },
	{ 7, 60 },
	{ 10, 75 },
	{ 8, 105 },
	{ 2, 120 },
	{ 4, 135 },
	{ 4, 150 },
	{ 6, 165 },
	{ 7, 180 },
	{ 3, 195 },
	{ 4, 210 },
	{ 3, 225 },
	{ 4, 240 },
	{ 7, 255 },
	{ 7, 270 },
	{ 7, 285 },
	{ 7, 300 },
	{ 7, 315 },
	{ 7, 330 },
	{ 7, 345 },
	{ 7, 360 },
	{ 7, 375 },
	{ 7, 390 },
	{ 3, 405 },
	{ 3, 420 },
	{ 7, 435 },
	{ 7, 450 },
	{ 7, 465 },
	{ 5, 480 },
	{ 9, 495 },
	{ 8, 525 },
	{ 7, 540 },
	{ 7, 555 },
	{ 8, 570 },
	{ 7, 585 },
	{ 7, 600 },
	{ 7, 615 },
	{ 8, 630 },
	{ 3, 645 },
	{ 3, 660 },
	{ 7, 675 },
	{ 6, 690 },
	{ 9, 705 },
	{ 8, 735 },
	{ 8, 750 },
	{ 7, 765 },
	{ 8, 780 },
	{ 8, 795 },
	{ 7, 810 },
	{ 7, 825 },
	{ 8, 840 },
	{ 8, 855 },
	{ 10, 870 },
	{ 8, 900 },
	{ 7, 915 },
	{ 7, 930 },
	{ 4, 945 },
	{ 4, 960 },
	{ 4, 975 },
	{ 7, 990 },
	{ 6, 1005 },
	{ 4, 1020 },
	{ 6, 1035 },
	{ 6, 1050 },
	{ 5, 1065 },
	{ 6, 1080 },
	{ 7, 1095 },
	{ 5, 1110 },
	{ 7, 1125 },
	{ 7, 1140 },
	{ 3, 1155 },
	{ 3, 1170 },
	{ 6, 1185 },
	{ 3, 1200 },
	{ 9, 1215 },
	{ 7, 1245 },
	{ 7, 1260 },
	{ 6, 1275 },
	{ 6, 1290 },
	{ 4, 1305 },
	{ 6, 1320 },
	{ 5, 1335 },
	{ 7, 1350 },
	{ 6, 1365 },
	{ 9, 1380 },
	{ 7, 1410 },
	{ 6, 1425 },
	{ 6, 1440 },
	{ 4, 1455 },
	{ 3, 1470 },
	{ 4, 1485 },
	{ 7, 1500 },
	{ 6, 1515 },
};

// Neo Sans 9 font
const font_t neo_9_font = {
	15,
	32,
	127,
	font_bitmap,
	font_widths,
};
