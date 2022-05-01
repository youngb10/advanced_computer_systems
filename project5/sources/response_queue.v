`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: response_queue
// Project Name: Memory_Controller
//
// Description: 
// Keeps the completed requests in FIFO buffers for the CPUs.
// 
// Dependencies:
// response_buffer
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module response_queue(
    input [66:0] completed_entry,
    input completed_ctrl,
    input cpu0_read_res,
    input cpu1_read_res,
    input cpu2_read_res,
    input cpu3_read_res,
    input reset,
    input clk,
    output [64:0] cpu0_res,
    output cpu0_res_ctrl,
    output [64:0] cpu1_res,
    output cpu1_res_ctrl,
    output [64:0] cpu2_res,
    output cpu2_res_ctrl,
    output [64:0] cpu3_res,
    output cpu3_res_ctrl
    );

// Signal Declaration
wire cpu0_wen;
wire cpu1_wen;
wire cpu2_wen;
wire cpu3_wen;

// If there is a completed request for a CPU, the write enable for that CPU FIFO goes high
assign cpu0_wen = completed_ctrl & ~completed_entry[66] & ~completed_entry[65];
assign cpu1_wen = completed_ctrl & ~completed_entry[66] & completed_entry[65];
assign cpu2_wen = completed_ctrl & completed_entry[66] & ~completed_entry[65];
assign cpu3_wen = completed_ctrl & completed_entry[66] & completed_entry[65];

response_buffer CPU0( // Response FIFO for CPU 0
    .in_data (completed_entry[64:0]),
    .wen (cpu0_wen),
    .read (cpu0_read_res),
    .reset (reset),
    .clk (clk),
    .out_data (cpu0_res),
    .res_ctrl (cpu0_res_ctrl)
);

response_buffer CPU1( // Response FIFO for CPU 1
    .in_data (completed_entry[64:0]),
    .wen (cpu1_wen),
    .read (cpu1_read_res),
    .reset (reset),
    .clk (clk),
    .out_data (cpu1_res),
    .res_ctrl (cpu1_res_ctrl)
);

response_buffer CPU2( // Response FIFO for CPU 2
    .in_data (completed_entry[64:0]),
    .wen (cpu2_wen),
    .read (cpu2_read_res),
    .reset (reset),
    .clk (clk),
    .out_data (cpu2_res),
    .res_ctrl (cpu2_res_ctrl)
);

response_buffer CPU3( // Response FIFO for CPU 3
    .in_data (completed_entry[64:0]),
    .wen (cpu3_wen),
    .read (cpu3_read_res),
    .reset (reset),
    .clk (clk),
    .out_data (cpu3_res),
    .res_ctrl (cpu3_res_ctrl)
);

endmodule
