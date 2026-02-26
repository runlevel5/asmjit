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
  TEST_INSTRUCTION("01006438", addi(r3, r4, 1));
  TEST_INSTRUCTION("00806138", addi(r3, r1, -32768));
  TEST_INSTRUCTION("FF7F7F38", addi(r3, r31, 32767));
  TEST_INSTRUCTION("0100643C", addis(r3, r4, 1));
  TEST_INSTRUCTION("FFFF643C", addis(r3, r4, -1));
  TEST_INSTRUCTION("2000804E", bclr(20, 0, 0));
  TEST_INSTRUCTION("2000824D", bclr(12, 2, 0));
  TEST_INSTRUCTION("20089E4C", bclr(4, 30, 1));
  TEST_INSTRUCTION("000064E8", ld(r3, ptr(r4)));
  TEST_INSTRUCTION("080064E8", ld(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("008061E8", ld(r3, ptr(r1, -32768)));
  TEST_INSTRUCTION("FC7FFFEB", ld(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("A602687C", mfspr(r3, 8));
  TEST_INSTRUCTION("A6426C7C", mfspr(r3, 268));
  TEST_INSTRUCTION("A603687C", mtspr(8, r3));
  TEST_INSTRUCTION("A603897D", mtspr(9, r12));
  TEST_INSTRUCTION("FFFF8360", ori(r3, r4, 65535));
  TEST_INSTRUCTION("0100DF63", ori(r31, r30, 1));
  TEST_INSTRUCTION("000064F8", std(r3, ptr(r4)));
  TEST_INSTRUCTION("080064F8", std(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("100001F8", std(r0, ptr(r1, 16)));
  TEST_INSTRUCTION("0080FFFB", std(r31, ptr(r31, -32768)));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_extended(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("2000804E", blr());
  TEST_INSTRUCTION("A602897D", mfctr(r12));
  TEST_INSTRUCTION("A602087C", mflr(r0));
  TEST_INSTRUCTION("A603897D", mtctr(r12));
  TEST_INSTRUCTION("A603087C", mtlr(r0));
  TEST_INSTRUCTION("00000060", nop());
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
