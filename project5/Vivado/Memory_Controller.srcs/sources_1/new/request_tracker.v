`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: request_tracker
// Project Name: Memory_Controller
//
// Description: 
// Keeps track of all active request properties and removes completed requests from the queue.
// 
// Dependencies:
// cpu_buffer
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module request_tracker(
    input [66:0] cpu0_req,
    input cpu0_req_ctrl,
    input [66:0] cpu1_req,
    input cpu1_req_ctrl,
    input [66:0] cpu2_req,
    input cpu2_req_ctrl,
    input [66:0] cpu3_req,
    input cpu3_req_ctrl,
    input [66:32] completed_entry,
    input completed_ctrl,
    input clk,
    input reset,
    output reg [68:0] unscheduled_0,
    output reg valid_0,
    output reg [5:0] age_0,
    output reg [68:0] unscheduled_1,
    output reg valid_1,
    output reg [5:0] age_1,
    output reg [68:0] unscheduled_2,
    output reg valid_2,
    output reg [5:0] age_2,
    output reg [68:0] unscheduled_3,
    output reg valid_3,
    output reg [5:0] age_3,
    output reg [68:0] unscheduled_4,
    output reg valid_4,
    output reg [5:0] age_4,
    output reg [68:0] unscheduled_5,
    output reg valid_5,
    output reg [5:0] age_5,
    output reg [68:0] unscheduled_6,
    output reg valid_6,
    output reg [5:0] age_6,
    output reg [68:0] unscheduled_7,
    output reg valid_7,
    output reg [5:0] age_7,
    output reg [68:0] unscheduled_8,
    output reg valid_8,
    output reg [5:0] age_8,
    output reg [68:0] unscheduled_9,
    output reg valid_9,
    output reg [5:0] age_9,
    output reg [68:0] unscheduled_10,
    output reg valid_10,
    output reg [5:0] age_10,
    output reg [68:0] unscheduled_11,
    output reg valid_11,
    output reg [5:0] age_11,
    output reg [68:0] unscheduled_12,
    output reg valid_12,
    output reg [5:0] age_12,
    output reg [68:0] unscheduled_13,
    output reg valid_13,
    output reg [5:0] age_13,
    output reg [68:0] unscheduled_14,
    output reg valid_14,
    output reg [5:0] age_14,
    output reg [68:0] unscheduled_15,
    output reg valid_15,
    output reg [5:0] age_15
    );

// Signal Declaration
reg [68:0] unscheduled_buffer [0:15];
reg valid_bit [0:15];
reg [8:0] age_bits [0:15];
reg [4:0] count;
reg [1:0] next_cpu;
reg [3:0] ptr_write;
reg [3:0] ptr_completed;
wire buf0_read;
wire buf1_read;
wire buf2_read;
wire buf3_read;
wire [3:0] buf0_count;
wire [3:0] buf1_count;
wire [3:0] buf2_count;
wire [3:0] buf3_count;
wire [66:0] cpu0_out;
wire [66:0] cpu1_out;
wire [66:0] cpu2_out;
wire [66:0] cpu3_out;
wire write = ((buf0_count + buf1_count + buf2_count + buf3_count)>0)? 1'b1:1'b0;
wire full;
integer i;
integer j;
integer k;
integer x;
integer y;

// Component Instantiation
cpu_buffer CPU0( // Takes requests in from CPU 0
    .cpu_data (cpu0_req),
    .cpu_req (cpu0_req_ctrl),
    .clk (clk),
    .read (buf0_read),
    .reset (reset),
    .out_data (cpu0_out),
    .empty (buf0_empty),
    .full (buf0_full),
    .count (buf0_count)
);

cpu_buffer CPU1( // Takes requests in from CPU 1
    .cpu_data (cpu1_req),
    .cpu_req (cpu1_req_ctrl),
    .clk (clk),
    .read (buf1_read),
    .reset (reset),
    .out_data (cpu1_out),
    .empty (buf1_empty),
    .full (buf1_full),
    .count (buf1_count)
);

cpu_buffer CPU2( // Takes requests in from CPU 2
    .cpu_data (cpu2_req),
    .cpu_req (cpu2_req_ctrl),
    .clk (clk),
    .read (buf2_read),
    .reset (reset),
    .out_data (cpu2_out),
    .empty (buf2_empty),
    .full (buf2_full),
    .count (buf2_count)
);

cpu_buffer CPU3( // Takes requests in from CPU 3
    .cpu_data (cpu3_req),
    .cpu_req (cpu3_req_ctrl),
    .clk (clk),
    .read (buf3_read),
    .reset (reset),
    .out_data (cpu3_out),
    .empty (buf3_empty),
    .full (buf3_full),
    .count (buf3_count)
);

// Logic
assign full = (count==5'b10000)? 1'b1:1'b0;

always @(*)
begin
    // Determines which CPU buffer to take the next FIFO entry from
    // next_cpu is the most full CPU buffer
    if ((buf3_count>buf2_count) & (buf3_count>buf1_count) & (buf3_count>buf0_count)) begin
        next_cpu = 2'b11;
    end
    else if ((buf2_count>buf1_count) & (buf2_count>buf0_count)) begin
        next_cpu = 2'b10;
    end
    else if (buf1_count>buf0_count) begin
        next_cpu = 2'b01;
    end
    else next_cpu = 2'b00;
    
    // Determines how many entries are currently occupied
    // If the corresponding valid bit is high, the entry is occupied
    count = 5'b10000;
    for (i=0; i<16; i=i+1)
        if (valid_bit[i]==1'b0) begin
            count = count - 1'b1;
        end
    
    // Determines which buffer location should be written to next
    // Finds the first non-occupied entry
    ptr_write = 4'b0000;
    for (j=15; j>-1; j=j-1)
        if (valid_bit[j]==1'b0) begin
            ptr_write = j;
        end
    
    // Determines which entry has been completed, if any
    // Whenever a completed request is received, it is automatically removed using this pointer
    for (k=0; k<16; k=k+1)
        if ({unscheduled_buffer[k][68:67], unscheduled_buffer[k][64:32]}==completed_entry[66:32]) begin
            ptr_completed = k;
        end
        else ptr_completed = 4'b0000;
end

// Determines which CPU buffer read signal should be active
// If the request tracker is full, no CPU buffer is read from
assign buf0_read = write & (next_cpu==2'b00) & ~full;
assign buf1_read = write & (next_cpu==2'b01) & ~full;
assign buf2_read = write & (next_cpu==2'b10) & ~full;
assign buf3_read = write & (next_cpu==2'b11) & ~full;

// Unscheduled Buffer
always @(posedge clk)
begin
    if (reset) begin
        for (x=0; x<16; x=x+1) valid_bit[x] = 1'b0;
    end
    // Removes completed entries
    else if (completed_ctrl) begin
        valid_bit[ptr_completed] = 1'b0;
    end
    // Reads new entries in
    else if (buf0_read) begin
        unscheduled_buffer[ptr_write] = {2'b00, cpu0_out};
        valid_bit[ptr_write] = 1'b1;
        age_bits[ptr_write] = 9'b000000000;
    end
    else if (buf1_read) begin
        unscheduled_buffer[ptr_write] = {2'b01, cpu1_out};
        valid_bit[ptr_write] = 1'b1;
        age_bits[ptr_write] = 9'b000000000;
    end
    else if (buf2_read) begin
        unscheduled_buffer[ptr_write] = {2'b10, cpu2_out};
        valid_bit[ptr_write] = 1'b1;
        age_bits[ptr_write] = 9'b000000000;
    end
    else if (buf3_read) begin
        unscheduled_buffer[ptr_write] = {2'b11, cpu3_out};
        valid_bit[ptr_write] = 1'b1;
        age_bits[ptr_write] = 9'b000000000;
    end
    // Increments the request age
    else if (1'b1) begin
        for (y=0; y<16; y=y+1)
            age_bits[y] = age_bits[y] + 1'b1;
    end
end

// Outputs the desired buffer contents to be received by the schedule algorithm and sorter
always @(*)
begin
    unscheduled_0 = unscheduled_buffer[0];
    valid_0 = valid_bit[0];
    age_0 = age_bits[0][8:3];
    unscheduled_1 = unscheduled_buffer[1];
    valid_1 = valid_bit[1];
    age_1 = age_bits[1][8:3];
    unscheduled_2 = unscheduled_buffer[2];
    valid_2 = valid_bit[2];
    age_2 = age_bits[2][8:3];
    unscheduled_3 = unscheduled_buffer[3];
    valid_3 = valid_bit[3];
    age_3 = age_bits[3][8:3];
    unscheduled_4 = unscheduled_buffer[4];
    valid_4 = valid_bit[4];
    age_4 = age_bits[4][8:3];
    unscheduled_5 = unscheduled_buffer[5];
    valid_5 = valid_bit[5];
    age_5 = age_bits[5][8:3];
    unscheduled_6 = unscheduled_buffer[6];
    valid_6 = valid_bit[6];
    age_6 = age_bits[6][8:3];
    unscheduled_7 = unscheduled_buffer[7];
    valid_7 = valid_bit[7];
    age_7 = age_bits[7][8:3];
    unscheduled_8 = unscheduled_buffer[8];
    valid_8 = valid_bit[8];
    age_8 = age_bits[8][8:3];
    unscheduled_9 = unscheduled_buffer[9];
    valid_9 = valid_bit[9];
    age_9 = age_bits[9][8:3];
    unscheduled_10 = unscheduled_buffer[10];
    valid_10 = valid_bit[10];
    age_10 = age_bits[10][8:3];
    unscheduled_11 = unscheduled_buffer[11];
    valid_11 = valid_bit[11];
    age_11 = age_bits[11][8:3];
    unscheduled_12 = unscheduled_buffer[12];
    valid_12 = valid_bit[12];
    age_12 = age_bits[12][8:3];
    unscheduled_13 = unscheduled_buffer[13];
    valid_13 = valid_bit[13];
    age_13 = age_bits[13][8:3];
    unscheduled_14 = unscheduled_buffer[14];
    valid_14 = valid_bit[14];
    age_14 = age_bits[14][8:3];
    unscheduled_15 = unscheduled_buffer[15];
    valid_15 = valid_bit[15];
    age_15 = age_bits[15][8:3];
end

endmodule
