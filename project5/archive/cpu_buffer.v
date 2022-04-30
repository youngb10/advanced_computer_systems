`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/24/2022 02:05:59 PM
// Design Name: 
// Module Name: cpu_buffer
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module cpu_buffer(
    input [66:0] cpu_data,
    input cpu_req,
    input clk,
    input read,
    input reset,
    output reg [66:0] out_data,
    output empty,
    output full,
    output reg [3:0] count
    );

// Signal Declaration
reg [66:0] FIFO [0:7];
reg [2:0] ptr_read;
reg [2:0] ptr_write;

// Empty/Full Logic
assign empty = (count==0)? 1'b1:1'b0;
assign full = (count==8)? 1'b1:1'b0;

// FIFO
always @(posedge clk)
begin
    if (reset) begin
        ptr_read = 3'b000;
        ptr_write = 3'b000;
        count = 4'b0000;
    end
    else if (cpu_req & ~full) begin
        FIFO[ptr_write] = cpu_data;
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
