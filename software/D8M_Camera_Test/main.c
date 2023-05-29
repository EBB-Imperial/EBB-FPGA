#include <stdio.h>
#include "I2C_core.h"
#include "terasic_includes.h"
#include "mipi_camera_config.h"
#include "mipi_bridge_config.h"

#include <system.h>
#include <altera_avalon_uart_regs.h>
#include <string.h>

#define EXPOSURE_INIT 0x000800
#define EXPOSURE_STEP 0x100
#define GAIN_INIT 0x040
#define GAIN_STEP 0x040
#define DEFAULT_LEVEL 3

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

//	bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
//	if (!bSuccess)
//		fprintf(stderr, "failed to init MIPI- Camera i2c\r\n");

	MipiCameraInit();
	MIPI_BIN_LEVEL(DEFAULT_LEVEL);
//    OV8865_FOCUS_Move_to(340);

//    oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);  // Release I2C bus , due to two I2C master shared!

	usleep(1000);

//    oc_i2c_uninit(I2C_OPENCORES_MIPI_BASE);

	return bSuccess;
}

alt_u16 rgb888_to_rgb565(alt_u32 rgb888);

int main() {

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

	alt_u32 exposureTime = EXPOSURE_INIT;
	alt_u16 gain = GAIN_INIT;

	OV8865SetExposure(exposureTime);
	OV8865SetGain(gain);

	// Set ESP32 UART baud rate to 1M
	IOWR_ALTERA_AVALON_UART_DIVISOR(UART_0_BASE, 49);

	while (1) {

		// Output image dimensions: 320 * 235

		static alt_u32 frame_counter = 0;
		fprintf(stderr, "\n\nSending frame %lu: ", frame_counter);
		fputs("UUUUUUUUUUUUUUUw", stdout); // Sync word

		for (alt_u16 y = 5; y < 476; y += 2) {
			fprintf(stderr, "%u ", y);

			alt_u16 line[320];
			for (alt_u16 x = 0; x < 640; x += 2) {
				alt_u32 pixel_offset = y * 640 + x;
				alt_u32 pixel = IORD(SDRAM_BASE, pixel_offset);
				line[x / 2] = rgb888_to_rgb565(pixel);
			}
			fwrite(&line, sizeof(line), 1, stdout);
		}

		frame_counter++;
		usleep(100 * 1000);
	}
}

inline alt_u16 rgb888_to_rgb565(alt_u32 rgb888)
{
	return ((rgb888 & 0xf80000) >> 8) | ((rgb888 & 0x00fc00) >> 5) | ((rgb888 & 0x0000f8) >> 3);
}
