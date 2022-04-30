`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
// 
// Create Date: 04/20/2022 03:24:28 PM
//
// Module Name: sort_four
// Project Name: Memory_Controller
//
// Description: 
// Sorts four elements using two-element sorters.
// 
// Dependencies:
// sort_two
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module sort_four(
    input [11:0] in0,
    input [11:0] in1,
    input [11:0] in2,
    input [11:0] in3,
    output [11:0] out0,
    output [11:0] out1,
    output [11:0] out2,
    output [11:0] out3
    );

// Signal Declaration
wire [11:0] int1_0;
wire [11:0] int1_1;
wire [11:0] int2_0;
wire [11:0] int2_1;
wire [11:0] int3_0;
wire [11:0] int3_1;

// Component Instantiation
sort_two SORT2_1(
    .in0 (in0),
    .in1 (in1),
    .max (int1_0),
    .min (int1_1)
);

sort_two SORT2_2(
    .in0 (in2),
    .in1 (in3),
    .max (int2_0),
    .min (int2_1)
);

sort_two SORT2_3(
    .in0 (int1_0),
    .in1 (int2_0),
    .max (out0),
    .min (int3_0)
);

sort_two SORT2_4(
    .in0 (int1_1),
    .in1 (int2_1),
    .max (int3_1),
    .min (out3)
);

sort_two SORT2_5(
    .in0 (int3_0),
    .in1 (int3_1),
    .max (out1),
    .min (out2)
);

endmodule
