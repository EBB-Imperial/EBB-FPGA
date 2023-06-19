module FpsMonitor(
	input					clk50,
	input     			vs,

	// output frame pixel data
	output reg [7:0]		fps,
	output wire [6:0]		hex_fps_h,
	output wire [6:0]		hex_fps_l
	
);


parameter  ONE_SEC = 32'd50_000_000;

reg [3:0]  fps_h;
reg [3:0]  fps_l;

reg [7:0]  rfps;
reg [3:0]  rfps_l;
reg [3:0]  rfps_h;

reg [26:0] sec_cnt;
reg        pre_vs;
wire       one_sec_mask;

assign one_sec_mask = (sec_cnt>= (ONE_SEC - 1'b1) )?1'b1:1'b0;

always @(posedge clk50) 
  if(one_sec_mask) sec_cnt <= 27'h0;
  else             sec_cnt <= sec_cnt + 1'b1;
 
 
always @(posedge clk50) begin
	  pre_vs <= vs;
	  if(sec_cnt == 27'd0) begin
		 rfps   <= 8'd0;
		 rfps_h <= 4'd0;
		 rfps_l <= 4'd0;
	  end
	  else if({pre_vs,vs} == 2'b01) begin
		  rfps <= rfps + 1'b1;
		  
		  if(rfps_l == 4'd9) begin
			  rfps_l <= 4'd0;
			  rfps_h <=  rfps_h + 1'b1;
		  end
		  else rfps_l <= rfps_l + 1'b1;
	  end
	  
end
  

always @ (posedge clk50) 
  if(one_sec_mask) begin
    fps    <= rfps;
    fps_h  <= rfps_h;
    fps_l  <= rfps_l;
  end

assign hex_fps_h = (fps_h == 4'd0)?8'hC0: //0
                   (fps_h == 4'd1)?8'hF9: //1
				   	 (fps_h == 4'd2)?8'hA4: //2
                   (fps_h == 4'd3)?8'hB0: //3
				   	 (fps_h == 4'd4)?8'h99: //4        
				    	 (fps_h == 4'd5)?8'h92: //5
					    (fps_h == 4'd6)?8'h82: //6
					    (fps_h == 4'd7)?8'hF8: //7
					    (fps_h == 4'd8)?8'h80: //8
                                   7'h90; //9
											  
assign hex_fps_l = (fps_l == 4'd0)?8'hC0: //0
                   (fps_l == 4'd1)?8'hF9: //1
				   	 (fps_l == 4'd2)?8'hA4: //2
                   (fps_l == 4'd3)?8'hB0: //3
				   	 (fps_l == 4'd4)?8'h99: //4        
				    	 (fps_l == 4'd5)?8'h92: //5
					    (fps_l == 4'd6)?8'h82: //6
					    (fps_l == 4'd7)?8'hF8: //7
					    (fps_l == 4'd8)?8'h80: //8
                                   8'h90; //9
endmodule

