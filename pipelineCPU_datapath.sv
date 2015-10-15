/*the datapath that contains everything for the pipeline CPU*/
module pipelineCPU_datapath(
	input clk,
	input i_mem_resp
	
);

/*INTERNAL SIGNALS*/
lc3b_word pcmux_out;	
lc3b_word pc_out;
lc3b_word pc_plus2_out;
lc3b_word if_id_pc_out;
logic nor_gate_out;
logic load_if_id;
logic if_id_v_out;
lc3b_word if_id_ir_out;
lc3b_reg storemux_out;
lc3b_reg destmux_out;
lc3b_word sr1_out;
lc3b_word sr2_out;
lc3b_reg cc_out;
/*END INTERNAL SIGNALS*/


/*FETCH STAGE COMPONENTS*/
mux3 pcmux 
(
	.sel(pcmux_sel), //Need to bring this from control rom
	.a(pc_plus2_out),
	.b(),//MEM.TARGET need this signal from MEM STAGE
	.c(),//MEM.TRAP need this signal from MEM STAGE
	.f(pcmux_out)
);

register pc
(
	.clk,
	.load(load_pc),//NEED to bring this from control ROM
	.in(pcmux_out),
	.out(pc_out)
);

plus2 pc_plus2
(
	.in(pc_out),.sel(pcmux_sel), //Need to bring this from control rom
	.a(pc_plus2_out),
	.b(),//MEM.TARGET need this signal from MEM STAGE
	.c(),//MEM.TRAP need this signal from MEM STAGE
	.f(pcmux_out),
	.out(pc_plus2_out)
);

nor3input #(.width(1)) nor_gate
(
	.a(),//ID.branch_stall
	.b(),//mem_stall
	.c(),//DEP_stall
	.f(nor_gate_out)
);

and2input #(.width(1)) and_gate
(
	.a(i_mem_resp),
	.b(nor_gate_out),
	.f(load_if_id)
);


/*END FETCH STAGE COMPONENTS*/

/*FETCH-DECODE PIPE COMPONENTS*/
register if_id_pc
(
	.clk,
	.load(load_if_id),
	.in(pc_plus2_out),
	.out(if_id_pc_out)
);

register if_id_ir
(
	.clk,
	.load(load_if_id),
	.in(imem_rdata),
	.out(if_id_ir_out)
);

register #(.width(1)) if_id_v
(
	.clk,
	.load(load_if_id),
	.in(load_if_id),
	.out(if_id_v_out)
);

/*END FETCH-DECODE PIPE COMPONENTS*/

/*DECODE STAGE COMPONENTS*/
mux2 #(.width(3)) storemux
(
	.sel(storemux_sel), //Need to bring this from control rom
	.a(if_id_ir_out[2:0]),
	.b(if_id_ir_out[11:9]),
	.f(storemux_out)
);

mux2 #(.width(3)) destmux
(
	.sel(destmux_sel), //Need to bring this from control rom
	.a(if_id_ir_out[11:9]),
	.b(3'b111),
	.f(destmux_out)
);

regfile regfile
(
	.clk,
	.in(),//WB.regfile.data	
	.src_a(if_id_ir_out[8:6]),
	.src_b(storemux_out),
	.dest(),//WB.DRID
	.reg_a(sr1_out),
	.reg_b(sr2_out),
);

register #(.width(3)) cc
(
	.clk,
	.load(),//WB.loadCC	
	.in(),//WB.ccdata
	.out(cc_out)
);
//Still need dependency logic//
//Still need branch stall logic//
//Still need control store//


/*END DECODE STAGE COMPONENTS*/

/*DECODE-EXECUTE PIPE COMPONENTS*/

/*END DECODE-EXECUTE PIPE COMPONENTS*/


/*EXECUTE STAGE COMPONENTS*/
/*sext #(.width(5)) sext5
(
     .in(),
	 .out(sext5_out)
);

sext #(.width(6)) sext6
(
     .in(),
	 .out(sext6_out)
);

sext #(.width(9)) sext9
(
     .in(),
	 .out(sext9_out)
);

sext #(.width(11)) sext11
(
     .in(),
	 .out(sext11_out)
);
*/

/*END EXECUTE STAGE COMPONENTS*/

/*EXECUTE-MEMORY PIPE COMPONENTS*/

/*END EXECUTE-MEMORY PIPE COMPONENTS*/


/*MEMORY STAGE COMPONENTS*/

/*END MEMORY STAGE COMPONENTS*/

/*MEMORY-WRITEBACK PIPE COMPONENTS*/

/*END MEMORY-WRITEBACK PIPE COMPONENTS*/

/*WRITE BACK STAGE COMPONENTS*/

/*END WRITE BACK STAGE COMPONENTS*/


endmodule : pipelineCPU_datapath
