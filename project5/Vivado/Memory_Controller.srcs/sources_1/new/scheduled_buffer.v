`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: scheduled_buffer
// Project Name: Memory_Controller
//
// Description: 
// Holds the active requests in scheduled order.
// 
// Dependencies:
// None
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module scheduled_buffer(
    input [67:0] in0,
    input [67:0] in1,
    input [67:0] in2,
    input [67:0] in3,
    input [67:0] in4,
    input [67:0] in5,
    input [67:0] in6,
    input [67:0] in7,
    input [67:0] in8,
    input [67:0] in9,
    input [67:0] in10,
    input [67:0] in11,
    input [67:0] in12,
    input [67:0] in13,
    input [67:0] in14,
    input [67:0] in15,
    input ext_load,
    input inc,
    input reset,
    input clk,
    output [67:0] request_out,
    output [15:0] next_row_out
    );

// Signal Declaration
reg [67:0] scheduled_buffer [0:15];
reg [3:0] ptr_read;
reg [6:0] load_counter;
reg load;
integer i;

// Load Counter
always @(posedge clk)
begin
    if (reset | load) begin
        load_counter = 7'b0000000;
    end
    else if (load_counter==7'b1111111);
    else load_counter = load_counter + 1;
end

always @(*)
begin
    // Only loads if next buffer is ready and the command generator is ready
    if ((load_counter > 7'b1100000 & inc) | ext_load) begin
        load = 1;
    end
    else load = 0;
end

// Scheduled Buffer
always @(posedge clk)
begin
    if (reset) begin
        for (i=0; i<16; i=i+1) scheduled_buffer[i][67] = 1'b0;
        ptr_read = 4'b0000;
    end
    else if (load) begin
        scheduled_buffer[0] = in0;
        scheduled_buffer[1] = in1;
        scheduled_buffer[2] = in2;
        scheduled_buffer[3] = in3;
        scheduled_buffer[4] = in4;
        scheduled_buffer[5] = in5;
        scheduled_buffer[6] = in6;
        scheduled_buffer[7] = in7;
        scheduled_buffer[8] = in8;
        scheduled_buffer[9] = in9;
        scheduled_buffer[10] = in10;
        scheduled_buffer[11] = in11;
        scheduled_buffer[12] = in12;
        scheduled_buffer[13] = in13;
        scheduled_buffer[14] = in14;
        scheduled_buffer[15] = in15;
        ptr_read = 4'b0000;
    end
    else if (inc) begin
        ptr_read = ptr_read + 1;
    end
end

// Outputs the next request to be completed
assign request_out = scheduled_buffer[ptr_read];
assign next_row_out = scheduled_buffer[ptr_read + 1][63:48];

endmodule
