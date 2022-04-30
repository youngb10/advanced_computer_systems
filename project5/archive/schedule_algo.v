`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: schedule_algo
// Project Name: Memory_Controller
//
// Description: 
// Determines the scheduling priority based on several request properties.
// 
// Dependencies:
// None
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module schedule_algo(
    input [2:0] unscheduled_0,
    input valid_0,
    input [5:0] age_0,
    input [2:0] unscheduled_1,
    input valid_1,
    input [5:0] age_1,
    input [2:0] unscheduled_2,
    input valid_2,
    input [5:0] age_2,
    input [2:0] unscheduled_3,
    input valid_3,
    input [5:0] age_3,
    input [2:0] unscheduled_4,
    input valid_4,
    input [5:0] age_4,
    input [2:0] unscheduled_5,
    input valid_5,
    input [5:0] age_5,
    input [2:0] unscheduled_6,
    input valid_6,
    input [5:0] age_6,
    input [2:0] unscheduled_7,
    input valid_7,
    input [5:0] age_7,
    input [2:0] unscheduled_8,
    input valid_8,
    input [5:0] age_8,
    input [2:0] unscheduled_9,
    input valid_9,
    input [5:0] age_9,
    input [2:0] unscheduled_10,
    input valid_10,
    input [5:0] age_10,
    input [2:0] unscheduled_11,
    input valid_11,
    input [5:0] age_11,
    input [2:0] unscheduled_12,
    input valid_12,
    input [5:0] age_12,
    input [2:0] unscheduled_13,
    input valid_13,
    input [5:0] age_13,
    input [2:0] unscheduled_14,
    input valid_14,
    input [5:0] age_14,
    input [2:0] unscheduled_15,
    input valid_15,
    input [5:0] age_15,
    output reg [7:0] schedule_0,
    output reg [7:0] schedule_1,
    output reg [7:0] schedule_2,
    output reg [7:0] schedule_3,
    output reg [7:0] schedule_4,
    output reg [7:0] schedule_5,
    output reg [7:0] schedule_6,
    output reg [7:0] schedule_7,
    output reg [7:0] schedule_8,
    output reg [7:0] schedule_9,
    output reg [7:0] schedule_10,
    output reg [7:0] schedule_11,
    output reg [7:0] schedule_12,
    output reg [7:0] schedule_13,
    output reg [7:0] schedule_14,
    output reg [7:0] schedule_15
    );

// Signal Declaration
reg [7:0] priority_0;
reg [7:0] priority_1;
reg [7:0] priority_2;
reg [7:0] priority_3;
reg [7:0] priority_4;
reg [7:0] priority_5;
reg [7:0] priority_6;
reg [7:0] priority_7;
reg [7:0] priority_8;
reg [7:0] priority_9;
reg [7:0] priority_10;
reg [7:0] priority_11;
reg [7:0] priority_12;
reg [7:0] priority_13;
reg [7:0] priority_14;
reg [7:0] priority_15;

// Correctly weighs the priorities for each request
always @(*)
begin
    case(unscheduled_0[2:1])
        2'b11   : priority_0 = 8'b10000000;
        2'b10   : priority_0 = 8'b00001000;
        2'b01   : priority_0 = 8'b00000010;
        default : priority_0 = 8'b00000000;
    endcase
    case(unscheduled_1[2:1])
        2'b11   : priority_1 = 8'b10000000;
        2'b10   : priority_1 = 8'b00001000;
        2'b01   : priority_1 = 8'b00000010;
        default : priority_1 = 8'b00000000;
    endcase
    case(unscheduled_2[2:1])
        2'b11   : priority_2 = 8'b10000000;
        2'b10   : priority_2 = 8'b00001000;
        2'b01   : priority_2 = 8'b00000010;
        default : priority_2 = 8'b00000000;
    endcase
    case(unscheduled_3[2:1])
        2'b11   : priority_3 = 8'b10000000;
        2'b10   : priority_3 = 8'b00001000;
        2'b01   : priority_3 = 8'b00000010;
        default : priority_3 = 8'b00000000;
    endcase
    case(unscheduled_4[2:1])
        2'b11   : priority_4 = 8'b10000000;
        2'b10   : priority_4 = 8'b00001000;
        2'b01   : priority_4 = 8'b00000010;
        default : priority_4 = 8'b00000000;
    endcase
    case(unscheduled_5[2:1])
        2'b11   : priority_5 = 8'b10000000;
        2'b10   : priority_5 = 8'b00001000;
        2'b01   : priority_5 = 8'b00000010;
        default : priority_5 = 8'b00000000;
    endcase
    case(unscheduled_6[2:1])
        2'b11   : priority_6 = 8'b10000000;
        2'b10   : priority_6 = 8'b00001000;
        2'b01   : priority_6 = 8'b00000010;
        default : priority_6 = 8'b00000000;
    endcase
    case(unscheduled_7[2:1])
        2'b11   : priority_7 = 8'b10000000;
        2'b10   : priority_7 = 8'b00001000;
        2'b01   : priority_7 = 8'b00000010;
        default : priority_7 = 8'b00000000;
    endcase
    case(unscheduled_8[2:1])
        2'b11   : priority_8 = 8'b10000000;
        2'b10   : priority_8 = 8'b00001000;
        2'b01   : priority_8 = 8'b00000010;
        default : priority_8 = 8'b00000000;
    endcase
    case(unscheduled_9[2:1])
        2'b11   : priority_9 = 8'b10000000;
        2'b10   : priority_9 = 8'b00001000;
        2'b01   : priority_9 = 8'b00000010;
        default : priority_9 = 8'b00000000;
    endcase
    case(unscheduled_10[2:1])
        2'b11   : priority_10 = 8'b10000000;
        2'b10   : priority_10 = 8'b00001000;
        2'b01   : priority_10 = 8'b00000010;
        default : priority_10 = 8'b00000000;
    endcase
    case(unscheduled_11[2:1])
        2'b11   : priority_11 = 8'b10000000;
        2'b10   : priority_11 = 8'b00001000;
        2'b01   : priority_11 = 8'b00000010;
        default : priority_11 = 8'b00000000;
    endcase
    case(unscheduled_12[2:1])
        2'b11   : priority_12 = 8'b10000000;
        2'b10   : priority_12 = 8'b00001000;
        2'b01   : priority_12 = 8'b00000010;
        default : priority_12 = 8'b00000000;
    endcase
    case(unscheduled_13[2:1])
        2'b11   : priority_13 = 8'b10000000;
        2'b10   : priority_13 = 8'b00001000;
        2'b01   : priority_13 = 8'b00000010;
        default : priority_13 = 8'b00000000;
    endcase
    case(unscheduled_14[2:1])
        2'b11   : priority_14 = 8'b10000000;
        2'b10   : priority_14 = 8'b00001000;
        2'b01   : priority_14 = 8'b00000010;
        default : priority_14 = 8'b00000000;
    endcase
    case(unscheduled_15[2:1])
        2'b11   : priority_15 = 8'b10000000;
        2'b10   : priority_15 = 8'b00001000;
        2'b01   : priority_15 = 8'b00000010;
        default : priority_15 = 8'b00000000;
    endcase
    
    // Assigns the final scheduling priority
    schedule_0 = valid_0 * (priority_0 + age_0 + (unscheduled_0[0]*8'b00000100));
    schedule_1 = valid_1 * (priority_1 + age_1 + (unscheduled_1[0]*8'b00000100));
    schedule_2 = valid_2 * (priority_2 + age_2 + (unscheduled_2[0]*8'b00000100));
    schedule_3 = valid_3 * (priority_3 + age_3 + (unscheduled_3[0]*8'b00000100));
    schedule_4 = valid_4 * (priority_4 + age_4 + (unscheduled_4[0]*8'b00000100));
    schedule_5 = valid_5 * (priority_5 + age_5 + (unscheduled_5[0]*8'b00000100));
    schedule_6 = valid_6 * (priority_6 + age_6 + (unscheduled_6[0]*8'b00000100));
    schedule_7 = valid_7 * (priority_7 + age_7 + (unscheduled_7[0]*8'b00000100));
    schedule_8 = valid_8 * (priority_8 + age_8 + (unscheduled_8[0]*8'b00000100));
    schedule_9 = valid_9 * (priority_9 + age_9 + (unscheduled_9[0]*8'b00000100));
    schedule_10 = valid_10 * (priority_10 + age_10 + (unscheduled_10[0]*8'b00000100));
    schedule_11 = valid_11 * (priority_11 + age_11 + (unscheduled_11[0]*8'b00000100));
    schedule_12 = valid_12 * (priority_12 + age_12 + (unscheduled_12[0]*8'b00000100));
    schedule_13 = valid_13 * (priority_13 + age_13 + (unscheduled_13[0]*8'b00000100));
    schedule_14 = valid_14 * (priority_14 + age_14 + (unscheduled_14[0]*8'b00000100));
    schedule_15 = valid_15 * (priority_15 + age_15 + (unscheduled_15[0]*8'b00000100));
    
end

endmodule
