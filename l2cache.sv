/* cache design top level -- contains the cache controller and the cache datapath */
import lc3b_types::*;

module l2cache
(
    input clk,
	 input cache_line pmem_rdata,
	 input logic pmem_resp,
	 input lc3b_mem_wmask l2_mem_byte_enable,
	 input lc3b_word l2_mem_address,
	 input cache_line l2_mem_wdata, //cache_line size for l2
	 input l2_mem_read,
    input l2_mem_write,


    /* Memory signals */
    output l2_mem_resp,
    output cache_line l2_mem_rdata,  //cache line size for l2


	 output logic pmem_read,
	 output logic pmem_write,
	 output lc3b_word pmem_address,
	 output cache_line pmem_wdata
	 
);


/*internal signals*/

logic hit;
logic lru_out;
logic data0write;
logic data1write;
logic valid0write;
logic valid1write;
logic tag0write;
logic tag1write;
logic lru_in;
logic way0and_out;
logic lru_write;
logic dirtymux_out;
logic stbwritemux_sel;
logic rwmux_sel;
logic dirty0write;
logic dirty1write;
logic dirty0_in;
logic dirty1_in;
logic pmemmux_sel;


l2cache_control l2cache_control
(
	 .clk,
    .mem_address(l2_mem_address),
	 .mem_byte_enable(l2_mem_byte_enable),
	 .mem_write(l2_mem_write),
	 .mem_read(l2_mem_read),
	 .mem_wdata(l2_mem_wdata),
	 .lru_out(lru_out),
	 .lru_in(lru_in),
	 .way0and_out(way0and_out),
	 .lru_write(lru_write),
	 
     
     /* Datapath controls */
    .pmem_rdata(pmem_rdata),
	 .pmem_resp(pmem_resp),
	 .dirtymux_out(dirtymux_out),
	 .hit(hit),
	 
	 .pmemmux_sel(pmemmux_sel),
	 .rwmux_sel(rwmux_sel),
	 .stbwritemux_sel(stbwritemux_sel),
	 .mem_resp(l2_mem_resp),
    .pmem_read(pmem_read),
	 .data0write(data0write),
	 .data1write(data1write),
	 .tag0write(tag0write), 
	 .tag1write(tag1write),
	 .valid0write(valid0write),
	 .valid1write(valid1write),
	 .dirty0write(dirty0write),
	 .dirty1write(dirty1write),
	 .dirty0_in(dirty0_in),
	 .dirty1_in(dirty1_in),
    .pmem_write(pmem_write)
);

l2cache_datapath  l2cache_datapath
(
    .clk,
	 /*inputs from cpu*/
	 .mem_address(l2_mem_address),
	 .data0write(data0write),
	 .data1write(data1write),
	 .tag0write(tag0write),
	 .tag1write(tag1write),
	 .valid0write(valid0write),
	 .valid1write(valid1write),
	 .dirty0write(dirty0write),
	 .dirty1write(dirty1write),
	 .dirty0_in(dirty0_in),
	 .dirty1_in(dirty1_in),
	 .pmem_rdata(pmem_rdata),
	 .l2_mem_wdata(l2_mem_wdata),
	 .l2_mem_rdata(l2_mem_rdata),
	 .mem_byte_enable(l2_mem_byte_enable),
	 .lru_in(lru_in),
	 .lru_write(lru_write),
	 .way0and_out(way0and_out),
	 .rwmux_sel(rwmux_sel),
	 .stbwritemux_sel(stbwritemux_sel),
	 .pmemmux_sel(pmemmux_sel),

    .hit(hit),
	 .dirtymux_out(dirtymux_out),
	 .lru_out(lru_out),
	 .pmem_address(pmem_address),
	 .pmem_wdata(pmem_wdata)
);


endmodule : l2cache
