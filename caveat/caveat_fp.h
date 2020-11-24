#define SOFT_FP
#ifndef NO_FP_MACROS
#define FMADD32( rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f32 = f32_mulAdd( F32(rs1), F32(rs2),  F32(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMADD32_dyn(rd, rs1, rs2, rs3) FR(rd).f32 = f32_mulAdd( F32(rs1), F32(rs2),  F32(rs3)); SET_FPX;
#define FMSUB32( rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f32 = f32_mulAdd( F32(rs1), F32(rs2), NF32(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMSUB32_dyn(rd, rs1, rs2, rs3) FR(rd).f32 = f32_mulAdd( F32(rs1), F32(rs2), NF32(rs3)); SET_FPX;
#define FNMADD32(rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f32 = f32_mulAdd(NF32(rs1), F32(rs2), NF32(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FNMADD32_dyn(rd, rs1, rs2, rs3) FR(rd).f32 = f32_mulAdd(NF32(rs1), F32(rs2), NF32(rs3)); SET_FPX;
#define FNMSUB32(rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f32 = f32_mulAdd(NF32(rs1), F32(rs2),  F32(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FNMSUB32_dyn(rd, rs1, rs2, rs3) FR(rd).f32 = f32_mulAdd(NF32(rs1), F32(rs2),  F32(rs3)); SET_FPX;
#define FMADD64( rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f64 = f64_mulAdd( F64(rs1), F64(rs2),  F64(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMADD64_dyn(rd, rs1, rs2, rs3) FR(rd).f64 = f64_mulAdd( F64(rs1), F64(rs2),  F64(rs3)); SET_FPX;
#define FMSUB64( rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f64 = f64_mulAdd( F64(rs1), F64(rs2), NF64(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMSUB64_dyn(rd, rs1, rs2, rs3) FR(rd).f64 = f64_mulAdd( F64(rs1), F64(rs2), NF64(rs3)); SET_FPX;
#define FNMADD64(rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f64 = f64_mulAdd(NF64(rs1), F64(rs2), NF64(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FNMADD64_dyn(rd, rs1, rs2, rs3) FR(rd).f64 = f64_mulAdd(NF64(rs1), F64(rs2), NF64(rs3)); SET_FPX;
#define FNMSUB64(rm, rd, rs1, rs2, rs3) SRM(rm); FR(rd).f64 = f64_mulAdd(NF64(rs1), F64(rs2),  F64(rs3)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FNMSUB64_dyn(rd, rs1, rs2, rs3) FR(rd).f64 = f64_mulAdd(NF64(rs1), F64(rs2),  F64(rs3)); SET_FPX;
#define FADD32(rm, rd, rs1, rs2) SRM(rm); FR(rd).f32 = f32_add(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FADD32_dyn(rd, rs1, rs2) FR(rd).f32 = f32_add(F32(rs1), F32(rs2)); SET_FPX;
#define FSUB32(rm, rd, rs1, rs2) SRM(rm); FR(rd).f32 = f32_sub(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FSUB32_dyn(rd, rs1, rs2) FR(rd).f32 = f32_sub(F32(rs1), F32(rs2)); SET_FPX;
#define FMUL32(rm, rd, rs1, rs2) SRM(rm); FR(rd).f32 = f32_mul(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMUL32_dyn(rd, rs1, rs2) FR(rd).f32 = f32_mul(F32(rs1), F32(rs2)); SET_FPX;
#define FDIV32(rm, rd, rs1, rs2) SRM(rm); FR(rd).f32 = f32_div(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FDIV32_dyn(rd, rs1, rs2) FR(rd).f32 = f32_div(F32(rs1), F32(rs2)); SET_FPX;
#define FADD64(rm, rd, rs1, rs2) SRM(rm); FR(rd).f64 = f64_add(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FADD64_dyn(rd, rs1, rs2) FR(rd).f64 = f64_add(F64(rs1), F64(rs2)); SET_FPX;
#define FSUB64(rm, rd, rs1, rs2) SRM(rm); FR(rd).f64 = f64_sub(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FSUB64_dyn(rd, rs1, rs2) FR(rd).f64 = f64_sub(F64(rs1), F64(rs2)); SET_FPX;
#define FMUL64(rm, rd, rs1, rs2) SRM(rm); FR(rd).f64 = f64_mul(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMUL64_dyn(rd, rs1, rs2) FR(rd).f64 = f64_mul(F64(rs1), F64(rs2)); SET_FPX;
#define FDIV64(rm, rd, rs1, rs2) SRM(rm); FR(rd).f64 = f64_div(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FDIV64_dyn(rd, rs1, rs2) FR(rd).f64 = f64_div(F64(rs1), F64(rs2)); SET_FPX;
#define FSQRT32(rm, rd, rs1) SRM(rm); FR(rd).f32 = f32_sqrt(F32(rs1)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FSQRT32_dyn(rd, rs1) FR(rd).f32 = f32_sqrt(F32(rs1)); SET_FPX;
#define FSQRT64(rm, rd, rs1) SRM(rm); FR(rd).f64 = f64_sqrt(F64(rs1)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FSQRT64_dyn(rd, rs1) FR(rd).f64 = f64_sqrt(F64(rs1)); SET_FPX;
#define FCVTWS( rm, rd, rs1) SRM(rm); IR(rd).l  = (long)f32_to_i32( F32(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTWS_dyn(rd, rs1) IR(rd).l  = (long)f32_to_i32( F32(rs1), RM, true); SET_FPX;
#define FCVTWUS(rm, rd, rs1) SRM(rm); IR(rd).l  = (long)f32_to_ui32(F32(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTWUS_dyn(rd, rs1) IR(rd).l  = (long)f32_to_ui32(F32(rs1), RM, true); SET_FPX;
#define FCVTLS( rm, rd, rs1) SRM(rm); IR(rd).l  =       f32_to_i64( F32(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTLS_dyn(rd, rs1) IR(rd).l  =       f32_to_i64( F32(rs1), RM, true); SET_FPX;
#define FCVTLUS(rm, rd, rs1) SRM(rm); IR(rd).ul =       f32_to_ui64(F32(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTLUS_dyn(rd, rs1) IR(rd).ul =       f32_to_ui64(F32(rs1), RM, true); SET_FPX;
#define FCVTWD( rm, rd, rs1) SRM(rm); IR(rd).l  = (long)f64_to_i64( F64(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTWD_dyn(rd, rs1) IR(rd).l  = (long)f64_to_i64( F64(rs1), RM, true); SET_FPX;
#define FCVTWUD(rm, rd, rs1) SRM(rm); IR(rd).l  = (long)f64_to_ui64(F64(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTWUD_dyn(rd, rs1) IR(rd).l  = (long)f64_to_ui64(F64(rs1), RM, true); SET_FPX;
#define FCVTLD( rm, rd, rs1) SRM(rm); IR(rd).l  =       f64_to_i64( F64(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTLD_dyn(rd, rs1) IR(rd).l  =       f64_to_i64( F64(rs1), RM, true); SET_FPX;
#define FCVTLUD(rm, rd, rs1) SRM(rm); IR(rd).ul =       f64_to_ui64(F64(rs1), RM, true); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTLUD_dyn(rd, rs1) IR(rd).ul =       f64_to_ui64(F64(rs1), RM, true); SET_FPX;
#define FCVTSW( rm, rd, rs1) SRM(rm); FR(rd).f32 =  i32_to_f32(IR(rs1).i ); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTSW_dyn(rd, rs1) FR(rd).f32 =  i32_to_f32(IR(rs1).i ); SET_FPX;
#define FCVTSWU(rm, rd, rs1) SRM(rm); FR(rd).f32 = ui32_to_f32(IR(rs1).ui); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTSWU_dyn(rd, rs1) FR(rd).f32 = ui32_to_f32(IR(rs1).ui); SET_FPX;
#define FCVTSL( rm, rd, rs1) SRM(rm); FR(rd).f32 =  i64_to_f32(IR(rs1).l ); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTSL_dyn(rd, rs1) FR(rd).f32 =  i64_to_f32(IR(rs1).l ); SET_FPX;
#define FCVTSLU(rm, rd, rs1) SRM(rm); FR(rd).f32 = ui64_to_f32(IR(rs1).ul); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTSLU_dyn(rd, rs1) FR(rd).f32 = ui64_to_f32(IR(rs1).ul); SET_FPX;
#define FCVTDW( rm, rd, rs1) SRM(rm); FR(rd).f64 =  i32_to_f64(IR(rs1).i ); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTDW_dyn(rd, rs1) FR(rd).f64 =  i32_to_f64(IR(rs1).i ); SET_FPX;
#define FCVTDWU(rm, rd, rs1) SRM(rm); FR(rd).f64 = ui32_to_f64(IR(rs1).ui); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTDWU_dyn(rd, rs1) FR(rd).f64 = ui32_to_f64(IR(rs1).ui); SET_FPX;
#define FCVTDL( rm, rd, rs1) SRM(rm); FR(rd).f64 =  i64_to_f64(IR(rs1).l ); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTDL_dyn(rd, rs1) FR(rd).f64 =  i64_to_f64(IR(rs1).l ); SET_FPX;
#define FCVTDLU(rm, rd, rs1) SRM(rm); FR(rd).f64 = ui64_to_f64(IR(rs1).ul); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTDLU_dyn(rd, rs1) FR(rd).f64 = ui64_to_f64(IR(rs1).ul); SET_FPX;
#define FCVTSD(rm, rd, rs1) SRM(rm); FR(rd).f32 = f64_to_f32(F64(rs1)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTSD_dyn(rd, rs1) FR(rd).f32 = f64_to_f32(F64(rs1)); SET_FPX;
#define FCVTDS(rm, rd, rs1) SRM(rm); FR(rd).f64 = f32_to_f64(F32(rs1)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FCVTDS_dyn(rd, rs1) FR(rd).f64 = f32_to_f64(F32(rs1)); SET_FPX;
#define FEQS(rd, rs1, rs2) IR(rd).l = f32_eq(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FLTS(rd, rs1, rs2) IR(rd).l = f32_lt(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FLES(rd, rs1, rs2) IR(rd).l = f32_le(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FEQD(rd, rs1, rs2) IR(rd).l = f64_eq(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FLTD(rd, rs1, rs2) IR(rd).l = f64_lt(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FLED(rd, rs1, rs2) IR(rd).l = f64_le(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMINS(rd, rs1, rs2) FR(rd).f32 = f32_min(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMAXS(rd, rs1, rs2) FR(rd).f32 = f32_max(F32(rs1), F32(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMIND(rd, rs1, rs2) FR(rd).f64 = f64_min(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#define FMAXD(rd, rs1, rs2) FR(rd).f64 = f64_max(F64(rs1), F64(rs2)); SET_FPX; SRM(cpu->state.fcsr.rmode);
#endif