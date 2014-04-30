
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 07/23/2013 at 20:23
// Component: PDM_Comb_v1_0
module PDM_Comb_v1_0 (
	output reg    d0_update,
	output wire   data_ready,
	output reg    fifo_ov,
	output wire   idle_sts,
	input  wire   clock,
	input  wire   d0_done,
	input  wire   enable,
	input  wire   input_avail
);
	parameter ApplyNoShift = 0;
	parameter ShiftCount = 8;
	parameter ShiftDirection = 0;

//`#start body` -- edit after this line, do not edit this line

//        Your code goes here

/* Below signal definitions commented out to add to module definition
   if the verilog code is regenerated */
//output reg   d0_update,
//output wire  data_ready,
//output reg   fifo_ov,
//output wire  idle_sts,
//input wire   clock,
//input wire   d0_done,
//input wire   enable,
//input wire   input_avail

/* Definitions for Shift Direction */
localparam RIGHT_SHIFT = 2'd0;
localparam LEFT_SHIFT  = 2'd1;
localparam NO_SHIFT    = 2'd2;

/* State machine states */
localparam PDM_COMB_STATE_RESET		           = 3'd0;
localparam PDM_COMB_STATE_DATAPOLL			   = 3'd1;
localparam PDM_COMB_STATE_FIFOLOAD			   = 3'd2;
localparam PDM_COMB_STATE_LOOP		           = 3'd3;
localparam PDM_COMB_STATE_LASTBUTONE	       = 3'd4;
localparam PDM_COMB_STATE_LAST	               = 3'd5;
localparam PDM_COMB_STATE_OUTSIGN	           = 3'd6;
localparam PDM_COMB_STATE_SHIFT                = 3'd7;

localparam [3:0] SHIFT_COUNT_PERIOD = ShiftCount - 1;

reg [2:0] cs_addr;         /* Datapath state machine */

/* Compare equal signals from datapath */
wire [3:0] ce1;

/* counter based on comb filter order */
reg [1:0] comb_loop_count; 

reg[1:0] first_count;

reg right_shift_msb;

/* FIFO load signals. F0 is output FIFO,
 and F1 is used as a dynamic FIFO */
reg f0_load;
reg f1_load;

/* Registered version of "d0_done" signal */
reg d0_reg_updated;

/* Datapath FIFO block, bus status signals */
wire [3:0] f0_bus_stat;
wire [3:0] f0_blk_stat;

/* Registered version of FIFO block status signal */
reg out_fifo_full;

/* Take the MSB datapath FIFO status signal for Comb block status */
assign data_ready = f0_bus_stat[3];


/* Ask for new data when in polling state, and after the update of D0 reg and output FIFO */
assign idle_sts = ((cs_addr == PDM_COMB_STATE_DATAPOLL) && (~f0_load) && (d0_reg_updated))? 1'b1 : 1'b0;

wire reset      = ~enable;     /* used as a global reset for the component */

always @(posedge clock or posedge reset)
begin
	if(reset)
	begin		
		first_count <= 0;
	end
	else
	begin		
		/* Dynamic FIFO initialization counter */
		if(first_count == 2'd3)	first_count <= first_count;	
		else                    first_count <= first_count + 1;	
	end
end

/* State machine generation depending on whether shift is required at the comb output or not */
generate
	if(ApplyNoShift == 1'b1)
	begin
		/* Start of logic required in case of no shift */
		always @(posedge clock or posedge reset)
		begin
			if(reset) cs_addr <= PDM_COMB_STATE_RESET;
			else
			begin
				case (cs_addr)
				
					PDM_COMB_STATE_RESET:
						if(first_count == 2'd3)    cs_addr <= PDM_COMB_STATE_DATAPOLL;
						else                       cs_addr <= PDM_COMB_STATE_RESET;
						
					PDM_COMB_STATE_DATAPOLL:
						/* Start comb operation once data is ready from integrator */
						if(input_avail)	cs_addr <= PDM_COMB_STATE_FIFOLOAD;
						else			    cs_addr <= PDM_COMB_STATE_DATAPOLL;
					
					/* Preload F1 FIFO before comb operation */
					PDM_COMB_STATE_FIFOLOAD: 
						cs_addr <= PDM_COMB_STATE_LOOP;
					
					/* Do three of the five comb stages in this state */
					PDM_COMB_STATE_LOOP:
						if(comb_loop_count == 2'd2) cs_addr <= PDM_COMB_STATE_LASTBUTONE;
						else cs_addr <= PDM_COMB_STATE_LOOP;
					
					/* 4th stage comb */
					PDM_COMB_STATE_LASTBUTONE:
						cs_addr <= PDM_COMB_STATE_LAST;
					
					/* 5th stage comb */
					PDM_COMB_STATE_LAST:
						cs_addr <= PDM_COMB_STATE_OUTSIGN;
					
					/* Dummy state for F0 load */
					PDM_COMB_STATE_OUTSIGN:					
						cs_addr <= PDM_COMB_STATE_DATAPOLL;
						
					default: cs_addr <= PDM_COMB_STATE_RESET;
				
				endcase
			end
		end
		
		/* Load F0 output FIFO after completing 5 stages of the comb */
		always @(posedge clock or posedge reset)
		begin
			if(reset) f0_load <= 0;
			else
			begin	
				if(cs_addr == PDM_COMB_STATE_OUTSIGN) f0_load <= 1;
				else f0_load <= 0;
			end
		end
	end
	else
	begin
	/* Start of logic required in case of left or right shift */
	
		reg [3:0] shift_count_reg; /* Counter for tracking number of bit shifts at comb o/p */
		
		always @(posedge clock or posedge reset)
		begin
			if(reset) cs_addr <= PDM_COMB_STATE_RESET;
			else
			begin
				case (cs_addr)				

					PDM_COMB_STATE_RESET:
						if(first_count == 2'd3)    cs_addr <= PDM_COMB_STATE_DATAPOLL;
						else                       cs_addr <= PDM_COMB_STATE_RESET;
						
					PDM_COMB_STATE_DATAPOLL:
						/* Start comb operation once data is ready from integrator */
						if(input_avail)	cs_addr <= PDM_COMB_STATE_FIFOLOAD;
						else			    cs_addr <= PDM_COMB_STATE_DATAPOLL;
					
					/* Preload F1 FIFO before comb operation */
					PDM_COMB_STATE_FIFOLOAD: 
						cs_addr <= PDM_COMB_STATE_LOOP;
					
					/* Do three of the five comb stages in this state */
					PDM_COMB_STATE_LOOP:
						if(comb_loop_count == 2'd2) cs_addr <= PDM_COMB_STATE_LASTBUTONE;
						else cs_addr <= PDM_COMB_STATE_LOOP;
					
					/* 4th stage comb */
					PDM_COMB_STATE_LASTBUTONE:
						cs_addr <= PDM_COMB_STATE_LAST;
					
					/* 5th stage comb */
					PDM_COMB_STATE_LAST:
						cs_addr <= PDM_COMB_STATE_OUTSIGN;
						
					/* Output Sign detection */
					PDM_COMB_STATE_OUTSIGN:					
						cs_addr <= PDM_COMB_STATE_SHIFT;	
						
					PDM_COMB_STATE_SHIFT:
						/* Loop for required number of bit shifts */
						if(shift_count_reg == SHIFT_COUNT_PERIOD) cs_addr <= PDM_COMB_STATE_DATAPOLL;
						else cs_addr <= PDM_COMB_STATE_SHIFT;	
						
					default: cs_addr <= PDM_COMB_STATE_RESET;
				
				endcase
			end
		end
		
		/* Load F0 output FIFO after completing 5 stages of the comb 
		   and also the shifting operations */
		always @(posedge clock or posedge reset)
		begin
			if(reset) f0_load <= 0;
			else
			begin	
				if((cs_addr == PDM_COMB_STATE_SHIFT )&& (shift_count_reg == SHIFT_COUNT_PERIOD)) f0_load <= 1;
				else f0_load <= 0;
			end
		end
		
		/* Counter for tracking number of shifts done while in shift state */
		always @(posedge clock or posedge reset)
		begin
			if(reset) shift_count_reg <= 0;
			else
			begin
				if(cs_addr == PDM_COMB_STATE_SHIFT )   shift_count_reg <= shift_count_reg + 1;
				else    					           shift_count_reg <= 0;
			end
		end
		
	end
endgenerate

/* Sampling logic for D0 register update signal from external DMA + glue logic */
always @(posedge clock or posedge reset)
begin
	if(reset) d0_reg_updated <= 1; /* At startup, D0 reg is updated */
	else
	begin	
		if(d0_reg_updated == 1)
		begin
			/* Once set, clear the signal when the comb requires a D0 reg update */
			if(cs_addr == PDM_COMB_STATE_LASTBUTONE) d0_reg_updated <= 0;
			else d0_reg_updated <= d0_reg_updated;
		end
		else
		begin
			/* Keep on smapling the external signal when d0_reg_updated = 0  */
			d0_reg_updated <= d0_done;
		end
	end
end

always @(posedge clock or posedge reset)
begin
	if(reset)
	begin
		f1_load <= 0;
		d0_update <= 0;
		comb_loop_count <= 0;
		fifo_ov <= 0;
		out_fifo_full <= 0;
	end
	else
	begin	
		/* Dynamic FIFO loading for circularly moving the filter data */
		if((cs_addr == PDM_COMB_STATE_FIFOLOAD) || (cs_addr == PDM_COMB_STATE_LOOP) || (cs_addr == PDM_COMB_STATE_RESET)) f1_load <= 1;
		else f1_load <= 0;
		
		/* Signal that D0 register needs update after completing 5 stages of comb */
		if(cs_addr == PDM_COMB_STATE_OUTSIGN)  d0_update <= 1;
		else    					           d0_update <= 0;	
		
		/* Counter for first 3 stages of comb */
		if(cs_addr == PDM_COMB_STATE_LOOP) comb_loop_count <= comb_loop_count + 1;
		else    					       comb_loop_count <= 0;		
		
		out_fifo_full <= f0_blk_stat[3]; /* Sampled version of FIFO full status */
		
		fifo_ov <= out_fifo_full & f0_load; /* Overflow status generation */		
	end
end

/* Different Datapaths generated based on requirement of
   left shift or right shift. Shift direction is a static
   configuration in datapath */
generate
	if(ShiftDirection == LEFT_SHIFT)
	begin
		cy_psoc3_dp32 #(.d1_init_d(8'b00000000), 
.cy_dpconfig_a(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_b(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_c(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_d(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'h80, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_ENBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_1, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		)) dp32(
		        /*  input                   */  .reset(1'b0),
		        /*  input                   */  .clk(clock),
		        /*  input   [02:00]         */  .cs_addr(cs_addr),
		        /*  input                   */  .route_si(1'b0),
		        /*  input                   */  .route_ci(1'b0),
		        /*  input                   */  .f0_load(f0_load),
		        /*  input                   */  .f1_load(f1_load),
		        /*  input                   */  .d0_load(1'b0),
		        /*  input                   */  .d1_load(1'b0),
		        /*  output  [03:00]                  */  .ce0(),
		        /*  output  [03:00]                  */  .cl0(),
		        /*  output  [03:00]                  */  .z0(),
		        /*  output  [03:00]                  */  .ff0(),
		        /*  output  [03:00]                  */  .ce1(ce1),
		        /*  output  [03:00]                  */  .cl1(),
		        /*  output  [03:00]                  */  .z1(),
		        /*  output  [03:00]                  */  .ff1(),
		        /*  output  [03:00]                  */  .ov_msb(),
		        /*  output  [03:00]                  */  .co_msb(),
		        /*  output  [03:00]                  */  .cmsb(),
		        /*  output  [03:00]                  */  .so(),
		        /*  output  [03:00]                  */  .f0_bus_stat(f0_bus_stat),
		        /*  output  [03:00]                  */  .f0_blk_stat(f0_blk_stat),
		        /*  output  [03:00]                  */  .f1_bus_stat(),
		        /*  output  [03:00]                  */  .f1_blk_stat()
		);		
	end
	else
	begin
	
	always @(posedge clock or posedge reset)
	begin
		if(reset)
		begin		
			right_shift_msb <= 0;
		end
		else
		begin		
			
			if((cs_addr == PDM_COMB_STATE_OUTSIGN ) && (ce1[3] == 1'b1))	     right_shift_msb <= 0;	
			else if ((cs_addr == PDM_COMB_STATE_OUTSIGN ) && (ce1[3] == 1'b0))   right_shift_msb <= 1;
			else                                                                 right_shift_msb <= right_shift_msb;           
		end
	end	
	
		cy_psoc3_dp32 #(.d1_init_d(8'b00000000), 
.cy_dpconfig_a(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SR, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SR, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_b(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SR, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SR, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_c(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SR, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'hFF, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_CHAIN, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SR, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_d(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0: PDM_COMB_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1: PDM_COMB_STATE_DATAPOLL*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2: PDM_COMB_STATE_FIFOLOAD*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3: PDM_COMB_STATE_LOOP*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4: PDM_COMB_STATE_LASTBUTONE*/
		    `CS_ALU_OP__SUB, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5: PDM_COMB_STATE_LAST*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6: PDM_COMB_STATE_OUTSIGN*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP___SR, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7: PDM_COMB_STATE_SHIFT*/
		    8'hFF, 8'h00,  /*CFG9:          */
		    8'h80, 8'hFF,  /*CFG11-10:          */
		    `SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_CHAIN,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_ENBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_1, `SC_SI_B_DEFSI,
		    `SC_SI_A_ROUTE, /*CFG13-12:          */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SR, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:          */
		    6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_OF,
		    2'h00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
		    `SC_FIFO_LEVEL /*CFG17-16:          */
		,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		)) dp32(
		        /*  input                   */  .reset(1'b0),
		        /*  input                   */  .clk(clock),
		        /*  input   [02:00]         */  .cs_addr(cs_addr),
		        /*  input                   */  .route_si(right_shift_msb),
		        /*  input                   */  .route_ci(1'b0),
		        /*  input                   */  .f0_load(f0_load),
		        /*  input                   */  .f1_load(f1_load),
		        /*  input                   */  .d0_load(1'b0),
		        /*  input                   */  .d1_load(1'b0),
		        /*  output  [03:00]                  */  .ce0(),
		        /*  output  [03:00]                  */  .cl0(),
		        /*  output  [03:00]                  */  .z0(),
		        /*  output  [03:00]                  */  .ff0(),
		        /*  output  [03:00]                  */  .ce1(ce1),
		        /*  output  [03:00]                  */  .cl1(),
		        /*  output  [03:00]                  */  .z1(),
		        /*  output  [03:00]                  */  .ff1(),
		        /*  output  [03:00]                  */  .ov_msb(),
		        /*  output  [03:00]                  */  .co_msb(),
		        /*  output  [03:00]                  */  .cmsb(),
		        /*  output  [03:00]                  */  .so(),
		        /*  output  [03:00]                  */  .f0_bus_stat(f0_bus_stat),
		        /*  output  [03:00]                  */  .f0_blk_stat(f0_blk_stat),
		        /*  output  [03:00]                  */  .f1_bus_stat(),
		        /*  output  [03:00]                  */  .f1_blk_stat()
		);
	end
endgenerate

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line

