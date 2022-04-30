`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: sort_eight
// Project Name: Memory_Controller
//
// Description: 
// Sorts eight elements using four-element sorters.
// 
// Dependencies:
// sort_four
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module sort_eight(
    input [11:0] in0,
    input [11:0] in1,
    input [11:0] in2,
    input [11:0] in3,
    input [11:0] in4,
    input [11:0] in5,
    input [11:0] in6,
    input [11:0] in7,
    output [11:0] out0,
    output [11:0] out1,
    output [11:0] out2,
    output [11:0] out3,
    output [11:0] out4,
    output [11:0] out5,
    output [11:0] out6,
    output [11:0] out7
    );

// Signal Declaration
wire [11:0] int1_0;
wire [11:0] int1_1;
wire [11:0] int1_2;
wire [11:0] int1_3;
wire [11:0] int2_0;
wire [11:0] int2_1;
wire [11:0] int2_2;
wire [11:0] int2_3;
wire [11:0] int3_0;
wire [11:0] int3_1;
wire [11:0] int3_2;
wire [11:0] int3_3;

// Component Instantiation
sort_four SORT4_1(
    .in0 (in0),
    .in1 (in1),
    .in2 (in2),
    .in3 (in3),
    .out0 (int1_0),
    .out1 (int1_1),
    .out2 (int1_2),
    .out3 (int1_3)
);

sort_four SORT4_2(
    .in0 (in4),
    .in1 (in5),
    .in2 (in6),
    .in3 (in7),
    .out0 (int2_0),
    .out1 (int2_1),
    .out2 (int2_2),
    .out3 (int2_3)
);

sort_four SORT4_3(
    .in0 (int1_0),
    .in1 (int1_1),
    .in2 (int2_2),
    .in3 (int2_3),
    .out0 (out0),
    .out1 (out1),
    .out2 (int3_0),
    .out3 (int3_1)
);

sort_four SORT4_4(
    .in0 (int1_2),
    .in1 (int1_3),
    .in2 (int2_2),
    .in3 (int2_3),
    .out0 (int3_2),
    .out1 (int3_3),
    .out2 (out6),
    .out3 (out7)
);

sort_four SORT4_5(
    .in0 (int3_0),
    .in1 (int3_1),
    .in2 (int3_2),
    .in3 (int3_3),
    .out0 (out2),
    .out1 (out3),
    .out2 (out4),
    .out3 (out5)
);

endmodule
