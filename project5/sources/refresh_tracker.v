`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: refresh_tracker
// Project Name: Memory_Controller
//
// Description: 
// Determines when the DRAM requires a refresh.
// 
// Dependencies:
// None
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module refresh_tracker(
    input clk,
    input reset,
    input refreshed,
    output reg refresh
    );

// Signal Delcaration
reg [22:0] refresh_counter;
wire refresh_set;

// Refresh occurs every 64ms
// With a clock frequency of 100MHz, 6400000 cycles may pass per refresh
// 6400000 in binary is 0b11000011010100000000000
// A refresh will be queued whenever the two most significant bits of the counter are high

// Counts the cycles since the last refresh
always @(posedge clk)
begin
    // Typically a reset would not be desired here, but it is necessary for simulation
    if (refreshed | reset) begin
        refresh_counter = 23'b00000000000000000000000;
    end
    else refresh_counter = refresh_counter + 1;
end

// If 63ms have passed since the last refresh, a refresh is queued
assign refresh_set = refresh_counter[22] & refresh_counter[21];

// Queues the refresh if refresh_set is high, resets the value after the refresh is successful
always @(posedge clk)
begin
    if (refreshed | reset) begin
        refresh = 1'b0;
    end
    else if (refresh_set) begin
        refresh = 1'b1;
    end
end

endmodule
