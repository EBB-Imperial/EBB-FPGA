#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting SOF File: /home/shuyuan/Documents/College/Project/EBB-FPGA/output_files/DE10_LITE_D8M_VIP_time_limited.sof to: "../flash/DE10_LITE_D8M_VIP_time_limited_onchip_flash_0.flash"
#
sof2flash --input="/home/shuyuan/Documents/College/Project/EBB-FPGA/output_files/DE10_LITE_D8M_VIP_time_limited.sof" --output="../flash/DE10_LITE_D8M_VIP_time_limited_onchip_flash_0.flash" --offset=0x0 

#
# Programming File: "../flash/DE10_LITE_D8M_VIP_time_limited_onchip_flash_0.flash" To Device: onchip_flash_0
#
nios2-flash-programmer "../flash/DE10_LITE_D8M_VIP_time_limited_onchip_flash_0.flash" --base=0x80000 --accept-bad-sysid --program 

#
# Converting ELF File: D8M_Camera_Test.elf to: "../flash/D8M_Camera_Test_onchip_flash_0.flash"
#
elf2flash --input="D8M_Camera_Test.elf" --output="../flash/D8M_Camera_Test_onchip_flash_0.flash" --boot="$SOPC_KIT_NIOS2/components/altera_nios2/boot_loader_cfi.srec" --base=0x80000 --end=0xf0000 --reset=0x80000 

#
# Programming File: "../flash/D8M_Camera_Test_onchip_flash_0.flash" To Device: onchip_flash_0
#
nios2-flash-programmer "../flash/D8M_Camera_Test_onchip_flash_0.flash" --base=0x80000 --accept-bad-sysid --program 

