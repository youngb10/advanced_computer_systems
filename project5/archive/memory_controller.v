`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: memory_controller
// Project Name: Memory_Controller
//
// Description: 
// Full memory controller containing all modules. Further information is available in the README. 
// 
// Dependencies:
// request_tracker
// schedule_algo
// sort_sixteen
// scheduled_buffer
// command_gen
// response_queue
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module memory_controller(
    input [66:0] cpu0_req, // CPU 0 incoming request data
    input cpu0_req_ctrl,   // CPU 0 incoming request enable
    input [66:0] cpu1_req, // CPU 1 incoming request data
    input cpu1_req_ctrl,   // CPU 1 incoming request enable
    input [66:0] cpu2_req, // CPU 2 incoming request data
    input cpu2_req_ctrl,   // CPU 2 incoming request enable
    input [66:0] cpu3_req, // CPU 3 incoming request data
    input cpu3_req_ctrl,   // CPU 3 incoming request enable
    input cpu0_read_res, // Indicates CPU 0 has read a response
    input cpu1_read_res, // Indicates CPU 1 has read a response
    input cpu2_read_res, // Indicates CPU 2 has read a response
    input cpu3_read_res, // Indicates CPU 3 has read a response
    input reset, // Memory controller reset
    input clk,   // Memory controller clock
    input [31:0] DRAM_data_rd, // Data read from the DRAM
    output [64:0] cpu0_res, // CPU 0 completed request
    output cpu0_res_ctrl,   // CPU 0 completed request notice
    output [64:0] cpu1_res, // CPU 1 completed request
    output cpu1_res_ctrl,   // CPU 1 completed request notice
    output [64:0] cpu2_res, // CPU 2 completed request
    output cpu2_res_ctrl,   // CPU 2 completed request notice
    output [64:0] cpu3_res, // CPU 3 completed request
    output cpu3_res_ctrl,   // CPU 3 completed request notice
    output CS_L,  // DRAM control signal
    output RAS_L, // DRAM control signal
    output CAS_L, // DRAM control signal
    output WE_L,  // DRAM control signal
    output [15:0] addr,     // DRAM row/col address
    output [1:0] addr_bank, // DRAM bank select
    output [31:0] DRAM_data_wr // Data to be written to the DRAM
    );

// Request Tracker Signal Declaration
wire [68:0] unscheduled_0;
wire [68:0] unscheduled_1;
wire [68:0] unscheduled_2;
wire [68:0] unscheduled_3;
wire [68:0] unscheduled_4;
wire [68:0] unscheduled_5;
wire [68:0] unscheduled_6;
wire [68:0] unscheduled_7;
wire [68:0] unscheduled_8;
wire [68:0] unscheduled_9;
wire [68:0] unscheduled_10;
wire [68:0] unscheduled_11;
wire [68:0] unscheduled_12;
wire [68:0] unscheduled_13;
wire [68:0] unscheduled_14;
wire [68:0] unscheduled_15;
wire [5:0] age_0;
wire [5:0] age_1;
wire [5:0] age_2;
wire [5:0] age_3;
wire [5:0] age_4;
wire [5:0] age_5;
wire [5:0] age_6;
wire [5:0] age_7;
wire [5:0] age_8;
wire [5:0] age_9;
wire [5:0] age_10;
wire [5:0] age_11;
wire [5:0] age_12;
wire [5:0] age_13;
wire [5:0] age_14;
wire [5:0] age_15;
// Request Tracker Instantiation
request_tracker request_tracker(
    .cpu0_req (cpu0_req),           // Input from memory_controller input
    .cpu0_req_ctrl (cpu0_req_ctrl), // Input from memory_controller input
    .cpu1_req (cpu1_req),           // Input from memory_controller input
    .cpu1_req_ctrl (cpu1_req_ctrl), // Input from memory_controller input
    .cpu2_req (cpu2_req),           // Input from memory_controller input
    .cpu2_req_ctrl (cpu2_req_ctrl), // Input from memory_controller input
    .cpu3_req (cpu3_req),           // Input from memory_controller input
    .cpu3_req_ctrl (cpu3_req_ctrl), // Input from memory_controller input
    .completed_entry (completed_entry[66:32]), // Input from command_gen
    .completed_ctrl (completed_ctrl),          // Input from command_gen
    .clk (clk),     // Input from memory_controller input
    .reset (reset), // Input from memory_controller input
    .unscheduled_0 (unscheduled_0), // Output to schedule_algo, sort_sixteen
    .valid_0 (valid_0),             // Output to schedule_algo, sort_sixteen
    .age_0 (age_0),                 // Output to schedule_algo
    .unscheduled_1 (unscheduled_1), // Output to schedule_algo, sort_sixteen
    .valid_1 (valid_1),             // Output to schedule_algo, sort_sixteen
    .age_1 (age_1),                 // Output to schedule_algo
    .unscheduled_2 (unscheduled_2), // Output to schedule_algo, sort_sixteen
    .valid_2 (valid_2),             // Output to schedule_algo, sort_sixteen
    .age_2 (age_2),                 // Output to schedule_algo
    .unscheduled_3 (unscheduled_3), // Output to schedule_algo, sort_sixteen
    .valid_3 (valid_3),             // Output to schedule_algo, sort_sixteen
    .age_3 (age_3),                 // Output to schedule_algo
    .unscheduled_4 (unscheduled_4), // Output to schedule_algo, sort_sixteen
    .valid_4 (valid_4),             // Output to schedule_algo, sort_sixteen
    .age_4 (age_4),                 // Output to schedule_algo
    .unscheduled_5 (unscheduled_5), // Output to schedule_algo, sort_sixteen
    .valid_5 (valid_5),             // Output to schedule_algo, sort_sixteen
    .age_5 (age_5),                 // Output to schedule_algo
    .unscheduled_6 (unscheduled_6), // Output to schedule_algo, sort_sixteen
    .valid_6 (valid_6),             // Output to schedule_algo, sort_sixteen
    .age_6 (age_6),                 // Output to schedule_algo
    .unscheduled_7 (unscheduled_7), // Output to schedule_algo, sort_sixteen
    .valid_7 (valid_7),             // Output to schedule_algo, sort_sixteen
    .age_7 (age_7),                 // Output to schedule_algo
    .unscheduled_8 (unscheduled_8), // Output to schedule_algo, sort_sixteen
    .valid_8 (valid_8),             // Output to schedule_algo, sort_sixteen
    .age_8 (age_8),                 // Output to schedule_algo
    .unscheduled_9 (unscheduled_9), // Output to schedule_algo, sort_sixteen
    .valid_9 (valid_9),             // Output to schedule_algo, sort_sixteen
    .age_9 (age_9),                 // Output to schedule_algo
    .unscheduled_10 (unscheduled_10), // Output to schedule_algo, sort_sixteen
    .valid_10 (valid_10),             // Output to schedule_algo, sort_sixteen
    .age_10 (age_10),                 // Output to schedule_algo
    .unscheduled_11 (unscheduled_11), // Output to schedule_algo, sort_sixteen
    .valid_11 (valid_11),             // Output to schedule_algo, sort_sixteen
    .age_11 (age_11),                 // Output to schedule_algo
    .unscheduled_12 (unscheduled_12), // Output to schedule_algo, sort_sixteen
    .valid_12 (valid_12),             // Output to schedule_algo, sort_sixteen
    .age_12 (age_12),                 // Output to schedule_algo
    .unscheduled_13 (unscheduled_13), // Output to schedule_algo, sort_sixteen
    .valid_13 (valid_13),             // Output to schedule_algo, sort_sixteen
    .age_13 (age_13),                 // Output to schedule_algo
    .unscheduled_14 (unscheduled_14), // Output to schedule_algo, sort_sixteen
    .valid_14 (valid_14),             // Output to schedule_algo, sort_sixteen
    .age_14 (age_14),                 // Output to schedule_algo
    .unscheduled_15 (unscheduled_15), // Output to schedule_algo, sort_sixteen
    .valid_15 (valid_15),             // Output to schedule_algo, sort_sixteen
    .age_15 (age_15)                  // Output to schedule_algo
);

// Schedule Algorithm Signal Declaration
wire [7:0] schedule_0;
wire [7:0] schedule_1;
wire [7:0] schedule_2;
wire [7:0] schedule_3;
wire [7:0] schedule_4;
wire [7:0] schedule_5;
wire [7:0] schedule_6;
wire [7:0] schedule_7;
wire [7:0] schedule_8;
wire [7:0] schedule_9;
wire [7:0] schedule_10;
wire [7:0] schedule_11;
wire [7:0] schedule_12;
wire [7:0] schedule_13;
wire [7:0] schedule_14;
wire [7:0] schedule_15;
// Schedule Algorithm Instantiation
schedule_algo schedule_algo(
    .unscheduled_0 (unscheduled_0[66:64]), // Input from request_tracker
    .valid_0 (valid_0), // Input from request_tracker
    .age_0 (age_0),     // Input from request_tracker
    .unscheduled_1 (unscheduled_1[66:64]), // Input from request_tracker
    .valid_1 (valid_1), // Input from request_tracker
    .age_1 (age_1),     // Input from request_tracker
    .unscheduled_2 (unscheduled_2[66:64]), // Input from request_tracker
    .valid_2 (valid_2), // Input from request_tracker
    .age_2 (age_2),     // Input from request_tracker
    .unscheduled_3 (unscheduled_3[66:64]), // Input from request_tracker
    .valid_3 (valid_3), // Input from request_tracker
    .age_3 (age_3),     // Input from request_tracker
    .unscheduled_4 (unscheduled_4[66:64]), // Input from request_tracker
    .valid_4 (valid_4), // Input from request_tracker
    .age_4 (age_4),     // Input from request_tracker
    .unscheduled_5 (unscheduled_5[66:64]), // Input from request_tracker
    .valid_5 (valid_5), // Input from request_tracker
    .age_5 (age_5),     // Input from request_tracker
    .unscheduled_6 (unscheduled_6[66:64]), // Input from request_tracker
    .valid_6 (valid_6), // Input from request_tracker
    .age_6 (age_6),     // Input from request_tracker
    .unscheduled_7 (unscheduled_7[66:64]), // Input from request_tracker
    .valid_7 (valid_7), // Input from request_tracker
    .age_7 (age_7),     // Input from request_tracker
    .unscheduled_8 (unscheduled_8[66:64]), // Input from request_tracker
    .valid_8 (valid_8), // Input from request_tracker
    .age_8 (age_8),     // Input from request_tracker
    .unscheduled_9 (unscheduled_9[66:64]), // Input from request_tracker
    .valid_9 (valid_9), // Input from request_tracker
    .age_9 (age_9),     // Input from request_tracker
    .unscheduled_10 (unscheduled_10[66:64]), // Input from request_tracker
    .valid_10 (valid_10), // Input from request_tracker
    .age_10 (age_10),     // Input from request_tracker
    .unscheduled_11 (unscheduled_11[66:64]), // Input from request_tracker
    .valid_11 (valid_11), // Input from request_tracker
    .age_11 (age_11),     // Input from request_tracker
    .unscheduled_12 (unscheduled_12[66:64]), // Input from request_tracker
    .valid_12 (valid_12), // Input from request_tracker
    .age_12 (age_12),     // Input from request_tracker
    .unscheduled_13 (unscheduled_13[66:64]), // Input from request_tracker
    .valid_13 (valid_13), // Input from request_tracker
    .age_13 (age_13),     // Input from request_tracker
    .unscheduled_14 (unscheduled_14[66:64]), // Input from request_tracker
    .valid_14 (valid_14), // Input from request_tracker
    .age_14 (age_14),     // Input from request_tracker
    .unscheduled_15 (unscheduled_15[66:64]), // Input from request_tracker
    .valid_15 (valid_15), // Input from request_tracker
    .age_15 (age_15),     // Input from request_tracker
    .schedule_0 (schedule_0), // Output to sort_sixteen
    .schedule_1 (schedule_1), // Output to sort_sixteen
    .schedule_2 (schedule_2), // Output to sort_sixteen
    .schedule_3 (schedule_3), // Output to sort_sixteen
    .schedule_4 (schedule_4), // Output to sort_sixteen
    .schedule_5 (schedule_5), // Output to sort_sixteen
    .schedule_6 (schedule_6), // Output to sort_sixteen
    .schedule_7 (schedule_7), // Output to sort_sixteen
    .schedule_8 (schedule_8), // Output to sort_sixteen
    .schedule_9 (schedule_9), // Output to sort_sixteen
    .schedule_10 (schedule_10), // Output to sort_sixteen
    .schedule_11 (schedule_11), // Output to sort_sixteen
    .schedule_12 (schedule_12), // Output to sort_sixteen
    .schedule_13 (schedule_13), // Output to sort_sixteen
    .schedule_14 (schedule_14), // Output to sort_sixteen
    .schedule_15 (schedule_15)  // Output to sort_sixteen
);

// Sort Sixteen Signal Declaration
wire [67:0] out0;
wire [67:0] out1;
wire [67:0] out2;
wire [67:0] out3;
wire [67:0] out4;
wire [67:0] out5;
wire [67:0] out6;
wire [67:0] out7;
wire [67:0] out8;
wire [67:0] out9;
wire [67:0] out10;
wire [67:0] out11;
wire [67:0] out12;
wire [67:0] out13;
wire [67:0] out14;
wire [67:0] out15;
// Sort Sixteen Instantiation
sort_sixteen sort_sixteen(
    .in0 ({schedule_0, valid_0, unscheduled_0[68:67], unscheduled_0[64:0]}), // Input from request_tracker, schedule_algo
    .in1 ({schedule_1, valid_1, unscheduled_1[68:67], unscheduled_1[64:0]}), // Input from request_tracker, schedule_algo
    .in2 ({schedule_2, valid_2, unscheduled_2[68:67], unscheduled_2[64:0]}), // Input from request_tracker, schedule_algo
    .in3 ({schedule_3, valid_3, unscheduled_3[68:67], unscheduled_3[64:0]}), // Input from request_tracker, schedule_algo
    .in4 ({schedule_4, valid_4, unscheduled_4[68:67], unscheduled_4[64:0]}), // Input from request_tracker, schedule_algo
    .in5 ({schedule_5, valid_5, unscheduled_5[68:67], unscheduled_5[64:0]}), // Input from request_tracker, schedule_algo
    .in6 ({schedule_6, valid_6, unscheduled_6[68:67], unscheduled_6[64:0]}), // Input from request_tracker, schedule_algo
    .in7 ({schedule_7, valid_7, unscheduled_7[68:67], unscheduled_7[64:0]}), // Input from request_tracker, schedule_algo
    .in8 ({schedule_8, valid_8, unscheduled_8[68:67], unscheduled_8[64:0]}), // Input from request_tracker, schedule_algo
    .in9 ({schedule_9, valid_9, unscheduled_9[68:67], unscheduled_9[64:0]}), // Input from request_tracker, schedule_algo
    .in10 ({schedule_10, valid_10, unscheduled_10[68:67], unscheduled_10[64:0]}), // Input from request_tracker, schedule_algo
    .in11 ({schedule_11, valid_11, unscheduled_11[68:67], unscheduled_11[64:0]}), // Input from request_tracker, schedule_algo
    .in12 ({schedule_12, valid_12, unscheduled_12[68:67], unscheduled_12[64:0]}), // Input from request_tracker, schedule_algo
    .in13 ({schedule_13, valid_13, unscheduled_13[68:67], unscheduled_13[64:0]}), // Input from request_tracker, schedule_algo
    .in14 ({schedule_14, valid_14, unscheduled_14[68:67], unscheduled_14[64:0]}), // Input from request_tracker, schedule_algo
    .in15 ({schedule_15, valid_15, unscheduled_15[68:67], unscheduled_15[64:0]}), // Input from request_tracker, schedule_algo
    .out0 (out0), // Output to scheduled_buffer
    .out1 (out1), // Output to scheduled_buffer
    .out2 (out2), // Output to scheduled_buffer
    .out3 (out3), // Output to scheduled_buffer
    .out4 (out4), // Output to scheduled_buffer
    .out5 (out5), // Output to scheduled_buffer
    .out6 (out6), // Output to scheduled_buffer
    .out7 (out7), // Output to scheduled_buffer
    .out8 (out8), // Output to scheduled_buffer
    .out9 (out9), // Output to scheduled_buffer
    .out10 (out10), // Output to scheduled_buffer
    .out11 (out11), // Output to scheduled_buffer
    .out12 (out12), // Output to scheduled_buffer
    .out13 (out13), // Output to scheduled_buffer
    .out14 (out14), // Output to scheduled_buffer
    .out15 (out15)  // Output to scheduled_buffer
);

// Scheduled Buffer Signal Declaration
wire ext_load;
wire inc;
wire [67:0] request_out;
wire [15:0] next_row_out;
// Scheduled Buffer Instantiation
scheduled_buffer scheduled_buffer(
    .in0 (out0), // Input from sort_sixteen
    .in1 (out1), // Input from sort_sixteen
    .in2 (out2), // Input from sort_sixteen
    .in3 (out3), // Input from sort_sixteen
    .in4 (out4), // Input from sort_sixteen
    .in5 (out5), // Input from sort_sixteen
    .in6 (out6), // Input from sort_sixteen
    .in7 (out7), // Input from sort_sixteen
    .in8 (out8), // Input from sort_sixteen
    .in9 (out9), // Input from sort_sixteen
    .in10 (out10), // Input from sort_sixteen
    .in11 (out11), // Input from sort_sixteen
    .in12 (out12), // Input from sort_sixteen
    .in13 (out13), // Input from sort_sixteen
    .in14 (out14), // Input from sort_sixteen
    .in15 (out15), // Input from sort_sixteen
    .ext_load (ext_load), // Input from command_gen
    .inc (inc),           // Input from command_gen
    .reset (reset), // Input from memory_controller input
    .clk (clk),     // Input from memory_controller input
    .request_out (request_out),  // Output to command_gen
    .next_row_out (next_row_out) // Output to command_gen
);

// Command Generator Signal Declaration
wire [66:0] completed_entry;
// Command Generator Instantiation
command_gen command_gen(
    .request_in (request_out), // Input from scheduled_buffer
    .next_row (next_row_out),  // Input from scheduled_buffer
    .reset (reset), // Input from memory_controller input
    .clk (clk),     // Input from memory_controller input
    .DRAM_data_rd (DRAM_data_rd), // Input from memory_controller input
    .completed_entry (completed_entry), // Output to request_tracker and response_queue
    .completed_ctrl (completed_ctrl),   // Output to request_tracker and response_queue
    .ext_load (ext_load), // Output to scheduled_buffer
    .inc (inc),           // Output to scheduled_buffer
    .CS_L (CS_L),   // Output to memory_controller output
    .RAS_L (RAS_L), // Output to memory_controller output
    .CAS_L (CAS_L), // Output to memory_controller output
    .WE_L (WE_L),   // Output to memory_controller output
    .addr (addr),   // Output to memory_controller output
    .addr_bank (addr_bank), // Output to memory_controller output
    .DRAM_data_wr (DRAM_data_wr) // Output to memory_controller output
);

// Response Queue Instantiation
response_queue response_queue(
    .completed_entry (completed_entry), // Input from command_gen
    .completed_ctrl (completed_ctrl),   // Input from command_gen
    .cpu0_read_res (cpu0_read_res), // Input from memory_controller input
    .cpu1_read_res (cpu1_read_res), // Input from memory_controller input
    .cpu2_read_res (cpu2_read_res), // Input from memory_controller input
    .cpu3_read_res (cpu3_read_res), // Input from memory_controller input
    .reset (reset), // Input from memory_controller input
    .clk (clk),     // Input from memory_controller input
    .cpu0_res (cpu0_res),           // Output from memory_controller output
    .cpu0_res_ctrl (cpu0_res_ctrl), // Output from memory_controller output
    .cpu1_res (cpu1_res),           // Output from memory_controller output
    .cpu1_res_ctrl (cpu1_res_ctrl), // Output from memory_controller output
    .cpu2_res (cpu2_res),           // Output from memory_controller output
    .cpu2_res_ctrl (cpu2_res_ctrl), // Output from memory_controller output
    .cpu3_res (cpu3_res),           // Output from memory_controller output
    .cpu3_res_ctrl (cpu3_res_ctrl)  // Output from memory_controller output
);

endmodule
