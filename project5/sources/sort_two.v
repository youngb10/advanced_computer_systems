`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Engineers: Bennett Young, Allan Nesathurai
//
// Module Name: sort_two
// Project Name: Memory_Controller
//
// Description: 
// Sorts two elements.
// 
// Dependencies: 
// None
//////////////////////////////////////////////////////////////////////////////////

// Module Declaration
module sort_two(
    input [11:0] in0,
    input [11:0] in1,
    output reg [11:0] max,
    output reg [11:0] min
    );

// Signal Declaration
wire [7:0] sort0 = in0[11:4];
wire [7:0] sort1 = in1[11:4];
wire gt;

// Logic
assign gt = sort0 > sort1;

always @(*)
begin
    if (gt == 1'b1) begin
        max = in0;
        min = in1;
    end
    else begin
        max = in1;
        min = in0;
    end
end

endmodule
