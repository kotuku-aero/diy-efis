#include "../photon/photon.h"

static const unsigned char font_bitmap[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x00, 0x00, 0x30, 0x30, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x33, 0x00, 0x33, 0x00, 0x66, 0x00, 0x66, 0x00, 
	0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x30, 0x06, 0x30, 
	0x06, 0x30, 0x04, 0x30, 0x0c, 0x30, 0x3f, 0xfe, 
	0x3f, 0xfe, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 
	0x0c, 0x20, 0x7f, 0xfc, 0x7f, 0xfc, 0x0c, 0x60, 
	0x08, 0x60, 0x08, 0x60, 0x18, 0x60, 0x18, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x0f, 0xf0, 0x1f, 0xf0, 0x38, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x1c, 0x00, 0x0e, 0x00, 
	0x07, 0x80, 0x01, 0xe0, 0x00, 0x70, 0x00, 0x38, 
	0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x38, 
	0x3f, 0xf0, 0x3f, 0xc0, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3e, 0x03, 0x00, 0x3e, 0x06, 0x00, 
	0x63, 0x0c, 0x00, 0x63, 0x0c, 0x00, 0x63, 0x18, 
	0x00, 0x63, 0x18, 0x00, 0x63, 0x30, 0x00, 0x63, 
	0x60, 0x00, 0x3e, 0x6f, 0x80, 0x1e, 0xcf, 0x80, 
	0x00, 0xd8, 0xc0, 0x01, 0x98, 0xc0, 0x01, 0x18, 
	0xc0, 0x03, 0x18, 0xc0, 0x06, 0x18, 0xc0, 0x06, 
	0x18, 0xc0, 0x0c, 0x0f, 0x80, 0x18, 0x07, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x1f, 
	0xe0, 0x00, 0x38, 0x70, 0x00, 0x30, 0x30, 0x00, 
	0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x18, 0x60, 
	0x00, 0x0c, 0xc0, 0x00, 0x0f, 0x80, 0x00, 0x0f, 
	0x06, 0x00, 0x33, 0x86, 0x00, 0x21, 0xc6, 0x00, 
	0x60, 0xe6, 0x00, 0x60, 0x7c, 0x00, 0x60, 0x3c, 
	0x00, 0x70, 0x7c, 0x00, 0x3f, 0xfe, 0x00, 0x0f, 
	0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x30, 0x60, 0x60, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0c, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 
	0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 
	0x30, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0c, 0x0c, 
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x18, 
	0x18, 0x18, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 
	0x7f, 0xe0, 0x1f, 0x80, 0x0f, 0x00, 0x19, 0x80, 
	0x19, 0x80, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x7f, 0xf8, 
	0x7f, 0xf8, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x30, 0x30, 0x60, 0x60, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
	0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x0c, 0x00, 
	0x0c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x10, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xc0, 0x1f, 0xf0, 0x38, 0x38, 0x30, 0x18, 
	0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 
	0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 
	0x60, 0x0c, 0x60, 0x0c, 0x30, 0x18, 0x38, 0x38, 
	0x1f, 0xf0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x0f, 0x80, 
	0x1d, 0x80, 0x11, 0x80, 0x01, 0x80, 0x01, 0x80, 
	0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 
	0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 
	0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0xe0, 0x1f, 0xf0, 0x00, 0x38, 0x00, 0x18, 
	0x00, 0x18, 0x00, 0x18, 0x00, 0x30, 0x00, 0x70, 
	0x01, 0xc0, 0x03, 0x80, 0x0e, 0x00, 0x1c, 0x00, 
	0x18, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x3f, 0xf8, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xe0, 
	0x00, 0x70, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 
	0x00, 0x30, 0x00, 0x60, 0x3f, 0xe0, 0x3f, 0xe0, 
	0x00, 0x60, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 
	0x00, 0x30, 0x00, 0x70, 0x3f, 0xe0, 0x3f, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x70, 0x00, 0xf0, 0x00, 0xb0, 0x01, 0xb0, 
	0x03, 0x30, 0x06, 0x30, 0x0c, 0x30, 0x08, 0x30, 
	0x18, 0x30, 0x30, 0x30, 0x60, 0x30, 0x7f, 0xfe, 
	0x7f, 0xfe, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 
	0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x1f, 0xf0, 
	0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x1f, 0xc0, 0x1f, 0xf0, 0x00, 0x70, 
	0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 
	0x00, 0x18, 0x00, 0x70, 0x3f, 0xf0, 0x1f, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xe0, 0x1f, 0xf0, 0x38, 0x00, 0x70, 0x00, 
	0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x7f, 0xe0, 
	0x7f, 0xf8, 0x60, 0x38, 0x60, 0x0c, 0x60, 0x0c, 
	0x60, 0x0c, 0x60, 0x0c, 0x30, 0x1c, 0x38, 0x38, 
	0x1f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x3f, 0xf8, 
	0x00, 0x38, 0x00, 0x30, 0x00, 0x70, 0x00, 0x60, 
	0x00, 0x60, 0x00, 0xc0, 0x00, 0xc0, 0x01, 0x80, 
	0x01, 0x80, 0x03, 0x00, 0x03, 0x00, 0x06, 0x00, 
	0x06, 0x00, 0x0e, 0x00, 0x0c, 0x00, 0x1c, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xe0, 0x3f, 0xf8, 0x38, 0x38, 0x60, 0x0c, 
	0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 0x30, 0x18, 
	0x3f, 0xf8, 0x3f, 0xf8, 0x30, 0x18, 0x60, 0x0c, 
	0x60, 0x0c, 0x60, 0x0c, 0x60, 0x0c, 0x38, 0x38, 
	0x1f, 0xf8, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x1f, 0xe0, 
	0x38, 0x70, 0x70, 0x38, 0x60, 0x18, 0x60, 0x18, 
	0x60, 0x18, 0x60, 0x18, 0x30, 0x18, 0x3f, 0xf8, 
	0x0f, 0xf8, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 
	0x00, 0x38, 0x00, 0x70, 0x3f, 0xe0, 0x1f, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
	0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
	0x30, 0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x70, 0x00, 0xc0, 0x01, 0x80, 
	0x07, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x1c, 0x00, 
	0x0e, 0x00, 0x07, 0x00, 0x01, 0x80, 0x00, 0xc0, 
	0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1c, 0x00, 0x0e, 0x00, 0x07, 0x00, 
	0x01, 0x80, 0x00, 0xc0, 0x00, 0x60, 0x00, 0x70, 
	0x00, 0xc0, 0x01, 0x80, 0x07, 0x00, 0x0e, 0x00, 
	0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0x00, 0x3f, 0x80, 0x00, 0xc0, 0x00, 0xc0, 
	0x00, 0xc0, 0x00, 0xc0, 0x01, 0x80, 0x03, 0x00, 
	0x07, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0xf8, 0x00, 0x07, 0xfe, 0x00, 0x0e, 0x07, 
	0x00, 0x1c, 0x01, 0x80, 0x39, 0xf8, 0xc0, 0x33, 
	0xf8, 0xc0, 0x33, 0x18, 0xc0, 0x66, 0x18, 0xc0, 
	0x66, 0x18, 0xc0, 0x66, 0x18, 0xc0, 0x66, 0x19, 
	0xc0, 0x66, 0x19, 0x80, 0x66, 0x1f, 0x80, 0x63, 
	0xff, 0x00, 0x71, 0xf8, 0x00, 0x30, 0x00, 0x00, 
	0x1c, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x03, 0xf8, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
	0x07, 0xc0, 0x06, 0xc0, 0x06, 0xe0, 0x0e, 0x60, 
	0x0c, 0x60, 0x0c, 0x60, 0x0c, 0x30, 0x18, 0x30, 
	0x18, 0x30, 0x1f, 0xf0, 0x3f, 0xf8, 0x30, 0x18, 
	0x30, 0x18, 0x30, 0x1c, 0x70, 0x0c, 0x60, 0x0c, 
	0x60, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xe0, 0x3f, 0xf8, 0x30, 0x38, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x18, 0x3f, 0xf0, 0x3f, 0xf0, 0x30, 0x18, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x18, 0x3f, 0xf8, 0x3f, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 
	0x0f, 0xf8, 0x1c, 0x00, 0x18, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x18, 0x00, 0x1c, 0x00, 0x0f, 0xf8, 
	0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xf0, 0x30, 0x38, 
	0x30, 0x18, 0x30, 0x1c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x1c, 0x30, 0x18, 
	0x30, 0x38, 0x3f, 0xf0, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 
	0x1f, 0xf0, 0x38, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x3f, 0xf0, 
	0x3f, 0xf0, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1f, 0xf0, 
	0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xf0, 0x1f, 0xf0, 0x38, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 
	0x1f, 0xf8, 0x38, 0x00, 0x30, 0x00, 0x60, 0x00, 
	0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x60, 0x18, 0x60, 0x18, 0x60, 0x18, 0x60, 0x18, 
	0x70, 0x18, 0x30, 0x18, 0x38, 0x18, 0x1f, 0xf8, 
	0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x3f, 0xfc, 0x3f, 0xfc, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x18, 0x30, 0x30, 0x30, 0x70, 
	0x30, 0xe0, 0x31, 0xc0, 0x33, 0x80, 0x33, 0x00, 
	0x36, 0x00, 0x3c, 0x00, 0x3e, 0x00, 0x37, 0x00, 
	0x33, 0x00, 0x33, 0x80, 0x31, 0xc0, 0x30, 0xe0, 
	0x30, 0x70, 0x30, 0x38, 0x30, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x38, 0x00, 0x1f, 0xf0, 
	0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x38, 0x07, 0x00, 0x38, 0x07, 0x00, 0x3c, 0x0f, 
	0x00, 0x3c, 0x0f, 0x00, 0x36, 0x1b, 0x00, 0x36, 
	0x1b, 0x00, 0x33, 0x33, 0x00, 0x33, 0x33, 0x00, 
	0x31, 0xe3, 0x00, 0x31, 0xe3, 0x00, 0x30, 0xc3, 
	0x00, 0x30, 0x03, 0x00, 0x30, 0x03, 0x00, 0x30, 
	0x03, 0x00, 0x30, 0x03, 0x00, 0x30, 0x03, 0x00, 
	0x30, 0x03, 0x00, 0x30, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x0c, 0x38, 0x0c, 
	0x3c, 0x0c, 0x3c, 0x0c, 0x36, 0x0c, 0x36, 0x0c, 
	0x33, 0x0c, 0x33, 0x0c, 0x31, 0x8c, 0x31, 0x8c, 
	0x30, 0xcc, 0x30, 0xcc, 0x30, 0x6c, 0x30, 0x6c, 
	0x30, 0x3c, 0x30, 0x3c, 0x30, 0x1c, 0x30, 0x1c, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xe0, 0x1f, 0xf8, 0x38, 0x1c, 0x30, 0x0c, 
	0x70, 0x0e, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 
	0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 0x60, 0x06, 
	0x60, 0x06, 0x70, 0x0e, 0x30, 0x0c, 0x38, 0x1c, 
	0x1f, 0xf8, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xf0, 
	0x30, 0x70, 0x30, 0x18, 0x30, 0x18, 0x30, 0x18, 
	0x30, 0x18, 0x30, 0x18, 0x30, 0x70, 0x3f, 0xe0, 
	0x3f, 0xc0, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xe0, 0x0f, 0xf0, 0x1c, 0x38, 0x18, 0x18, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x18, 0x18, 0x1c, 0x38, 
	0x0f, 0xf0, 0x03, 0xc0, 0x01, 0x80, 0x01, 0x80, 
	0x01, 0xf0, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xf0, 
	0x30, 0x70, 0x30, 0x18, 0x30, 0x18, 0x30, 0x18, 
	0x30, 0x18, 0x30, 0x18, 0x30, 0x70, 0x3f, 0xe0, 
	0x3f, 0xc0, 0x31, 0xc0, 0x30, 0xe0, 0x30, 0x60, 
	0x30, 0x70, 0x30, 0x38, 0x30, 0x18, 0x30, 0x1c, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0xe0, 0x3f, 0xe0, 0x70, 0x00, 0x60, 0x00, 
	0x60, 0x00, 0x70, 0x00, 0x38, 0x00, 0x1e, 0x00, 
	0x0f, 0x00, 0x03, 0xc0, 0x00, 0xe0, 0x00, 0x70, 
	0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x70, 
	0x3f, 0xe0, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x7f, 0xf8, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 
	0x30, 0x0c, 0x30, 0x0c, 0x38, 0x1c, 0x1c, 0x38, 
	0x0f, 0xf0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x60, 0x0e, 0x60, 0x0c, 
	0x70, 0x0c, 0x30, 0x0c, 0x30, 0x18, 0x30, 0x18, 
	0x18, 0x18, 0x18, 0x38, 0x18, 0x30, 0x1c, 0x30, 
	0x0c, 0x30, 0x0c, 0x60, 0x0c, 0x60, 0x06, 0x60, 
	0x06, 0xe0, 0x06, 0xc0, 0x07, 0xc0, 0x03, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 
	0x60, 0x60, 0x00, 0x60, 0x60, 0x00, 0xe0, 0x70, 
	0x00, 0xc0, 0x70, 0x00, 0xc0, 0x30, 0x00, 0xc0, 
	0x30, 0xf0, 0xc0, 0x30, 0xf0, 0xc0, 0x30, 0xf0, 
	0xc0, 0x31, 0xb9, 0xc0, 0x39, 0x99, 0x80, 0x19, 
	0x99, 0x80, 0x19, 0x99, 0x80, 0x1b, 0x0d, 0x80, 
	0x1b, 0x0d, 0x80, 0x1b, 0x0f, 0x80, 0x1e, 0x0f, 
	0x00, 0x0e, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x70, 0x0c, 0x30, 0x1c, 0x38, 0x18, 
	0x1c, 0x30, 0x0c, 0x70, 0x0e, 0x60, 0x06, 0xe0, 
	0x03, 0xc0, 0x03, 0x80, 0x03, 0x80, 0x07, 0xc0, 
	0x06, 0xe0, 0x0e, 0x60, 0x0c, 0x30, 0x18, 0x38, 
	0x38, 0x18, 0x30, 0x1c, 0x60, 0x0e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x18, 
	0x70, 0x38, 0x30, 0x30, 0x38, 0x70, 0x18, 0x60, 
	0x18, 0x60, 0x0c, 0xc0, 0x0c, 0xc0, 0x0f, 0xc0, 
	0x07, 0x80, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf8, 0x00, 0x38, 
	0x00, 0x30, 0x00, 0x70, 0x00, 0x60, 0x00, 0xc0, 
	0x01, 0xc0, 0x01, 0x80, 0x03, 0x00, 0x07, 0x00, 
	0x06, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x18, 0x00, 
	0x30, 0x00, 0x3f, 0xf8, 0x3f, 0xf8, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3e, 0x3e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x3e, 0x3e, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x60, 0x00, 0x20, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x06, 0x00, 
	0x06, 0x00, 0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7c, 0x7c, 0x0c, 0x0c, 0x0c, 0x0c, 
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x7c, 
	0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 
	0x0e, 0xc0, 0x0c, 0xe0, 0x38, 0x70, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xff, 0xf0, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x30, 0x00, 
	0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x7f, 0x80, 
	0x01, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x1f, 0xc0, 
	0x3f, 0xc0, 0x70, 0xc0, 0x60, 0xc0, 0x60, 0xc0, 
	0x61, 0xc0, 0x3f, 0xc0, 0x1e, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x3f, 0x80, 0x3f, 0xe0, 0x30, 0x60, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x60, 0x3f, 0xe0, 
	0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x3f, 0x80, 
	0x30, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x30, 0x00, 0x3f, 0x80, 0x0f, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 
	0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 
	0x0f, 0xe0, 0x3f, 0xe0, 0x30, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x30, 0x60, 0x3f, 0xe0, 
	0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x3f, 0xe0, 
	0x30, 0x60, 0x60, 0x30, 0x60, 0x30, 0x7f, 0xf0, 
	0x7f, 0xf0, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 
	0x30, 0x00, 0x3f, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 
	0x1f, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0xff, 0x00, 0xff, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x3f, 0xf0, 
	0x70, 0x60, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 
	0x70, 0x70, 0x3f, 0xe0, 0x7f, 0xc0, 0x60, 0x00, 
	0x3c, 0x00, 0x33, 0x80, 0x60, 0x60, 0x60, 0x30, 
	0x60, 0x30, 0x70, 0x70, 0x3f, 0xe0, 0x1f, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x37, 0x80, 0x3f, 0xc0, 0x30, 0xe0, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 
	0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xf0, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x30, 0x60, 0x30, 0xc0, 0x31, 0x80, 0x33, 0x00, 
	0x36, 0x00, 0x3e, 0x00, 0x3c, 0x00, 0x36, 0x00, 
	0x37, 0x00, 0x33, 0x80, 0x31, 0xc0, 0x30, 0xc0, 
	0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xdf, 0x00, 
	0x3f, 0xff, 0x80, 0x38, 0x61, 0xc0, 0x30, 0x60, 
	0xc0, 0x30, 0x60, 0xc0, 0x30, 0x60, 0xc0, 0x30, 
	0x60, 0xc0, 0x30, 0x60, 0xc0, 0x30, 0x60, 0xc0, 
	0x30, 0x60, 0xc0, 0x30, 0x60, 0xc0, 0x30, 0x60, 
	0xc0, 0x30, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x3f, 0xc0, 
	0x38, 0xe0, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0x80, 0x3f, 0xe0, 0x30, 0x60, 0x60, 0x30, 
	0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 
	0x60, 0x30, 0x60, 0x30, 0x30, 0x60, 0x3f, 0xe0, 
	0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x3f, 0xe0, 
	0x30, 0x60, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x60, 0x3f, 0xe0, 0x3f, 0x80, 0x30, 0x00, 
	0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xc0, 0x3f, 0xe0, 0x30, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0x60, 0x60, 0x30, 0x60, 0x3f, 0xe0, 
	0x0f, 0xe0, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 
	0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x37, 0x3f, 0x38, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0x80, 0x7f, 0x80, 0x60, 0x00, 
	0x60, 0x00, 0x70, 0x00, 0x3c, 0x00, 0x0f, 0x00, 
	0x03, 0x80, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 
	0x7f, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0xff, 0x00, 
	0xff, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 
	0x18, 0x00, 0x18, 0x00, 0x1f, 0x00, 0x0f, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x30, 0x60, 0x30, 0x60, 0x38, 0xe0, 
	0x1f, 0xe0, 0x0f, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 
	0x60, 0x60, 0x30, 0x60, 0x30, 0x60, 0x30, 0xe0, 
	0x38, 0xc0, 0x18, 0xc0, 0x18, 0xc0, 0x19, 0x80, 
	0x0d, 0x80, 0x0d, 0x80, 0x0f, 0x80, 0x07, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xc0, 
	0x60, 0xe0, 0xc0, 0x60, 0xe1, 0xc0, 0x31, 0xb1, 
	0x80, 0x31, 0xb1, 0x80, 0x31, 0xb1, 0x80, 0x31, 
	0x31, 0x80, 0x33, 0x1b, 0x00, 0x1b, 0x1b, 0x00, 
	0x1b, 0x1b, 0x00, 0x1a, 0x1b, 0x00, 0x1e, 0x0f, 
	0x00, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x30, 0x30, 0x60, 
	0x18, 0xc0, 0x1c, 0xc0, 0x0d, 0x80, 0x07, 0x00, 
	0x07, 0x00, 0x07, 0x80, 0x0d, 0x80, 0x18, 0xc0, 
	0x38, 0xe0, 0x30, 0x60, 0x60, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x60, 0x30, 0x60, 0x70, 0x30, 0x60, 0x30, 0x60, 
	0x30, 0x60, 0x38, 0xc0, 0x18, 0xc0, 0x18, 0xc0, 
	0x19, 0xc0, 0x0d, 0x80, 0x0d, 0x80, 0x0f, 0x80, 
	0x07, 0x00, 0x03, 0x00, 0x03, 0x00, 0x07, 0x00, 
	0x7e, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x3f, 0xe0, 
	0x00, 0x60, 0x00, 0xc0, 0x01, 0x80, 0x03, 0x00, 
	0x06, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 
	0x60, 0x00, 0x7f, 0xe0, 0x7f, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1e, 0x3e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x60, 0x40, 0x60, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x3e, 0x1e, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x78, 0x7c, 0x0c, 0x0c, 
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x06, 0x02, 
	0x06, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
	0x0c, 0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1c, 0x10, 0x3f, 0x18, 0x33, 0x98, 
	0x31, 0xf8, 0x30, 0x70, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7c, 0x00, 0x44, 0x00, 0x44, 0x00, 
	0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x10, 
	0x45, 0x10, 0x45, 0x90, 0x45, 0xf0, 0x44, 0x70, 
	0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 
	0x44, 0x00, 0x44, 0x00, 0x7c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
};

static const font_dimensions_t font_widths[] = {
	{ 6, 0 },
	{ 5, 30 },
	{ 9, 60 },
	{ 16, 120 },
	{ 15, 180 },
	{ 19, 240 },
	{ 17, 330 },
	{ 5, 420 },
	{ 7, 450 },
	{ 7, 480 },
	{ 11, 510 },
	{ 15, 570 },
	{ 5, 630 },
	{ 8, 660 },
	{ 5, 690 },
	{ 9, 720 },
	{ 15, 780 },
	{ 15, 840 },
	{ 15, 900 },
	{ 15, 960 },
	{ 15, 1020 },
	{ 15, 1080 },
	{ 15, 1140 },
	{ 15, 1200 },
	{ 15, 1260 },
	{ 15, 1320 },
	{ 5, 1380 },
	{ 5, 1410 },
	{ 15, 1440 },
	{ 15, 1500 },
	{ 15, 1560 },
	{ 11, 1620 },
	{ 18, 1680 },
	{ 15, 1770 },
	{ 15, 1830 },
	{ 13, 1890 },
	{ 15, 1950 },
	{ 13, 2010 },
	{ 13, 2070 },
	{ 15, 2130 },
	{ 16, 2190 },
	{ 6, 2250 },
	{ 6, 2280 },
	{ 15, 2310 },
	{ 12, 2370 },
	{ 18, 2430 },
	{ 16, 2520 },
	{ 16, 2580 },
	{ 14, 2640 },
	{ 15, 2700 },
	{ 15, 2760 },
	{ 13, 2820 },
	{ 14, 2880 },
	{ 16, 2940 },
	{ 15, 3000 },
	{ 20, 3060 },
	{ 16, 3150 },
	{ 14, 3210 },
	{ 14, 3270 },
	{ 8, 3330 },
	{ 9, 3360 },
	{ 8, 3420 },
	{ 15, 3450 },
	{ 12, 3510 },
	{ 9, 3570 },
	{ 12, 3630 },
	{ 13, 3690 },
	{ 11, 3750 },
	{ 13, 3810 },
	{ 13, 3870 },
	{ 9, 3930 },
	{ 13, 3990 },
	{ 13, 4050 },
	{ 6, 4110 },
	{ 6, 4140 },
	{ 12, 4170 },
	{ 6, 4230 },
	{ 20, 4260 },
	{ 13, 4350 },
	{ 13, 4410 },
	{ 13, 4470 },
	{ 13, 4530 },
	{ 8, 4590 },
	{ 11, 4620 },
	{ 9, 4680 },
	{ 13, 4740 },
	{ 13, 4800 },
	{ 19, 4860 },
	{ 13, 4950 },
	{ 13, 5010 },
	{ 12, 5070 },
	{ 8, 5130 },
	{ 6, 5160 },
	{ 8, 5190 },
	{ 15, 5220 },
	{ 12, 5280 },
};

// Neo Sans 18 font
const font_t neo_18_font = {
	30,
	32,
	127,
	font_bitmap,
	font_widths,
};
