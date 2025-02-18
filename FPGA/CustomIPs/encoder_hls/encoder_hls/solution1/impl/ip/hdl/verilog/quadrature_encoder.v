// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and OpenCL
// Version: 2020.1
// Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="quadrature_encoder,hls_ip_2020_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xc7z020-clg400-1,HLS_INPUT_CLOCK=20.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=6.806000,HLS_SYN_LAT=1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=113,HLS_SYN_LUT=193,HLS_VERSION=2020_1}" *)

module quadrature_encoder (
        ap_clk,
        ap_rst_n,
        A,
        B,
        s_axi_ENCODER_AXI_AWVALID,
        s_axi_ENCODER_AXI_AWREADY,
        s_axi_ENCODER_AXI_AWADDR,
        s_axi_ENCODER_AXI_WVALID,
        s_axi_ENCODER_AXI_WREADY,
        s_axi_ENCODER_AXI_WDATA,
        s_axi_ENCODER_AXI_WSTRB,
        s_axi_ENCODER_AXI_ARVALID,
        s_axi_ENCODER_AXI_ARREADY,
        s_axi_ENCODER_AXI_ARADDR,
        s_axi_ENCODER_AXI_RVALID,
        s_axi_ENCODER_AXI_RREADY,
        s_axi_ENCODER_AXI_RDATA,
        s_axi_ENCODER_AXI_RRESP,
        s_axi_ENCODER_AXI_BVALID,
        s_axi_ENCODER_AXI_BREADY,
        s_axi_ENCODER_AXI_BRESP
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_state2 = 2'd2;
parameter    C_S_AXI_ENCODER_AXI_DATA_WIDTH = 32;
parameter    C_S_AXI_ENCODER_AXI_ADDR_WIDTH = 5;
parameter    C_S_AXI_DATA_WIDTH = 32;

parameter C_S_AXI_ENCODER_AXI_WSTRB_WIDTH = (32 / 8);
parameter C_S_AXI_WSTRB_WIDTH = (32 / 8);

input   ap_clk;
input   ap_rst_n;
input   A;
input   B;
input   s_axi_ENCODER_AXI_AWVALID;
output   s_axi_ENCODER_AXI_AWREADY;
input  [C_S_AXI_ENCODER_AXI_ADDR_WIDTH - 1:0] s_axi_ENCODER_AXI_AWADDR;
input   s_axi_ENCODER_AXI_WVALID;
output   s_axi_ENCODER_AXI_WREADY;
input  [C_S_AXI_ENCODER_AXI_DATA_WIDTH - 1:0] s_axi_ENCODER_AXI_WDATA;
input  [C_S_AXI_ENCODER_AXI_WSTRB_WIDTH - 1:0] s_axi_ENCODER_AXI_WSTRB;
input   s_axi_ENCODER_AXI_ARVALID;
output   s_axi_ENCODER_AXI_ARREADY;
input  [C_S_AXI_ENCODER_AXI_ADDR_WIDTH - 1:0] s_axi_ENCODER_AXI_ARADDR;
output   s_axi_ENCODER_AXI_RVALID;
input   s_axi_ENCODER_AXI_RREADY;
output  [C_S_AXI_ENCODER_AXI_DATA_WIDTH - 1:0] s_axi_ENCODER_AXI_RDATA;
output  [1:0] s_axi_ENCODER_AXI_RRESP;
output   s_axi_ENCODER_AXI_BVALID;
input   s_axi_ENCODER_AXI_BREADY;
output  [1:0] s_axi_ENCODER_AXI_BRESP;

 reg    ap_rst_n_inv;
wire    reset;
wire   [31:0] count;
reg    count_ap_vld;
reg   [31:0] curr_count;
reg   [0:0] lastA;
reg   [0:0] lastB;
wire   [3:0] quad_encoding_table_address0;
reg    quad_encoding_table_ce0;
wire   [1:0] quad_encoding_table_q0;
(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
wire   [63:0] zext_ln41_fu_104_p1;
wire   [31:0] add_ln41_fu_137_p2;
wire    ap_CS_fsm_state2;
wire   [0:0] tmp_1_fu_92_p2;
wire   [0:0] tmp_1_fu_92_p4;
wire   [3:0] tmp_1_fu_92_p5;
wire   [0:0] select_ln36_fu_125_p0;
wire  signed [31:0] sext_ln41_fu_133_p1;
wire   [31:0] select_ln36_fu_125_p3;
reg   [1:0] ap_NS_fsm;

// power-on initialization
initial begin
#0 curr_count = 32'd0;
#0 lastA = 1'd0;
#0 lastB = 1'd0;
#0 ap_CS_fsm = 2'd1;
end

quadrature_encodebkb #(
    .DataWidth( 2 ),
    .AddressRange( 16 ),
    .AddressWidth( 4 ))
quad_encoding_table_U(
    .clk(ap_clk),
    .reset(ap_rst_n_inv),
    .address0(quad_encoding_table_address0),
    .ce0(quad_encoding_table_ce0),
    .q0(quad_encoding_table_q0)
);

quadrature_encoder_ENCODER_AXI_s_axi #(
    .C_S_AXI_ADDR_WIDTH( C_S_AXI_ENCODER_AXI_ADDR_WIDTH ),
    .C_S_AXI_DATA_WIDTH( C_S_AXI_ENCODER_AXI_DATA_WIDTH ))
quadrature_encoder_ENCODER_AXI_s_axi_U(
    .AWVALID(s_axi_ENCODER_AXI_AWVALID),
    .AWREADY(s_axi_ENCODER_AXI_AWREADY),
    .AWADDR(s_axi_ENCODER_AXI_AWADDR),
    .WVALID(s_axi_ENCODER_AXI_WVALID),
    .WREADY(s_axi_ENCODER_AXI_WREADY),
    .WDATA(s_axi_ENCODER_AXI_WDATA),
    .WSTRB(s_axi_ENCODER_AXI_WSTRB),
    .ARVALID(s_axi_ENCODER_AXI_ARVALID),
    .ARREADY(s_axi_ENCODER_AXI_ARREADY),
    .ARADDR(s_axi_ENCODER_AXI_ARADDR),
    .RVALID(s_axi_ENCODER_AXI_RVALID),
    .RREADY(s_axi_ENCODER_AXI_RREADY),
    .RDATA(s_axi_ENCODER_AXI_RDATA),
    .RRESP(s_axi_ENCODER_AXI_RRESP),
    .BVALID(s_axi_ENCODER_AXI_BVALID),
    .BREADY(s_axi_ENCODER_AXI_BREADY),
    .BRESP(s_axi_ENCODER_AXI_BRESP),
    .ACLK(ap_clk),
    .ARESET(ap_rst_n_inv),
    .ACLK_EN(1'b1),
    .reset(reset),
    .count(count),
    .count_ap_vld(count_ap_vld)
);

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        curr_count <= add_ln41_fu_137_p2;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state1)) begin
        lastA <= A;
        lastB <= B;
    end
end

always @ (*) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        count_ap_vld = 1'b1;
    end else begin
        count_ap_vld = 1'b0;
    end
end

always @ (*) begin
    if ((1'b1 == ap_CS_fsm_state1)) begin
        quad_encoding_table_ce0 = 1'b1;
    end else begin
        quad_encoding_table_ce0 = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            ap_NS_fsm = ap_ST_fsm_state2;
        end
        ap_ST_fsm_state2 : begin
            ap_NS_fsm = ap_ST_fsm_state1;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign add_ln41_fu_137_p2 = ($signed(sext_ln41_fu_133_p1) + $signed(select_ln36_fu_125_p3));

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_CS_fsm_state2 = ap_CS_fsm[32'd1];

always @ (*) begin
    ap_rst_n_inv = ~ap_rst_n;
end

assign count = ($signed(sext_ln41_fu_133_p1) + $signed(select_ln36_fu_125_p3));

assign quad_encoding_table_address0 = zext_ln41_fu_104_p1;

assign select_ln36_fu_125_p0 = reset;

assign select_ln36_fu_125_p3 = ((select_ln36_fu_125_p0[0:0] === 1'b1) ? 32'd0 : curr_count);

assign sext_ln41_fu_133_p1 = $signed(quad_encoding_table_q0);

assign tmp_1_fu_92_p2 = A;

assign tmp_1_fu_92_p4 = B;

assign tmp_1_fu_92_p5 = {{{{lastA}, {tmp_1_fu_92_p2}}, {lastB}}, {tmp_1_fu_92_p4}};

assign zext_ln41_fu_104_p1 = tmp_1_fu_92_p5;

endmodule //quadrature_encoder
