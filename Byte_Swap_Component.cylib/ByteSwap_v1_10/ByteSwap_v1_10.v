
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
// Generated on 11/08/2011 at 19:09
// Component: ByteSwap_v1_10
module ByteSwap_v1_10 (
	InDMA,
	OutDMA,
	Clock
);
	input   Clock;
	output InDMA;
	output OutDMA;


//`#start body` -- edit after this line, do not edit this line

	/***************************************************************************
    *       Control Register Implementation                                      
    ***************************************************************************/   
    wire [7:0] ctrl;
    /* Control Register bit location (bits 3-7 are unused) */
    localparam BYTESWAP_CTRL_ENABLE          = 1'd0;
    localparam BYTESWAP_RES_CTRL          = 1'd1;
    
    /* Add support of sync mode for PSoC3 ES3 Rev */
	cy_psoc3_control #(.cy_force_order(1)) ControlReg
	(
	    /* output [07:00] */  .control(ctrl)
	);
	
	/* Enable control signals */
    reg enable;
    reg resolution_ctrl;
	
	reg [4:0] State;
	reg f0_blk_stat;	/* Set to 1 if the FIFO is not empty */
	reg f1_blk_stat;   /* Set to 1 if F1 FIFO is full */
	wire f0_not_full_stat; /* FIFO 0 level status */
	wire f1_not_empty_stat; /* FIFO 1 level status */
	
	/* Local parameters for driving data path block */
	localparam BYTESWAP_IDLE0 = 5'd8;
	localparam BYTESWAP_IDLE1 = 5'd14;
	localparam BYTESWAP_IDLE2 = 5'd16;
	localparam BYTESWAP_IDLE3 = 5'd22;
    localparam BYTESWAP_IDLE4 = 5'd24;
	localparam BYTESWAP_IDLE5 = 5'd30;
    
    localparam BYTESWAP_STATE0 = 5'd0;
	localparam BYTESWAP_STATE1 = 5'd1;
	localparam BYTESWAP_STATE2 = 5'd2;
	localparam BYTESWAP_STATE3 = 5'd3;
	localparam BYTESWAP_STATE4 = 5'd4;
	localparam BYTESWAP_STATE5 = 5'd5;
	localparam BYTESWAP_STATE6 = 5'd6;
	localparam BYTESWAP_STATE7 = 5'd7;
	

	always @(posedge Clock) 
	begin
		enable <= ctrl[BYTESWAP_CTRL_ENABLE];
        resolution_ctrl <= ctrl[BYTESWAP_RES_CTRL];
	end
	
	assign InDMA = enable? f0_not_full_stat : 1'b0;
	assign OutDMA = enable? f1_not_empty_stat : 1'b0;

	
    always @(posedge Clock) 
	begin
		if(enable)
		begin
			case (State)
			BYTESWAP_IDLE0: 
				begin
    			    if (resolution_ctrl)
                    begin 
                    State <= BYTESWAP_STATE2;
                    end
                    else 
                    begin 
                    State <= BYTESWAP_STATE0;
                    end
                    
                end
            
                       
            BYTESWAP_STATE0: 
			  begin
                //Datapath Nothing ;
				if(!f0_blk_stat) 
				begin
					State <= BYTESWAP_STATE1; /* Start the swap process only when FIFO is full */
				end	
			  end	
			
			BYTESWAP_STATE1:
			  begin
				//F0 => A0;
				State <= BYTESWAP_IDLE1;
			  end	
		    
            BYTESWAP_STATE2:
			  begin
				//A0 => 0;
				State <= BYTESWAP_IDLE1;
			  end	
            
            
			BYTESWAP_IDLE1: 
				if(!f0_blk_stat) 
				begin
					State <= BYTESWAP_STATE3;
				end
                
            BYTESWAP_STATE3:
			  begin
				//ALU[A0] => A1 & F0=>D0
				State <= BYTESWAP_IDLE2;
			  end	    
			
            
            
            BYTESWAP_IDLE2: 
				if(!f0_blk_stat) 
				begin
					State <= BYTESWAP_STATE4;
				end
            
            BYTESWAP_STATE4:
			  begin
				//F0-> A0;
				State <= BYTESWAP_IDLE3;
			  end	
            
                       
			BYTESWAP_IDLE3: 
				if(!f1_blk_stat) 
				begin
					State <= BYTESWAP_STATE5;
				end
            
             BYTESWAP_STATE5:
			  begin
				//ALU[A0] => F1, D0-> A0;
				State <= BYTESWAP_IDLE4;
			  end
            
            BYTESWAP_IDLE4: 
				if(!f1_blk_stat) 
				begin
					State <= BYTESWAP_STATE6;
				end
            
             BYTESWAP_STATE6:
			  begin
				//ALU[D0] => F1;
				State <= BYTESWAP_IDLE5;
			  end
            
             BYTESWAP_IDLE5: 
				if(!f1_blk_stat) 
				begin
					State <= BYTESWAP_STATE7;
				end
            
             BYTESWAP_STATE7:
			  begin
				//ALU[A1] => F1;
				State <= BYTESWAP_IDLE0;
			  end
              
			endcase
		end
		else
		begin
			State <= BYTESWAP_IDLE0;	
		end
	end
	
	/* Signal for loading F1 from D0 */
	wire f1_load = (State == BYTESWAP_STATE7 || State == BYTESWAP_STATE6 || State == BYTESWAP_STATE5);
	
	/* Signal for loading D0 register from F0 */
	wire d0_load = (State == BYTESWAP_STATE3);
	
//        Your code goes here
// 8-Bit Datapath

//`#end` -- edit above this line, do not edit this line

cy_psoc3_dp8 #(.cy_dpconfig_a(
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0:  Wait for F0 to fill up*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1:              F0 -> A0*/
    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2: A0=0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3:  ALU[A0]-> A1, F0-> D0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4:         F0->A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5:          ALU(A0) -> F1,D0 -> A0,*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6:              ALU(D0) -> F1*/
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7:              ALU (A1) -> F1 */
    8'hFF, 8'h00,  /*CFG9:              */
    8'hFF, 8'hFF,  /*CFG11-10:              */
    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_DEFSI, /*CFG13-12:              */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1_ALU, `SC_FIFO0_BUS,
    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
    `SC_CMP0_NOCHN, /*CFG15-14:              */
    10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO_LEVEL,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:              */
}
)) dp_ByteSwap(
        /*  input                   */  .reset(1'b0),
        /*  input                   */  .clk(Clock),
        /*  input   [02:00]         */  .cs_addr(State[2:0]),
        /*  input                   */  .route_si(1'b0),
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(1'b0),
        /*  input                   */  .f1_load(f1_load),
        /*  input                   */  .d0_load(d0_load),
        /*  input                   */  .d1_load(1'b0),
        /*  output                  */  .ce0(),
        /*  output                  */  .cl0(),
        /*  output                  */  .z0(),
        /*  output                  */  .ff0(),
        /*  output                  */  .ce1(),
        /*  output                  */  .cl1(),
        /*  output                  */  .z1(),
        /*  output                  */  .ff1(),
        /*  output                  */  .ov_msb(),
        /*  output                  */  .co_msb(),
        /*  output                  */  .cmsb(),
        /*  output                  */  .so(),
        /*  output                  */  .f0_bus_stat(f0_not_full_stat),
        /*  output                  */  .f0_blk_stat(f0_blk_stat),
        /*  output                  */  .f1_bus_stat(f1_not_empty_stat),
        /*  output                  */  .f1_blk_stat(f1_blk_stat)
);
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line













