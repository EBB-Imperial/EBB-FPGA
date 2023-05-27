#ifndef __FRAME_READER_HPP__
#define __FRAME_READER_HPP__

#include "VipCore.hpp"


class Frame_Reader : public VipCore {
private:
    Frame_Reader(const Frame_Reader&);  // Disable copy constructor

public:
    enum VFRRegisterType {
        VFR_FRAME_COUNTER        = 3,
        VFR_REPEAT_COUNTER       = 4,
        VFR_FRAME_INFORMATION    = 5,
        VFR_FRAME_START_ADDR     = 6,
        VFR_READER_INFO          = 7,
    };

    /*
     * @brief   The constructor
     * @param   base_address   base address of the slave interface of the core
     * @param   irq_number     interrupt is compulsory for a Frame_Reader, set to -1 only if the irq line is not connected
     */
    Frame_Reader(unsigned long base_address, int irq_number);

    /*
     * @return    The maximum width of frames that may be read (as parameterized)
     */
    inline unsigned int get_max_width() const {
        return (do_read(VFR_READER_INFO) >> 13) & 0x00001FFF;
    }

    /*
     * @return    The maximum height of frames that may be read (as parameterized)
     */
    inline unsigned int get_max_height() const {
        return do_read(VFR_READER_INFO) & 0x00001FFF;
    }

    /*
     * @return    Whether the reader is ready to accept the details of the next frame to be read
     */
    inline bool is_ready() const {
        return do_read(VFR_READER_INFO) & 0x04000000;
    }

    /*
     * @param   address   The address of the next frame/field in memory
     * @post    Set the buffer base address for the next frame/field to be read
     */
    inline void set_frame_address(unsigned long address) {
        do_write(VFR_FRAME_START_ADDR, address);
    }

    /*
     * @param   width                The width of the next frame/field
     * @param   height               The height of the next frame/field
     * @param   interlace_nibble     The interlace nibble used in the control packet that will precede the next frame
     * @post    Set the dimensions of the next frame to be read
     */
    void set_frame_information(unsigned int width, unsigned int height, unsigned int interlace_nibble);

    /*
    inline void enable_frame_interrupt() {
        enable_interrupt(0);
    }

    inline void disable_frame_interrupt() {
        disable_interrupt(0);
    }
    */

    /*
     * @post   Clear the frame ready interrupt
     */
    inline void clear_frame_interrupt() {
        write_interrupt_register(0);
    }
};


#endif   // __FRAME_READER_HPP__
