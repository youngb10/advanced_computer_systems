`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: sort_sixteen
// Project Name: Memory_Controller
//
// Description: 
// Sorts sixteen elements using eight-element sorters.
// 
// Dependencies:
// sort_eight
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module sort_sixteen(
    input [75:0] in0,
    input [75:0] in1,
    input [75:0] in2,
    input [75:0] in3,
    input [75:0] in4,
    input [75:0] in5,
    input [75:0] in6,
    input [75:0] in7,
    input [75:0] in8,
    input [75:0] in9,
    input [75:0] in10,
    input [75:0] in11,
    input [75:0] in12,
    input [75:0] in13,
    input [75:0] in14,
    input [75:0] in15,
    output reg [67:0] out0,
    output reg [67:0] out1,
    output reg [67:0] out2,
    output reg [67:0] out3,
    output reg [67:0] out4,
    output reg [67:0] out5,
    output reg [67:0] out6,
    output reg [67:0] out7,
    output reg [67:0] out8,
    output reg [67:0] out9,
    output reg [67:0] out10,
    output reg [67:0] out11,
    output reg [67:0] out12,
    output reg [67:0] out13,
    output reg [67:0] out14,
    output reg [67:0] out15
    );

// Sort Signal Declaration
wire [11:0] in0_sort = {in0[75:68], 4'b0000}; // Only the original location of each output element will be kept
wire [11:0] in1_sort = {in1[75:68], 4'b0001}; //  track of during the sort to greatly reduce hardware utilization.
wire [11:0] in2_sort = {in2[75:68], 4'b0010}; //  When the sort is complete, the inputs will be matched to the
wire [11:0] in3_sort = {in3[75:68], 4'b0011}; //  outputs based on these markers. 
wire [11:0] in4_sort = {in4[75:68], 4'b0100};
wire [11:0] in5_sort = {in5[75:68], 4'b0101};
wire [11:0] in6_sort = {in6[75:68], 4'b0110};
wire [11:0] in7_sort = {in7[75:68], 4'b0111};
wire [11:0] in8_sort = {in8[75:68], 4'b1000};
wire [11:0] in9_sort = {in9[75:68], 4'b1001};
wire [11:0] in10_sort = {in10[75:68], 4'b1010};
wire [11:0] in11_sort = {in11[75:68], 4'b1011};
wire [11:0] in12_sort = {in12[75:68], 4'b1100};
wire [11:0] in13_sort = {in13[75:68], 4'b1101};
wire [11:0] in14_sort = {in14[75:68], 4'b1110};
wire [11:0] in15_sort = {in15[75:68], 4'b1111};
wire [11:0] out0_sort;
wire [11:0] out1_sort;
wire [11:0] out2_sort;
wire [11:0] out3_sort;
wire [11:0] out4_sort;
wire [11:0] out5_sort;
wire [11:0] out6_sort;
wire [11:0] out7_sort;
wire [11:0] out8_sort;
wire [11:0] out9_sort;
wire [11:0] out10_sort;
wire [11:0] out11_sort;
wire [11:0] out12_sort;
wire [11:0] out13_sort;
wire [11:0] out14_sort;
wire [11:0] out15_sort;
reg [67:0] IN_TEMP [0:15];
reg [67:0] OUT_TEMP [0:15];
reg [11:0] out_sort [0:15];
integer i;

// Intermediate Signal Declaration
wire [11:0] int1_0;
wire [11:0] int1_1;
wire [11:0] int1_2;
wire [11:0] int1_3;
wire [11:0] int1_4;
wire [11:0] int1_5;
wire [11:0] int1_6;
wire [11:0] int1_7;
wire [11:0] int2_0;
wire [11:0] int2_1;
wire [11:0] int2_2;
wire [11:0] int2_3;
wire [11:0] int2_4;
wire [11:0] int2_5;
wire [11:0] int2_6;
wire [11:0] int2_7;
wire [11:0] int3_0;
wire [11:0] int3_1;
wire [11:0] int3_2;
wire [11:0] int3_3;
wire [11:0] int3_4;
wire [11:0] int3_5;
wire [11:0] int3_6;
wire [11:0] int3_7;

// Component Instantiation
sort_eight SORT8_1( // Sorts eight elements
.in0 (in0_sort),
.in1 (in1_sort),
.in2 (in2_sort),
.in3 (in3_sort),
.in4 (in4_sort),
.in5 (in5_sort),
.in6 (in6_sort),
.in7 (in7_sort),
.out0 (int1_0),
.out1 (int1_1),
.out2 (int1_2),
.out3 (int1_3),
.out4 (int1_4),
.out5 (int1_5),
.out6 (int1_6),
.out7 (int1_7)
);

sort_eight SORT8_2( // Sorts eight elements
.in0 (in8_sort),
.in1 (in9_sort),
.in2 (in10_sort),
.in3 (in11_sort),
.in4 (in12_sort),
.in5 (in13_sort),
.in6 (in14_sort),
.in7 (in15_sort),
.out0 (int2_0),
.out1 (int2_1),
.out2 (int2_2),
.out3 (int2_3),
.out4 (int2_4),
.out5 (int2_5),
.out6 (int2_6),
.out7 (int2_7)
);

sort_eight SORT8_3( // Sorts eight elements
.in0 (int1_0),
.in1 (int1_1),
.in2 (int1_2),
.in3 (int1_3),
.in4 (int2_0),
.in5 (int2_1),
.in6 (int2_2),
.in7 (int2_3),
.out0 (out0_sort),
.out1 (out1_sort),
.out2 (out2_sort),
.out3 (out3_sort),
.out4 (int3_0),
.out5 (int3_1),
.out6 (int3_2),
.out7 (int3_3)
);

sort_eight SORT8_4( // Sorts eight elements
.in0 (int1_4),
.in1 (int1_5),
.in2 (int1_6),
.in3 (int1_7),
.in4 (int2_4),
.in5 (int2_5),
.in6 (int2_6),
.in7 (int2_7),
.out0 (int3_4),
.out1 (int3_5),
.out2 (int3_6),
.out3 (int3_7),
.out4 (out12_sort),
.out5 (out13_sort),
.out6 (out14_sort),
.out7 (out15_sort)
);

sort_eight SORT8_5( // Sorts eight elements
.in0 (int3_0),
.in1 (int3_1),
.in2 (int3_2),
.in3 (int3_3),
.in4 (int3_4),
.in5 (int3_5),
.in6 (int3_6),
.in7 (int3_7),
.out0 (out4_sort),
.out1 (out5_sort),
.out2 (out6_sort),
.out3 (out7_sort),
.out4 (out8_sort),
.out5 (out9_sort),
.out6 (out10_sort),
.out7 (out11_sort)
);

always @(*)
begin
    // Assigns inputs and outputs to temporary arrays so they may be indexed
    IN_TEMP[0] = in0[67:0];
    IN_TEMP[1] = in1[67:0];
    IN_TEMP[2] = in2[67:0];
    IN_TEMP[3] = in3[67:0];
    IN_TEMP[4] = in4[67:0];
    IN_TEMP[5] = in5[67:0];
    IN_TEMP[6] = in6[67:0];
    IN_TEMP[7] = in7[67:0];
    IN_TEMP[8] = in8[67:0];
    IN_TEMP[9] = in9[67:0];
    IN_TEMP[10] = in10[67:0];
    IN_TEMP[11] = in11[67:0];
    IN_TEMP[12] = in12[67:0];
    IN_TEMP[13] = in13[67:0];
    IN_TEMP[14] = in14[67:0];
    IN_TEMP[15] = in15[67:0];
    out_sort[0] = out0_sort;
    out_sort[1] = out1_sort;
    out_sort[2] = out2_sort;
    out_sort[3] = out3_sort;
    out_sort[4] = out4_sort;
    out_sort[5] = out5_sort;
    out_sort[6] = out6_sort;
    out_sort[7] = out7_sort;
    out_sort[8] = out8_sort;
    out_sort[9] = out9_sort;
    out_sort[10] = out10_sort;
    out_sort[11] = out11_sort;
    out_sort[12] = out12_sort;
    out_sort[13] = out13_sort;
    out_sort[14] = out14_sort;
    out_sort[15] = out15_sort;
    
    // For each sorted output, assign to it the input at the location specified by the smallest four bits
    for (i=0; i<16; i=i+1)
        OUT_TEMP[i] = IN_TEMP[out_sort[i][3:0]];
end

// Assigns the temporary array to the outputs
always @(*)
begin
    out0 = OUT_TEMP[4'b0000];
    out1 = OUT_TEMP[4'b0001];
    out2 = OUT_TEMP[4'b0010];
    out3 = OUT_TEMP[4'b0011];
    out4 = OUT_TEMP[4'b0100];
    out5 = OUT_TEMP[4'b0101];
    out6 = OUT_TEMP[4'b0110];
    out7 = OUT_TEMP[4'b0111];
    out8 = OUT_TEMP[4'b1000];
    out9 = OUT_TEMP[4'b1001];
    out10 = OUT_TEMP[4'b1010];
    out11 = OUT_TEMP[4'b1011];
    out12 = OUT_TEMP[4'b1100];
    out13 = OUT_TEMP[4'b1101];
    out14 = OUT_TEMP[4'b1110];
    out15 = OUT_TEMP[4'b1111];
end

endmodule
