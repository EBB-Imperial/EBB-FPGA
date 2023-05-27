#include <stdio.h>
#include "I2C_core.h"
#include "terasic_includes.h"
#include "mipi_camera_config.h"
#include "mipi_bridge_config.h"

#include <fcntl.h>
#include <unistd.h>

#include <altera_avalon_uart.h>

#include "system.h"

#define EXPOSURE_INIT 0x80000
#define GAIN_INIT 0x700
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

	MipiCameraInit();
	MIPI_BIN_LEVEL(DEFAULT_LEVEL);

	usleep(1000);

	return bSuccess;
}

int main() {

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	fprintf(stderr, "DE10-LITE D8M VGA Demo");
	fprintf(stderr, "Imperial College EEE2 Project version");
	IOWR(MIPI_PWDN_N_BASE, 0x00, 0x00);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0x00);

	usleep(2000);
	IOWR(MIPI_PWDN_N_BASE, 0x00, 0xFF);
	usleep(2000);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0xFF);
	usleep(2000);

	// MIPI Init
	if (!MIPI_Init()) {
		fprintf(stderr, "MIPI_Init Init failed!\n");
	} else {
		fprintf(stderr, "MIPI_Init Init successfully!\n");
	}

	mipi_clear_error();
	usleep(50 * 1000);
	mipi_clear_error();
	usleep(500 * 1000);
	mipi_show_error_info();

	OV8865SetGain(GAIN_INIT);
	OV8865SetExposure(EXPOSURE_INIT);

	while (1) {
		static alt_u32 frame_count = 0;

		fprintf(stderr, "Sending frame %lu\n", frame_count);

		for (alt_u16 line = 0; line < 480; line++) {
			for (alt_u16 x = 0; x < 640; x++) {
				alt_u32 offset = line * 640 + x;
				alt_u32 sdram_word = IORD(SDRAM_BASE, offset);
				putchar((sdram_word >> 16) & 0xff);
				putchar((sdram_word >> 8) & 0xff);
				putchar((sdram_word) & 0xff);
			}
		}

		frame_count++;
		usleep(1000 * 1000);
	};

	return 0;
}
