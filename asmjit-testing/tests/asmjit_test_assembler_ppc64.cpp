// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include <asmjit/core.h>
#if !defined(ASMJIT_NO_PPC64)

#include <asmjit/ppc64.h>

#include <asmjit-testing/tests/asmjit_test_assembler.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace asmjit;

#define TEST_INSTRUCTION(OPCODE, ...) \
  tester.test_valid_instruction(#__VA_ARGS__, OPCODE, tester.assembler.__VA_ARGS__)

#define FAIL_INSTRUCTION(ERROR, ...) \
  tester.test_invalid_instruction(#__VA_ARGS__, ERROR, tester.assembler.__VA_ARGS__)

// Expected bytes were produced by GNU assembler (`as -mpower9 -mlittle`).
static void ASMJIT_NOINLINE test_ppc64_assembler_base(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("142A647C", add(r3, r4, r5));
  TEST_INSTRUCTION("140AE07F", add(r31, r0, r1));
  TEST_INSTRUCTION("1542C77C", add_rc(r6, r7, r8));
  TEST_INSTRUCTION("1428647C", addc(r3, r4, r5));
  TEST_INSTRUCTION("1408E07F", addc(r31, r0, r1));
  TEST_INSTRUCTION("1540C77C", addc_rc(r6, r7, r8));
  TEST_INSTRUCTION("1444C77C", addco(r6, r7, r8));
  TEST_INSTRUCTION("1544C77C", addco_rc(r6, r7, r8));
  TEST_INSTRUCTION("1429647C", adde(r3, r4, r5));
  TEST_INSTRUCTION("1409E07F", adde(r31, r0, r1));
  TEST_INSTRUCTION("1541C77C", adde_rc(r6, r7, r8));
  TEST_INSTRUCTION("1445C77C", addeo(r6, r7, r8));
  TEST_INSTRUCTION("1545C77C", addeo_rc(r6, r7, r8));
  TEST_INSTRUCTION("01006438", addi(r3, r4, 1));
  TEST_INSTRUCTION("00806138", addi(r3, r1, -32768));
  TEST_INSTRUCTION("FF7F7F38", addi(r3, r31, 32767));
  TEST_INSTRUCTION("01006430", addic(r3, r4, 1));
  TEST_INSTRUCTION("0080E033", addic(r31, r0, -32768));
  TEST_INSTRUCTION("FF7F1F30", addic(r0, r31, 32767));
  TEST_INSTRUCTION("01006434", addic_rc(r3, r4, 1));
  TEST_INSTRUCTION("0080E037", addic_rc(r31, r0, -32768));
  TEST_INSTRUCTION("FF7F1F34", addic_rc(r0, r31, 32767));
  TEST_INSTRUCTION("0100643C", addis(r3, r4, 1));
  TEST_INSTRUCTION("FFFF643C", addis(r3, r4, -1));
  TEST_INSTRUCTION("D401647C", addme(r3, r4));
  TEST_INSTRUCTION("D401E07F", addme(r31, r0));
  TEST_INSTRUCTION("D501C77C", addme_rc(r6, r7));
  TEST_INSTRUCTION("D405C77C", addmeo(r6, r7));
  TEST_INSTRUCTION("D505C77C", addmeo_rc(r6, r7));
  TEST_INSTRUCTION("1446C77C", addo(r6, r7, r8));
  TEST_INSTRUCTION("1546C77C", addo_rc(r6, r7, r8));
  TEST_INSTRUCTION("9401647C", addze(r3, r4));
  TEST_INSTRUCTION("9401E07F", addze(r31, r0));
  TEST_INSTRUCTION("9501C77C", addze_rc(r6, r7));
  TEST_INSTRUCTION("9405C77C", addzeo(r6, r7));
  TEST_INSTRUCTION("9505C77C", addzeo_rc(r6, r7));
  TEST_INSTRUCTION("2000804E", bclr(20, 0, 0));
  TEST_INSTRUCTION("2000824D", bclr(12, 2, 0));
  TEST_INSTRUCTION("20089E4C", bclr(4, 30, 1));
  TEST_INSTRUCTION("D22B647C", divd(r3, r4, r5));
  TEST_INSTRUCTION("D20BE07F", divd(r31, r0, r1));
  TEST_INSTRUCTION("D343C77C", divd_rc(r6, r7, r8));
  TEST_INSTRUCTION("D247C77C", divdo(r6, r7, r8));
  TEST_INSTRUCTION("D347C77C", divdo_rc(r6, r7, r8));
  TEST_INSTRUCTION("922B647C", divdu(r3, r4, r5));
  TEST_INSTRUCTION("920BE07F", divdu(r31, r0, r1));
  TEST_INSTRUCTION("9343C77C", divdu_rc(r6, r7, r8));
  TEST_INSTRUCTION("9247C77C", divduo(r6, r7, r8));
  TEST_INSTRUCTION("9347C77C", divduo_rc(r6, r7, r8));
  TEST_INSTRUCTION("D62B647C", divw(r3, r4, r5));
  TEST_INSTRUCTION("D60BE07F", divw(r31, r0, r1));
  TEST_INSTRUCTION("D743C77C", divw_rc(r6, r7, r8));
  TEST_INSTRUCTION("D647C77C", divwo(r6, r7, r8));
  TEST_INSTRUCTION("D747C77C", divwo_rc(r6, r7, r8));
  TEST_INSTRUCTION("962B647C", divwu(r3, r4, r5));
  TEST_INSTRUCTION("960BE07F", divwu(r31, r0, r1));
  TEST_INSTRUCTION("9743C77C", divwu_rc(r6, r7, r8));
  TEST_INSTRUCTION("9647C77C", divwuo(r6, r7, r8));
  TEST_INSTRUCTION("9747C77C", divwuo_rc(r6, r7, r8));
  TEST_INSTRUCTION("000064E8", ld(r3, ptr(r4)));
  TEST_INSTRUCTION("080064E8", ld(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("008061E8", ld(r3, ptr(r1, -32768)));
  TEST_INSTRUCTION("FC7FFFEB", ld(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("01006038", li(r3, 1));
  TEST_INSTRUCTION("0080E03B", li(r31, -32768));
  TEST_INSTRUCTION("FF7F0038", li(r0, 32767));
  TEST_INSTRUCTION("0100603C", lis(r3, 1));
  TEST_INSTRUCTION("0080E03F", lis(r31, -32768));
  TEST_INSTRUCTION("FF7F003C", lis(r0, 32767));
  TEST_INSTRUCTION("A602687C", mfspr(r3, 8));
  TEST_INSTRUCTION("A6426C7C", mfspr(r3, 268));
  TEST_INSTRUCTION("122E647C", modsd(r3, r4, r5));
  TEST_INSTRUCTION("120EE07F", modsd(r31, r0, r1));
  TEST_INSTRUCTION("162E647C", modsw(r3, r4, r5));
  TEST_INSTRUCTION("160EE07F", modsw(r31, r0, r1));
  TEST_INSTRUCTION("122A647C", modud(r3, r4, r5));
  TEST_INSTRUCTION("120AE07F", modud(r31, r0, r1));
  TEST_INSTRUCTION("162A647C", moduw(r3, r4, r5));
  TEST_INSTRUCTION("160AE07F", moduw(r31, r0, r1));
  TEST_INSTRUCTION("A603687C", mtspr(8, r3));
  TEST_INSTRUCTION("A603897D", mtspr(9, r12));
  TEST_INSTRUCTION("9228647C", mulhd(r3, r4, r5));
  TEST_INSTRUCTION("9208E07F", mulhd(r31, r0, r1));
  TEST_INSTRUCTION("9340C77C", mulhd_rc(r6, r7, r8));
  TEST_INSTRUCTION("1228647C", mulhdu(r3, r4, r5));
  TEST_INSTRUCTION("1208E07F", mulhdu(r31, r0, r1));
  TEST_INSTRUCTION("1340C77C", mulhdu_rc(r6, r7, r8));
  TEST_INSTRUCTION("9628647C", mulhw(r3, r4, r5));
  TEST_INSTRUCTION("9608E07F", mulhw(r31, r0, r1));
  TEST_INSTRUCTION("9740C77C", mulhw_rc(r6, r7, r8));
  TEST_INSTRUCTION("1628647C", mulhwu(r3, r4, r5));
  TEST_INSTRUCTION("1608E07F", mulhwu(r31, r0, r1));
  TEST_INSTRUCTION("1740C77C", mulhwu_rc(r6, r7, r8));
  TEST_INSTRUCTION("D229647C", mulld(r3, r4, r5));
  TEST_INSTRUCTION("D209E07F", mulld(r31, r0, r1));
  TEST_INSTRUCTION("D341C77C", mulld_rc(r6, r7, r8));
  TEST_INSTRUCTION("D245C77C", mulldo(r6, r7, r8));
  TEST_INSTRUCTION("D345C77C", mulldo_rc(r6, r7, r8));
  TEST_INSTRUCTION("0100641C", mulli(r3, r4, 1));
  TEST_INSTRUCTION("0080E01F", mulli(r31, r0, -32768));
  TEST_INSTRUCTION("FF7F1F1C", mulli(r0, r31, 32767));
  TEST_INSTRUCTION("D629647C", mullw(r3, r4, r5));
  TEST_INSTRUCTION("D609E07F", mullw(r31, r0, r1));
  TEST_INSTRUCTION("D741C77C", mullw_rc(r6, r7, r8));
  TEST_INSTRUCTION("D645C77C", mullwo(r6, r7, r8));
  TEST_INSTRUCTION("D745C77C", mullwo_rc(r6, r7, r8));
  TEST_INSTRUCTION("D000647C", neg(r3, r4));
  TEST_INSTRUCTION("D000E07F", neg(r31, r0));
  TEST_INSTRUCTION("D100C77C", neg_rc(r6, r7));
  TEST_INSTRUCTION("D004C77C", nego(r6, r7));
  TEST_INSTRUCTION("D104C77C", nego_rc(r6, r7));
  TEST_INSTRUCTION("FFFF8360", ori(r3, r4, 65535));
  TEST_INSTRUCTION("0100DF63", ori(r31, r30, 1));
  TEST_INSTRUCTION("000064F8", std(r3, ptr(r4)));
  TEST_INSTRUCTION("080064F8", std(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("100001F8", std(r0, ptr(r1, 16)));
  TEST_INSTRUCTION("0080FFFB", std(r31, ptr(r31, -32768)));
  TEST_INSTRUCTION("5028647C", subf(r3, r4, r5));
  TEST_INSTRUCTION("5008E07F", subf(r31, r0, r1));
  TEST_INSTRUCTION("5140C77C", subf_rc(r6, r7, r8));
  TEST_INSTRUCTION("1028647C", subfc(r3, r4, r5));
  TEST_INSTRUCTION("1008E07F", subfc(r31, r0, r1));
  TEST_INSTRUCTION("1140C77C", subfc_rc(r6, r7, r8));
  TEST_INSTRUCTION("1044C77C", subfco(r6, r7, r8));
  TEST_INSTRUCTION("1144C77C", subfco_rc(r6, r7, r8));
  TEST_INSTRUCTION("1029647C", subfe(r3, r4, r5));
  TEST_INSTRUCTION("1009E07F", subfe(r31, r0, r1));
  TEST_INSTRUCTION("1141C77C", subfe_rc(r6, r7, r8));
  TEST_INSTRUCTION("1045C77C", subfeo(r6, r7, r8));
  TEST_INSTRUCTION("1145C77C", subfeo_rc(r6, r7, r8));
  TEST_INSTRUCTION("01006420", subfic(r3, r4, 1));
  TEST_INSTRUCTION("0080E023", subfic(r31, r0, -32768));
  TEST_INSTRUCTION("FF7F1F20", subfic(r0, r31, 32767));
  TEST_INSTRUCTION("D001647C", subfme(r3, r4));
  TEST_INSTRUCTION("D001E07F", subfme(r31, r0));
  TEST_INSTRUCTION("D101C77C", subfme_rc(r6, r7));
  TEST_INSTRUCTION("D005C77C", subfmeo(r6, r7));
  TEST_INSTRUCTION("D105C77C", subfmeo_rc(r6, r7));
  TEST_INSTRUCTION("5044C77C", subfo(r6, r7, r8));
  TEST_INSTRUCTION("5144C77C", subfo_rc(r6, r7, r8));
  TEST_INSTRUCTION("9001647C", subfze(r3, r4));
  TEST_INSTRUCTION("9001E07F", subfze(r31, r0));
  TEST_INSTRUCTION("9101C77C", subfze_rc(r6, r7));
  TEST_INSTRUCTION("9005C77C", subfzeo(r6, r7));
  TEST_INSTRUCTION("9105C77C", subfzeo_rc(r6, r7));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_extended(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("2000804E", blr());
  TEST_INSTRUCTION("A602897D", mfctr(r12));
  TEST_INSTRUCTION("A602087C", mflr(r0));
  TEST_INSTRUCTION("A603897D", mtctr(r12));
  TEST_INSTRUCTION("A603087C", mtlr(r0));
  TEST_INSTRUCTION("00000060", nop());
  TEST_INSTRUCTION("5020657C", sub(r3, r4, r5));
  TEST_INSTRUCTION("5120657C", sub_rc(r3, r4, r5));
  TEST_INSTRUCTION("5024657C", subo(r3, r4, r5));
  TEST_INSTRUCTION("5124657C", subo_rc(r3, r4, r5));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_invalid(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  // R0 would be read as zero.
  FAIL_INSTRUCTION(Error::kInvalidPhysId, addi(r3, r0, 1));
  FAIL_INSTRUCTION(Error::kInvalidPhysId, addis(r3, r0, 1));
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, ptr(r0, 8)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, std(r3, ptr(r0, 8)));

  // Immediate out of range.
  FAIL_INSTRUCTION(Error::kInvalidImmediate, addi(r3, r4, 32768));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, addi(r3, r4, -32769));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, li(r3, 32768));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, lis(r3, -32769));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, mulli(r3, r4, 32768));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, ori(r3, r4, -1));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, ori(r3, r4, 65536));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, mfspr(r3, 1024));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(32, 0, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(20, 32, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(20, 0, 4));

  // Displacement must be a multiple of 4 that fits into 16 bits.
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, -1)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, 2)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, 32768)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, std(r3, ptr(r4, -32772)));

  // These instructions only provide base + displacement addressing.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, ptr(r4, r5)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, ptr(tester.L0)));

  // Floating point register used as a general purpose register.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, Mem(f4, 8)));
}

// Expected bytes were produced by GNU assembler (`as -mpower9 -mbig`).
static void ASMJIT_NOINLINE test_ppc64_assembler_big_endian(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("7C642A14", add(r3, r4, r5));
  TEST_INSTRUCTION("38618000", addi(r3, r1, -32768));
  TEST_INSTRUCTION("4E800020", blr());
  TEST_INSTRUCTION("EBFF7FFC", ld(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("7C0802A6", mflr(r0));
  TEST_INSTRUCTION("7D8903A6", mtctr(r12));
  TEST_INSTRUCTION("7C643DD3", mulldo_rc(r3, r4, r7));
  TEST_INSTRUCTION("60000000", nop());
  TEST_INSTRUCTION("F8010010", std(r0, ptr(r1, 16)));
}

bool test_ppc64_assembler(const TestSettings& settings) noexcept {
  using namespace ppc;

  AssemblerTester<Assembler> tester_le(Arch::kPPC64_LE, settings);
  tester_le.print_header("PPC64_LE");

  test_ppc64_assembler_base(tester_le);
  test_ppc64_assembler_extended(tester_le);
  test_ppc64_assembler_invalid(tester_le);

  tester_le.print_summary();

  AssemblerTester<Assembler> tester_be(Arch::kPPC64_BE, settings);
  tester_be.print_header("PPC64_BE");

  test_ppc64_assembler_big_endian(tester_be);

  tester_be.print_summary();
  return tester_le.did_pass() && tester_be.did_pass();
}

#undef FAIL_INSTRUCTION
#undef TEST_INSTRUCTION

#endif // !ASMJIT_NO_PPC64
