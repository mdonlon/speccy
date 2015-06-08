/* 0x00 */
OP( NOP, 4 )
OP( LD_BC_NN, 10 )
OP( LD_RBC_A, 7 )
OP( INC_BC, 6 )
OP( INC_B, 4 )
OP( DEC_B, 4 )
OP( LD_B_N, 7 )
OP( RLCA, 4 )
OP( EX_AF_AF, 4 )
OP( ADD_HL_BC, 11 )
OP( LD_A_RBC, 7 )
OP( DEC_BC, 6 )
OP( INC_C, 4 )
OP( DEC_C, 4 )
OP( LD_C_N, 7 )
OP( RRCA, 4 )

/* 0x10 */
OP( DJNZ_N, 8 )
OP( LD_DE_NN, 10 )
OP( LD_RDE_A, 7 )
OP( INC_DE, 6 )
OP( INC_D, 4 )
OP( DEC_D, 4 )
OP( LD_D_N, 7 )
OP( RLA, 4 )
OP( JR_N, 7 )
OP( ADD_HL_DE, 11 )
OP( LD_A_RDE, 7 )
OP( DEC_DE, 6 )
OP( INC_E, 4 )
OP( DEC_E, 4 )
OP( LD_E_N, 7 )
OP( RRA, 4 )

/* 0x20 */
OP( JR_NZ_N, 7 )
OP( LD_HL_NN, 10 )
OP( LD_RNN_HL, 16 )
OP( INC_HL, 6 )
OP( INC_H, 4 )
OP( DEC_H, 4 )
OP( LD_H_N, 7 )
OP( DAA, 4 )
OP( JR_Z_N, 7 )
OP( ADD_HL_HL, 11 )
OP( LD_HL_RNN, 20 )
OP( DEC_HL, 6 )
OP( INC_L, 4 )
OP( DEC_L, 4 )
OP( LD_L_N, 7 )
OP( CPL, 4 )

/* 0x30 */
OP( JR_NC_N, 7 )
OP( LD_SP_NN, 10 )
OP( LD_RNN_A, 13 )
OP( INC_SP, 6 )
OP( INC_RHL, 11 )
OP( DEC_RHL, 11 )
OP( LD_RHL_N, 10 )
OP( SCF, 4 )
OP( JR_C_N, 7 )
OP( ADD_HL_SP, 11 )
OP( LD_A_RNN, 13 )
OP( DEC_SP, 6 )
OP( INC_A, 4 )
OP( DEC_A, 4 )
OP( LD_A_N, 7 )
OP( CCF, 4 )

/* 0x40 */
OP( LD_B_B, 4 )
OP( LD_B_C, 4 )
OP( LD_B_D, 4 )
OP( LD_B_E, 4 )
OP( LD_B_H, 4 )
OP( LD_B_L, 4 )
OP( LD_B_RHL, 7 )
OP( LD_B_A, 4 )
OP( LD_C_B, 4 )
OP( LD_C_C, 4 )
OP( LD_C_D, 4 )
OP( LD_C_E, 4 )
OP( LD_C_H, 4 )
OP( LD_C_L, 4 )
OP( LD_C_RHL, 7 )
OP( LD_C_A, 4 )

/* 0x50 */
OP( LD_D_B, 4 )
OP( LD_D_C, 4 )
OP( LD_D_D, 4 )
OP( LD_D_E, 4 )
OP( LD_D_H, 4 )
OP( LD_D_L, 4 )
OP( LD_D_RHL, 7 )
OP( LD_D_A, 4 )
OP( LD_E_B, 4 )
OP( LD_E_C, 4 )
OP( LD_E_D, 4 )
OP( LD_E_E, 4 )
OP( LD_E_H, 4 )
OP( LD_E_L, 4 )
OP( LD_E_RHL, 7 )
OP( LD_E_A, 4 )

/* 0x60 */
OP( LD_H_B, 4 )
OP( LD_H_C, 4 )
OP( LD_H_D, 4 )
OP( LD_H_E, 4 )
OP( LD_H_H, 4 )
OP( LD_H_L, 4 )
OP( LD_H_RHL, 7 )
OP( LD_H_A, 4 )
OP( LD_L_B, 4 )
OP( LD_L_C, 4 )
OP( LD_L_D, 4 )
OP( LD_L_E, 4 )
OP( LD_L_H, 4 )
OP( LD_L_L, 4 )
OP( LD_L_RHL, 7 )
OP( LD_L_A, 4 )

/* 0x70 */
OP( LD_RHL_B, 7 )
OP( LD_RHL_C, 7 )
OP( LD_RHL_D, 7 )
OP( LD_RHL_E, 7 )
OP( LD_RHL_H, 7 )
OP( LD_RHL_L, 7 )
OP( HALT, 4 )
OP( LD_RHL_A, 7 )
OP( LD_A_B, 4 )
OP( LD_A_C, 4 )
OP( LD_A_D, 4 )
OP( LD_A_E, 4 )
OP( LD_A_H, 4 )
OP( LD_A_L, 4 )
OP( LD_A_RHL, 7 )
OP( LD_A_A, 4 )

/* 0x80 */
OP( ADD_A_B, 4 )
OP( ADD_A_C, 4 )
OP( ADD_A_D, 4 )
OP( ADD_A_E, 4 )
OP( ADD_A_H, 4 )
OP( ADD_A_L, 4 )
OP( ADD_A_RHL, 7 )
OP( ADD_A_A, 4 )
OP( ADC_A_B, 4 )
OP( ADC_A_C, 4 )
OP( ADC_A_D, 4 )
OP( ADC_A_E, 4 )
OP( ADC_A_H, 4 )
OP( ADC_A_L, 4 )
OP( ADC_A_RHL, 7 )
OP( ADC_A_A, 4 )

/* 0x90 */
OP( SUB_A_B, 4 )
OP( SUB_A_C, 4 )
OP( SUB_A_D, 4 )
OP( SUB_A_E, 4 )
OP( SUB_A_H, 4 )
OP( SUB_A_L, 4 )
OP( SUB_A_RHL, 7 )
OP( SUB_A_A, 4 )
OP( SBC_A_B, 4 )
OP( SBC_A_C, 4 )
OP( SBC_A_D, 4 )
OP( SBC_A_E, 4 )
OP( SBC_A_H, 4 )
OP( SBC_A_L, 4 )
OP( SBC_A_RHL, 7 )
OP( SBC_A_A, 4 )

/* 0xA0 */
OP( AND_B, 4 )
OP( AND_C, 4 )
OP( AND_D, 4 )
OP( AND_E, 4 )
OP( AND_H, 4 )
OP( AND_L, 4 )
OP( AND_RHL, 7 )
OP( AND_A, 4 )
OP( XOR_B, 4 )
OP( XOR_C, 4 )
OP( XOR_D, 4 )
OP( XOR_E, 4 )
OP( XOR_H, 4 )
OP( XOR_L, 4 )
OP( XOR_RHL, 7 )
OP( XOR_A, 4 )

/* 0xB0 */
OP( OR_B, 4 )
OP( OR_C, 4 )
OP( OR_D, 4 )
OP( OR_E, 4 )
OP( OR_H, 4 )
OP( OR_L, 4 )
OP( OR_RHL, 7 )
OP( OR_A, 4 )
OP( CP_B, 4 )
OP( CP_C, 4 )
OP( CP_D, 4 )
OP( CP_E, 4 )
OP( CP_H, 4 )
OP( CP_L, 4 )
OP( CP_RHL, 7 )
OP( CP_A, 4 )

/* 0xC0 */
OP( RET_NZ, 5 )
OP( POP_BC, 10 )
OP( JP_NZ_NN, 1 )
OP( JP_NN, 1 )
OP( CALL_NZ_NN, 1 )
OP( PUSH_BC, 11 )
OP( ADD_A_N, 7 )
OP( RST_00, 11 )
OP( RET_Z, 5 )
OP( RET, 4 )
OP( JP_Z_NN, 1 )
OP( PREFIX_CB, 4 )
OP( CALL_Z_NN, 1 )
OP( CALL_NN, 1 )
OP( ADC_A_N, 7 )
OP( RST_08, 11 )

/* 0xD0 */
OP( RET_NC, 5 )
OP( POP_DE, 10 )
OP( JP_NC_NN, 1 )
OP( OUT_RN_A, 11 )
OP( CALL_NC_NN, 1 )
OP( PUSH_DE, 11 )
OP( SUB_A_N, 7 )
OP( RST_10, 11 )
OP( RET_C, 5 )
OP( EXX, 4 )
OP( JP_C_NN, 1 )
OP( IN_A_RN, 11 )
OP( CALL_C_NN, 1 )
OP( PREFIX_DD, 4 )
OP( SBC_A_N, 7 )
OP( RST_18, 11 )

/* 0xE0 */
OP( RET_PO, 5 )
OP( POP_HL, 10 )
OP( JP_PO_NN, 1 )
OP( EX_RSP_HL, 19 )
OP( CALL_PO_NN, 1 )
OP( PUSH_HL, 11 )
OP( AND_N, 7 )
OP( RST_20, 11 )
OP( RET_PE, 5 )
OP( JP_HL, 1 )
OP( JP_PE_NN, 1 )
OP( EX_DE_HL, 4 )
OP( CALL_PE_NN, 1 )
OP( PREFIX_ED, 4 )
OP( XOR_N, 7 )
OP( RST_28, 11 )

/* 0xF0 */
OP( RET_P, 5 )
OP( POP_AF, 10 )
OP( JP_P_NN, 1 )
OP( DI, 4 )
OP( CALL_P_NN, 1 )
OP( PUSH_AF, 11 )
OP( OR_N, 7 )
OP( RST_30, 11 )
OP( RET_M, 5 )
OP( LD_SP_HL, 6 )
OP( JP_M_NN, 1 )
OP( EI, 4 )
OP( CALL_M_NN, 1 )
OP( PREFIX_FD, 4 )
OP( CP_N, 7 )
OP( RST_38, 11 )

