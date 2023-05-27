#ifndef __FRAME_WRITER_HPP__
#define __FRAME_WRITER_HPP__

#include "VipCore.hpp"

class Frame_Writer : public VipCore {
private:
    Frame_Writer(const Frame_Writer&);  // Disable copy constructor

public:
    Frame_Writer(long base_address);
};

#endif   // __FRAME_WRITER_HPP__
