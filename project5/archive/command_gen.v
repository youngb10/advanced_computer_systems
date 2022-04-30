`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: command_gen
// Project Name: Memory_Controller
//
// Description: 
// Generates the commands interpreted by the DRAM. 
// 
// Dependencies:
// refresh_tracker
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module command_gen(
    input [67:0] request_in,
    input [15:0] next_row,
    input reset,
    input clk,
    input [31:0] DRAM_data_rd,
    output [66:0] completed_entry,
    output reg completed_ctrl,
    output reg ext_load,
    output reg inc,
    output reg CS_L,
    output reg RAS_L,
    output reg CAS_L,
    output reg WE_L,
    output reg [15:0] addr,
    output reg [1:0] addr_bank,
    output [31:0] DRAM_data_wr
    );

// Signal Declaration
wire [15:0] addr_row;
wire [10:0] addr_col;
reg [31:0] data_out;
reg [15:0] step_count;
reg [1:0] state;
reg [1:0] state_next;
reg count_reset;
reg refreshed;
wire refresh_queued;
wire wr_L;

// Component Instantiation
refresh_tracker REFRESH(
    .clk (clk),
    .reset (reset),
    .refreshed (refreshed),
    .refresh (refresh_queued)
);

// Utilizes the address mapping scheme to determine where the physical address will point to
assign addr_row = request_in[63:48];
assign addr_col = request_in[47:37];

// Determines if a read or write will occur
assign wr_L = ~request_in[64];

// Determines what the output data will be
always @(*)
begin
    if (request_in[64]) begin
        data_out = request_in[31:0];
    end
    else data_out = DRAM_data_rd;
end

assign DRAM_data_wr = request_in[31:0];

// Determines what the completed entry may be
assign completed_entry = {request_in[66:32], data_out};

// Command Generation FSM - Update state
always @(posedge clk)
begin
    state = state_next;
    if (count_reset==1'b1) begin
        step_count = 16'b0000000000000000;
    end
    else step_count = step_count + 1'b1;
end

// Command Generation FSM - Next state logic and completed entry logic
always @(*)
begin
    state_next = state; // Next state is the same as the current state by default
    count_reset = 1'b0; // Does not reset the count by default
    inc = 1'b0;         // Does not move to the next
    ext_load = 1'b0;    // Scheduled buffer load is off by default
    refreshed = 1'b0;   // Refreshed is off by default
    completed_ctrl = 1'b0; // There are no completed entries by default
    case(state)
        2'b00: // Idle
            if (refresh_queued) begin
                state_next = 2'b11; // If a refresh is queued, go to the refresh state
                count_reset = 1'b1;
            end
            else if (request_in[67]==1'b1 & addr_row==next_row) begin
                state_next = 2'b01; // If a request is here and the next row is the same, there will be same row access
                count_reset = 1'b1;
            end
            else if (request_in[67]==1'b1 & addr_row!=next_row) begin
                state_next = 2'b10; // If a request is here and the next row is not the same, there will not be same row access
                count_reset = 1'b1;
            end
            else if (step_count[4:0] > 5'b11111) begin
                ext_load = 1'b1; // If no request is available, make sure the scheduled buffer loads new content
                count_reset = 1'b1;
            end
        2'b01: // Access Active, Next Row Same
            if (step_count[4:0] > 5'b10000) begin // When on step 17, return to idle next cycle
                state_next = 2'b00; // Returns to idle
                count_reset = 1'b1;
                inc = 1'b1;
                completed_ctrl = 1'b1; // An entry has been completed
            end
        2'b10:
            if (step_count[4:0] > 5'b10010) begin // When on step 25, return to idle next cycle
                state_next = 2'b00; // Returns to idle
                count_reset = 1'b1;
                inc = 1'b1;
                completed_ctrl = 1'b1; // An entry has been completed
            end
        2'b11:
            if (step_count==16'b1111111111111111) begin
                state_next = 2'b00; // Returns to idle
                count_reset = 1'b1;
                ext_load = 1'b1;
                refreshed = 1'b1;
            end
    endcase
end

// Determines the output based on the state
always @(*)
begin
    case(state)
        2'b00: begin
            {CS_L, CAS_L, RAS_L, WE_L} = {1'b1, 1'b1, 1'b1, 1'b1}; // Chip inactive
            addr = addr_row;
            addr_bank = 3'b00;
        end
        2'b01: begin
            if (step_count[4:0]==5'b00000) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 0
                addr = addr_row;   // Row address is used for activation
                addr_bank = 2'b00; // Bank 0 first
            end
            else if (step_count[4:0]==5'b00010) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 0
                addr = {5'b00000, addr_col}; // Column address is used for commands
                addr_bank = 2'b00;           // Bank 0 first
            end
            else if (step_count[4:0]==5'b00100) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 1
                addr = addr_row;
                addr_bank = 2'b01;
            end
            else if (step_count[4:0]==5'b00110) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 1
                addr = {5'b00000, addr_col};
                addr_bank = 2'b01;
            end
            else if (step_count[4:0]==5'b01000) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 2
                addr = addr_row;
                addr_bank = 2'b10;
            end
            else if (step_count[4:0]==5'b01010) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 2
                addr = {5'b00000, addr_col};
                addr_bank = 2'b10;
            end
            else if (step_count[4:0]==5'b01100) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 3
                addr = addr_row;
                addr_bank = 2'b11;
            end
            else if (step_count[4:0]==5'b01110) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 3
                addr = {5'b00000, addr_col};
                addr_bank = 2'b11;
            end
            else begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b1, 1'b1}; // nop
                addr = addr_row;
                addr_bank = 2'b00;
            end
        end
        2'b10: begin
            if (step_count[4:0]==5'b00000) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 0
                addr = addr_row;   // Row address is used for activation
                addr_bank = 2'b00; // Bank 0 first
            end
            else if (step_count[4:0]==5'b00010) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 0
                addr = {5'b00000, addr_col}; // Column address is used for commands
                addr_bank = 2'b00;           // Bank 0 first
            end
            else if (step_count[4:0]==5'b00100) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 1
                addr = addr_row;
                addr_bank = 2'b01;
            end
            else if (step_count[4:0]==5'b00110) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 1
                addr = {5'b00000, addr_col};
                addr_bank = 2'b01;
            end
            else if (step_count[4:0]==5'b01000) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 2
                addr = addr_row;
                addr_bank = 2'b10;
            end
            else if (step_count[4:0]==5'b01010) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 2
                addr = {5'b00000, addr_col};
                addr_bank = 2'b10;
            end
            else if (step_count[4:0]==5'b01100) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b1}; // Activate Bank 3
                addr = addr_row;
                addr_bank = 2'b11;
            end
            else if (step_count[4:0]==5'b01110) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b0, wr_L}; // Command Bank 3
                addr = {5'b00000, addr_col};
                addr_bank = 2'b11;
            end
            else if (step_count[4:0]==5'b01111) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b0}; // Precharge Bank 0
                addr = {5'b00000, addr_col};
                addr_bank = 2'b00;
            end
            else if (step_count[4:0]==5'b10000) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b0}; // Precharge Bank 1
                addr = {5'b00000, addr_col};
                addr_bank = 2'b01;
            end
            else if (step_count[4:0]==5'b10001) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b0}; // Precharge Bank 2
                addr = {5'b00000, addr_col};
                addr_bank = 2'b10;
            end
            else if (step_count[4:0]==5'b10010) begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b1, 1'b0}; // Precharge Bank 3
                addr = {5'b00000, addr_col};
                addr_bank = 2'b11;
            end
            else begin
                {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b1, 1'b1, 1'b1}; // nop
                addr = addr_row;
                addr_bank = 2'b00;
            end
        end
        2'b11: begin
            {CS_L, CAS_L, RAS_L, WE_L} = {1'b0, 1'b0, 1'b0, 1'b1}; // Refresh
            addr = step_count; // Iterates through every row in 0.6ms
            addr_bank = 2'b00; // Don't care for refresh
        end
    endcase
end

endmodule
