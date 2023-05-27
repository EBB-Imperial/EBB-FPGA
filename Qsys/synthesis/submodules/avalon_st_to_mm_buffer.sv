module avalon_st_to_mm_buffer(
	input logic reset,
	
	// Avalon streaming interface sink
	input logic 		sink_clock,
	input logic[23:0] sink_data,
	input logic			sink_valid,
	output logic		sink_ready,
	input logic			sink_endofpacket,
	input logic			sink_startofpacket,
	
	input logic 			mm_clock,
	input logic				mm_chipselect,
	input logic				mm_address,
	input logic 			mm_read,
	output logic[31:0] 	mm_readdata,
	input logic 			mm_write,
	input logic[31:0]		mm_writedata
);

parameter bufferSize = 640 * 480;

`define REG_DATA 0
`define REG_POINTER 1


logic[31:0] buffer[bufferSize];
logic[31:0] write_ptr, read_ptr;

assign sink_ready = 1; // Always ready :)

always @(posedge sink_clock) begin
	if (reset) write_ptr <= 0;
	
	if (sink_valid) begin
		buffer[write_ptr] <= {8'b0, sink_data};
		if (write_ptr == bufferSize - 1) begin
			write_ptr <= 0;
		end else begin
			write_ptr <= write_ptr + 1;
		end
	end
end

always @(posedge mm_clock) begin
	if (reset) begin 
		read_ptr <= 0;
		mm_readdata <= 0;
	end
	
	if (mm_read && mm_address == `REG_DATA) mm_readdata <= buffer[read_ptr];
	if (mm_write && mm_address == `REG_POINTER) read_ptr <= mm_writedata;
end


endmodule