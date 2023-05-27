#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

#include "VipCore.hpp"


class Frame_Buffer : public VipCore {
private:
    Frame_Buffer(const VipCore&);  // Disable copy constructor

public:
    // Frame buffer specific registers
    enum VFBRegisterType {
        VFB_FRAME_COUNTER         = 3,
        VFB_DROP_REPEAT_COUNTER   = 4,
        VFB_MISC                  = 8,
        VFB_LOCKED_MODE           = 9,
        VFB_INPUT_FRAME_RATE      = 10,
        VFB_OUTPUT_FRAME_RATE     = 11,
    };

    enum VFBMiscBit {
        // Bit 0 is reserved
        USER_PACKET_AFFINITY      = 1,
        // Bits 2-15 are reserved
        FRAME_DELAY_BASE          = 16,
    };
    static const unsigned int FRAME_DELAY_BITS = 12;

    enum UserPacketAffinity {
        DROP_OR_REPEAT_WITH_ASSOCIATED_FRAME,
        CONSERVE_USER_PACKETS
    };

    Frame_Buffer(unsigned long base_address, bool writer_interface, int irq_number=-1);

    /*
     * @brief  disable locked frame rate conversion mode
     * @post   the frame buffer will drop and/or repeat frame in a non-deterministic way
     *         to manage differences between input and output throughput.
     * @pre    dropping and/or repeating were enabled in HW parameterization
     * @pre    user defined rate conversion was enabled in HW parameterization
     */
    inline void disable_locked_mode() {
        do_write(VFB_LOCKED_MODE, 0);
    }

    /*
     * @brief  enable locked frame rate conversion mode
     * @post   the frame buffer will drop and/or repeat frame in a deterministic way
     *         to match the requested output frame rate for the given input frame rate
     *         (using Bresenham's algorithm)
     * @pre    users should not be using this feature if the output video clock rate is not
     *         locked to the input video clock rate
     */
    inline void enable_locked_mode(unsigned int input_frame_rate, unsigned int output_frame_rate) {
        do_write(VFB_INPUT_FRAME_RATE, input_frame_rate);
        do_write(VFB_OUTPUT_FRAME_RATE, output_frame_rate);
        do_write(VFB_LOCKED_MODE, 1);
    }

    /*
     * @brief    Return the number of frames received by the frame buffer
     * @return   The number of input frame (writer interface) or the number of input frames
     *           that have not been dropped (reader interface)
     */
    inline unsigned int get_frame_counter() {
        return do_read(VFB_FRAME_COUNTER);
    }

    /*
     * @return   The number of frames dropped by the writer side
     * @pre      writer interface only
     */
    inline unsigned int get_number_dropped_frames() {
        assert(writer_interface);
        return do_read(VFB_DROP_REPEAT_COUNTER);
    }

    /*
     * @return   The number of frames repeated by the reader side (ie, the counter is re
     * @pre      reader interface only
     */
    inline unsigned int get_number_repeated_frames() {
        assert(!writer_interface);
        return do_read(VFB_DROP_REPEAT_COUNTER);
    }

    /*
     * @brief    Change the handling of user packets
     * @param    user_packet_affinity      whether user packet should be dropped/repeated along with the frame
     *                                     they are associated with or should be passed on with drop/repeat
     * @pre      Whatever the mode chosen, you must ensure there is enough buffering per frame
     *           to store all user packets to avoid them being overwritten
     */
    void set_user_packet_affinity(UserPacketAffinity user_packet_affinity);

    /*
     * @brief    Change/increase the frame buffer latency
     * @param    The new delay (in number of input frames)
     * @pre      The delay is larger than 1 and enough HW buffers have been declared to handle this delay
     */
    void set_frame_delay(unsigned int delay);

private:
    bool writer_interface;
    unsigned int misc_reg;
};

#endif   // __FRAME_BUFFER_H__
