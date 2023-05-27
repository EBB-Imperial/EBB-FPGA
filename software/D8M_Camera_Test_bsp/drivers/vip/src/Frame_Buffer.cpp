#include "Frame_Buffer.hpp"

#include <cassert>

Frame_Buffer::Frame_Buffer(unsigned long base_address, bool writer_interface, int irq_number) :
    VipCore(base_address, irq_number), misc_reg(0) {
    do_write(VFB_MISC, misc_reg);
}

void Frame_Buffer::set_user_packet_affinity(UserPacketAffinity user_packet_affinity) {
    if (user_packet_affinity == DROP_OR_REPEAT_WITH_ASSOCIATED_FRAME) {
        misc_reg = misc_reg & ~(1 << USER_PACKET_AFFINITY);
    } else {
        misc_reg = misc_reg | (1 << USER_PACKET_AFFINITY);
    }
    do_write(VFB_MISC, misc_reg);
}

void Frame_Buffer::set_frame_delay(unsigned int delay) {
    assert(delay >= 1);
    misc_reg = (misc_reg & ((1 << FRAME_DELAY_BASE) - 1)) | (delay << FRAME_DELAY_BASE);
    do_write(VFB_MISC, misc_reg);
}
