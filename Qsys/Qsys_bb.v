
module Qsys (
	altpll_0_areset_conduit_export,
	altpll_0_locked_conduit_export,
	clk_clk,
	clk_sdram_clk,
	d8m_xclkin_clk,
	i2c_opencores_camera_export_scl_pad_io,
	i2c_opencores_camera_export_sda_pad_io,
	i2c_opencores_mipi_export_scl_pad_io,
	i2c_opencores_mipi_export_sda_pad_io,
	mipi_pwdn_n_external_connection_export,
	mipi_reset_n_external_connection_export,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	terasic_camera_0_conduit_end_D,
	terasic_camera_0_conduit_end_FVAL,
	terasic_camera_0_conduit_end_LVAL,
	terasic_camera_0_conduit_end_PIXCLK,
	uart_0_rx_tx_rxd,
	uart_0_rx_tx_txd);	

	input		altpll_0_areset_conduit_export;
	output		altpll_0_locked_conduit_export;
	input		clk_clk;
	output		clk_sdram_clk;
	output		d8m_xclkin_clk;
	inout		i2c_opencores_camera_export_scl_pad_io;
	inout		i2c_opencores_camera_export_sda_pad_io;
	inout		i2c_opencores_mipi_export_scl_pad_io;
	inout		i2c_opencores_mipi_export_sda_pad_io;
	output		mipi_pwdn_n_external_connection_export;
	output		mipi_reset_n_external_connection_export;
	input		reset_reset_n;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	input	[11:0]	terasic_camera_0_conduit_end_D;
	input		terasic_camera_0_conduit_end_FVAL;
	input		terasic_camera_0_conduit_end_LVAL;
	input		terasic_camera_0_conduit_end_PIXCLK;
	input		uart_0_rx_tx_rxd;
	output		uart_0_rx_tx_txd;
endmodule
