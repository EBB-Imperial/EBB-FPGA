#include "Frame_Reader.hpp"

Frame_Reader::Frame_Reader(unsigned long base_address, int irq_number)
: VipCore(base_address, irq_number) {
}


void Frame_Reader::set_frame_information(unsigned int width, unsigned int height, unsigned int interlace_nibble) {
    do_write(VFR_FRAME_INFORMATION, (width & 0x7FF) | ((height & 0x7FF) << 13) | ((interlace_nibble & 0xF) << 26));
}
