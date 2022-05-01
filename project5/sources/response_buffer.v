`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: response_buffer
// Project Name: Memory_Controller
//
// Description: 
// FIFO buffer for outgoing data.
// 
// Dependencies:
// None
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module response_buffer(
    input [64:0] in_data,
    input wen,
    input read,
    input reset,
    input clk,
    output reg [64:0] out_data,
    output res_ctrl
    );

// Signal Declaration
reg [66:0] FIFO [0:7];
reg [2:0] ptr_read;
reg [2:0] ptr_write;
reg [4:0] count;
wire empty;
wire full;

// Empty/Full Logic
assign empty = (count==0)? 1'b1:1'b0;
assign full = (count==8)? 1'b1:1'b0;
assign res_ctrl = ~empty;

// FIFO
always @(posedge clk)
begin
    if (reset) begin
        ptr_read = 3'b000;
        ptr_write = 3'b000;
        count = 4'b0000;
    end
    else if (wen & ~full) begin
        FIFO[ptr_write] = in_data;
        ptr_write = ptr_write + 3'b001;
        count = count + 4'b0001;
    end
    else if (read & ~empty) begin
        out_data = FIFO[ptr_read];
        ptr_read = ptr_read + 3'b001;
        count = count - 4'b0001;
    end
    if (ptr_read == 8) begin
        ptr_read = 3'b000;
    end
    if (ptr_write == 8) begin
        ptr_write = 3'b000;
    end
end

endmodule
