#include <stdio.h>
#include "I2C_core.h"
#include "terasic_includes.h"
#include "mipi_camera_config.h"
#include "mipi_bridge_config.h"

#include <system.h>
#include <altera_avalon_uart_regs.h>
#include <altera_avalon_pio_regs.h>
#include <string.h>

#define EXPOSURE_INIT 0x001000
#define GAIN_INIT 0x400
#define FOCUS_INIT 100
#define DEFAULT_LEVEL 3

#define FRAME_FIRST_LINE 5
#define FRAME_LAST_LINE 475

#define CMD_RESET_READ_POINTER 0x00
#define CMD_READ_BLOCK 0x01

#define MIPI_REG_PHYClkCtl		0x0056
#define MIPI_REG_PHYData0Ctl	0x0058
#define MIPI_REG_PHYData1Ctl	0x005A
#define MIPI_REG_PHYData2Ctl	0x005C
#define MIPI_REG_PHYData3Ctl	0x005E
#define MIPI_REG_PHYTimDly		0x0060
#define MIPI_REG_PHYSta			0x0062
#define MIPI_REG_CSIStatus		0x0064
#define MIPI_REG_CSIErrEn		0x0066
#define MIPI_REG_MDLSynErr		0x0068
#define MIPI_REG_FrmErrCnt		0x0080
#define MIPI_REG_MDLErrCnt		0x0090

void mipi_clear_error(void);
void mipi_show_error_info(void);
void mipi_show_error_info_more(void);
bool MIPI_Init(void);
alt_u16 rgb888_to_rgb565(alt_u32 rgb888);

int main() {
	IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, 0b0000011111); // For debugging: light up LEDs to show firmware is running

	fprintf(stderr, "DE10-LITE D8M VGA Demo\n");
	fprintf(stderr, "Imperial College EEE2 Project version\n");
	IOWR(MIPI_PWDN_N_BASE, 0x00, 0x00);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0x00);

	usleep(2000);
	IOWR(MIPI_PWDN_N_BASE, 0x00, 0xFF);
	usleep(2000);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0xFF);
	usleep(2000);

	// MIPI Init
	if (!MIPI_Init()) {
		fprintf(stderr, "MIPI_Init Init failed!\r\n");
	} else {
		fprintf(stderr, "MIPI_Init Init successfully!\r\n");
	}

	mipi_clear_error();
	usleep(50 * 1000);
	mipi_clear_error();
	usleep(50 * 1000);
	mipi_show_error_info();

	OV8865SetExposure(EXPOSURE_INIT);
	OV8865SetGain(GAIN_INIT);
	OV8865_FOCUS_Move_to(FOCUS_INIT);

	// Set ESP32 UART baud rate to 1M (with clock = 50M)
	IOWR_ALTERA_AVALON_UART_DIVISOR(UART_0_BASE, 49);

	// Output image dimensions: 320 * 236
	while (1) {
		static alt_u16 next_x, next_y;

		fprintf(stderr, "Brewing tea... ");

		alt_u8 instruction = fgetc(stdin);
		if (instruction == CMD_RESET_READ_POINTER) {
			next_x = 0;
			next_y = FRAME_FIRST_LINE;
			fprintf(stderr, "    Command from ESP32: 0x%02X CMD_RESET_READ_POINTER\n", instruction);
		} else if (instruction == CMD_READ_BLOCK) {
			alt_u8 length_lsb = fgetc(stdin);
			alt_u8 length_msb = fgetc(stdin);
			alt_u16 req_byte_count = (length_msb << 8) | length_lsb;
			alt_u16 req_pixel_count = req_byte_count / 2;
			fprintf(stderr, "    Command from ESP32: 0x%02X CMD_READ_BLOCK, param 0x%04X = %u\n", instruction, req_byte_count, req_byte_count);

			for (unsigned i = 0; i < req_pixel_count; i++) {
				alt_u32 pixel_index = next_y * 640 + next_x + 6;
				alt_u16 pixel_rgb565 = rgb888_to_rgb565(IORD(SDRAM_BASE, pixel_index));

				putchar(pixel_rgb565 & 0xff);
				putchar((pixel_rgb565 >> 8) & 0xff);

				next_x += 2;
				if (next_x >= 640) {
					next_x = 0;
					next_y += 2;
				}
				if (next_y >= FRAME_LAST_LINE) {
					next_y = FRAME_FIRST_LINE;
				}
			}
		} else {
			fprintf(stderr, "!!! Unknown command from ESP32: 0x%02X\n", instruction);
		}


//		for (alt_u16 y = 5; y < 476; y += 2) {
//			fprintf(stderr, "%u ", y);
//
//			// Read one line from the framebuffer, skipping every other pixel, and compress into RGB565
//			alt_u16 line[320];
//			for(alt_u16 x = 0; x < 640; x += 2) {
//				line[x / 2] = rgb888_to_rgb565(IORD(SDRAM_BASE, y * 640 + x + 6));
//			}
//
//			// Dump to the UART
//			fwrite(line, sizeof(line[0]), sizeof(line)/sizeof(line[0]), stdout);
//		}
//
//		usleep(200 * 1000);
	}
}


void mipi_clear_error(void) {
	MipiBridgeRegWrite(MIPI_REG_CSIStatus, 0x01FF); // clear error
	MipiBridgeRegWrite(MIPI_REG_MDLSynErr, 0x0000); // clear error
	MipiBridgeRegWrite(MIPI_REG_FrmErrCnt, 0x0000); // clear error
	MipiBridgeRegWrite(MIPI_REG_MDLErrCnt, 0x0000); // clear error

	MipiBridgeRegWrite(0x0082, 0x00);
	MipiBridgeRegWrite(0x0084, 0x00);
	MipiBridgeRegWrite(0x0086, 0x00);
	MipiBridgeRegWrite(0x0088, 0x00);
	MipiBridgeRegWrite(0x008A, 0x00);
	MipiBridgeRegWrite(0x008C, 0x00);
	MipiBridgeRegWrite(0x008E, 0x00);
	MipiBridgeRegWrite(0x0090, 0x00);
}

void mipi_show_error_info(void) {

	alt_u16 PHY_status, SCI_status, MDLSynErr, FrmErrCnt, MDLErrCnt;

	PHY_status = MipiBridgeRegRead(MIPI_REG_PHYSta);
	SCI_status = MipiBridgeRegRead(MIPI_REG_CSIStatus);
	MDLSynErr = MipiBridgeRegRead(MIPI_REG_MDLSynErr);
	FrmErrCnt = MipiBridgeRegRead(MIPI_REG_FrmErrCnt);
	MDLErrCnt = MipiBridgeRegRead(MIPI_REG_MDLErrCnt);
	fprintf(stderr,
			"PHY_status=%xh, CSI_status=%xh, MDLSynErr=%xh, FrmErrCnt=%xh, MDLErrCnt=%xh\r\n",
			PHY_status, SCI_status, MDLSynErr, FrmErrCnt, MDLErrCnt);
}

void mipi_show_error_info_more(void) {
	fprintf(stderr, "FrmErrCnt = %d\n", MipiBridgeRegRead(0x0080));
	fprintf(stderr, "CRCErrCnt = %d\n", MipiBridgeRegRead(0x0082));
	fprintf(stderr, "CorErrCnt = %d\n", MipiBridgeRegRead(0x0084));
	fprintf(stderr, "HdrErrCnt = %d\n", MipiBridgeRegRead(0x0086));
	fprintf(stderr, "EIDErrCnt = %d\n", MipiBridgeRegRead(0x0088));
	fprintf(stderr, "CtlErrCnt = %d\n", MipiBridgeRegRead(0x008A));
	fprintf(stderr, "SoTErrCnt = %d\n", MipiBridgeRegRead(0x008C));
	fprintf(stderr, "SynErrCnt = %d\n", MipiBridgeRegRead(0x008E));
	fprintf(stderr, "MDLErrCnt = %d\n", MipiBridgeRegRead(0x0090));
	fprintf(stderr, "FIFOSTATUS = %d\n", MipiBridgeRegRead(0x00F8));
	fprintf(stderr, "DataType = 0x%04x\n", MipiBridgeRegRead(0x006A));
	fprintf(stderr, "CSIPktLen = %d\n", MipiBridgeRegRead(0x006E));
}

bool MIPI_Init(void) {
	bool bSuccess;

	bSuccess = oc_i2c_init_ex(I2C_OPENCORES_MIPI_BASE, 50 * 1000 * 1000,
			400 * 1000); //I2C: 400K
	if (!bSuccess)
		fprintf(stderr, "failed to init MIPI- Bridge i2c\r\n");

	usleep(50 * 1000);
	MipiBridgeInit();

	usleep(500 * 1000);

	MipiCameraInit();
	MIPI_BIN_LEVEL(DEFAULT_LEVEL);

	usleep(1000);

	return bSuccess;
}

inline alt_u16 rgb888_to_rgb565(alt_u32 rgb888)
{
	return ((rgb888 & 0xf80000) >> 8) | ((rgb888 & 0x00fc00) >> 5) | ((rgb888 & 0x0000f8) >> 3);
}
