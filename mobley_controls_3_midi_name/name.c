// To give your project a unique name, this code must be
// placed into a .c file (its own tab).  It can not be in
// a .cpp file or your main sketch (the .ino file).

#include "usb_names.h"

// Edit these lines to create your own name.  The length must
// match the number of characters in your custom name.

//#define MIDI_NAME   {'D','A','U','G','H','T','E','R','S','H','I','P','_','C','1'}
//#define MIDI_NAME_LEN  15

#define MIDI_NAME   {'D','A','U','G','H','T','E','R','S','H','I','P'}
#define MIDI_NAME_LEN  12


// Do not change this part.  This exact format is required by USB.

struct usb_string_descriptor_struct usb_string_product_name = {
        2 + MIDI_NAME_LEN * 2,
        3,
        MIDI_NAME
};
