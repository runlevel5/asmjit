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

// Tests a sequence of instructions, which is what instructions that use labels require.
template<typename Fn>
static bool test_sequence(AssemblerTester<ppc::Assembler>& tester, const char* s, const char* expected_opcode, Fn&& fn) noexcept {
  return tester.test_valid_instruction(s, expected_opcode, fn(tester.assembler));
}

// Tests a single instruction word of a sequence that is too long to be compared as a whole.
template<typename Fn>
static bool test_word_at(AssemblerTester<ppc::Assembler>& tester, const char* s, size_t offset, const char* expected_opcode, Fn&& fn) noexcept {
  Error err = fn(tester.assembler);
  Section* text = tester.code.text_section();

  String encoded_opcode;
  if (err == Error::kOk && offset + 4u <= text->buffer_size()) {
    encoded_opcode.append_hex(text->data() + offset, 4);
  }

  tester.count++;
  bool ok = err == Error::kOk && encoded_opcode == expected_opcode;

  if (ok) {
    tester.passed++;
    if (tester.settings.verbose) {
      printf("  OK [%s] <- %s\n", encoded_opcode.data(), s);
    }
  }
  else {
    printf("  !! [%s] <- %s <%s>\n"
           "     [%s] (Expected)\n", encoded_opcode.data(), s, DebugUtils::error_as_string(err), expected_opcode);
  }

  tester.prepare();
  return ok;
}

template<typename Fn>
static bool test_invalid_sequence(AssemblerTester<ppc::Assembler>& tester, const char* s, Error expected_error, Fn&& fn) noexcept {
  return tester.test_invalid_instruction(s, expected_error, fn(tester.assembler));
}

// Emits `count` nops.
static Error emit_nops(ppc::Assembler& a, size_t count) noexcept {
  for (size_t i = 0; i < count; i++) {
    ASMJIT_PROPAGATE(a.nop());
  }
  return Error::kOk;
}

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
  TEST_INSTRUCTION("3828837C", and_(r3, r4, r5));
  TEST_INSTRUCTION("38081F7C", and_(r31, r0, r1));
  TEST_INSTRUCTION("3940E67C", and_rc(r6, r7, r8));
  TEST_INSTRUCTION("7828837C", andc(r3, r4, r5));
  TEST_INSTRUCTION("78081F7C", andc(r31, r0, r1));
  TEST_INSTRUCTION("7940E67C", andc_rc(r6, r7, r8));
  TEST_INSTRUCTION("FFFF8370", andi_rc(r3, r4, 65535));
  TEST_INSTRUCTION("0100E073", andi_rc(r0, r31, 1));
  TEST_INSTRUCTION("FFFF8374", andis_rc(r3, r4, 65535));
  TEST_INSTRUCTION("0100E077", andis_rc(r0, r31, 1));
  TEST_INSTRUCTION("2004804E", bcctr(20, 0, 0));
  TEST_INSTRUCTION("2004824D", bcctr(12, 2, 0));
  TEST_INSTRUCTION("200C9E4C", bcctr(4, 30, 1));
  TEST_INSTRUCTION("2104804E", bcctrl(20, 0, 0));
  TEST_INSTRUCTION("2104824D", bcctrl(12, 2, 0));
  TEST_INSTRUCTION("2000804E", bclr(20, 0, 0));
  TEST_INSTRUCTION("2000824D", bclr(12, 2, 0));
  TEST_INSTRUCTION("20089E4C", bclr(4, 30, 1));
  TEST_INSTRUCTION("2100804E", bclrl(20, 0, 0));
  TEST_INSTRUCTION("2100824D", bclrl(12, 2, 0));
  TEST_INSTRUCTION("0020237C", cmp(cr0, 1, r3, r4));
  TEST_INSTRUCTION("00009F7F", cmp(cr7, 0, r31, r0));
  TEST_INSTRUCTION("00F8A07C", cmp(cr1, 1, r0, r31));
  TEST_INSTRUCTION("0100232C", cmpi(cr0, 1, r3, 1));
  TEST_INSTRUCTION("00809F2F", cmpi(cr7, 0, r31, -32768));
  TEST_INSTRUCTION("FF7FA02C", cmpi(cr1, 1, r0, 32767));
  TEST_INSTRUCTION("4020237C", cmpl(cr0, 1, r3, r4));
  TEST_INSTRUCTION("40009F7F", cmpl(cr7, 0, r31, r0));
  TEST_INSTRUCTION("40F8A07C", cmpl(cr1, 1, r0, r31));
  TEST_INSTRUCTION("01002328", cmpli(cr0, 1, r3, 1));
  TEST_INSTRUCTION("FFFF9F2B", cmpli(cr7, 0, r31, 65535));
  TEST_INSTRUCTION("0080A028", cmpli(cr1, 1, r0, 32768));
  TEST_INSTRUCTION("7400837C", cntlzd(r3, r4));
  TEST_INSTRUCTION("74001F7C", cntlzd(r31, r0));
  TEST_INSTRUCTION("7500E67C", cntlzd_rc(r6, r7));
  TEST_INSTRUCTION("3400837C", cntlzw(r3, r4));
  TEST_INSTRUCTION("34001F7C", cntlzw(r31, r0));
  TEST_INSTRUCTION("3500E67C", cntlzw_rc(r6, r7));
  TEST_INSTRUCTION("7404837C", cnttzd(r3, r4));
  TEST_INSTRUCTION("74041F7C", cnttzd(r31, r0));
  TEST_INSTRUCTION("7504E67C", cnttzd_rc(r6, r7));
  TEST_INSTRUCTION("3404837C", cnttzw(r3, r4));
  TEST_INSTRUCTION("34041F7C", cnttzw(r31, r0));
  TEST_INSTRUCTION("3504E67C", cnttzw_rc(r6, r7));
  TEST_INSTRUCTION("0212014C", crand(0, 1, 2));
  TEST_INSTRUCTION("02EAFE4F", crand(31, 30, 29));
  TEST_INSTRUCTION("0232424C", crand(2, 2, 6));
  TEST_INSTRUCTION("0211014C", crandc(0, 1, 2));
  TEST_INSTRUCTION("02E9FE4F", crandc(31, 30, 29));
  TEST_INSTRUCTION("0231424C", crandc(2, 2, 6));
  TEST_INSTRUCTION("4212014C", creqv(0, 1, 2));
  TEST_INSTRUCTION("42EAFE4F", creqv(31, 30, 29));
  TEST_INSTRUCTION("4232424C", creqv(2, 2, 6));
  TEST_INSTRUCTION("C211014C", crnand(0, 1, 2));
  TEST_INSTRUCTION("C2E9FE4F", crnand(31, 30, 29));
  TEST_INSTRUCTION("C231424C", crnand(2, 2, 6));
  TEST_INSTRUCTION("4210014C", crnor(0, 1, 2));
  TEST_INSTRUCTION("42E8FE4F", crnor(31, 30, 29));
  TEST_INSTRUCTION("4230424C", crnor(2, 2, 6));
  TEST_INSTRUCTION("8213014C", cror(0, 1, 2));
  TEST_INSTRUCTION("82EBFE4F", cror(31, 30, 29));
  TEST_INSTRUCTION("8233424C", cror(2, 2, 6));
  TEST_INSTRUCTION("4213014C", crorc(0, 1, 2));
  TEST_INSTRUCTION("42EBFE4F", crorc(31, 30, 29));
  TEST_INSTRUCTION("4233424C", crorc(2, 2, 6));
  TEST_INSTRUCTION("8211014C", crxor(0, 1, 2));
  TEST_INSTRUCTION("82E9FE4F", crxor(31, 30, 29));
  TEST_INSTRUCTION("8231424C", crxor(2, 2, 6));
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
  TEST_INSTRUCTION("382A837C", eqv(r3, r4, r5));
  TEST_INSTRUCTION("380A1F7C", eqv(r31, r0, r1));
  TEST_INSTRUCTION("3942E67C", eqv_rc(r6, r7, r8));
  TEST_INSTRUCTION("7407837C", extsb(r3, r4));
  TEST_INSTRUCTION("74071F7C", extsb(r31, r0));
  TEST_INSTRUCTION("7507E67C", extsb_rc(r6, r7));
  TEST_INSTRUCTION("3407837C", extsh(r3, r4));
  TEST_INSTRUCTION("34071F7C", extsh(r31, r0));
  TEST_INSTRUCTION("3507E67C", extsh_rc(r6, r7));
  TEST_INSTRUCTION("B407837C", extsw(r3, r4));
  TEST_INSTRUCTION("B4071F7C", extsw(r31, r0));
  TEST_INSTRUCTION("B507E67C", extsw_rc(r6, r7));
  TEST_INSTRUCTION("9E28647C", isel(r3, r4, r5, 2));
  TEST_INSTRUCTION("DE07E17F", isel(r31, r1, r0, 31));
  TEST_INSTRUCTION("1EF01F7C", isel(r0, r31, r30, 0));
  TEST_INSTRUCTION("6828647C", lbarx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6800FF7F", lbarx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("68F8017C", lbarx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("00006488", lbz(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF6488", lbz(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFF8B", lbz(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("00800188", lbz(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("0800648C", lbzu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E18F", lbzu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1F8C", lbzu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("EE28647C", lbzux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("EE00E17F", lbzux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("EEF01F7C", lbzux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("AE28647C", lbzx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AE00FF7F", lbzx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("AEF8017C", lbzx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("000064E8", ld(r3, ptr(r4)));
  TEST_INSTRUCTION("080064E8", ld(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("008061E8", ld(r3, ptr(r1, -32768)));
  TEST_INSTRUCTION("FC7FFFEB", ld(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("A828647C", ldarx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("A800FF7F", ldarx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("A8F8017C", ldarx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("282C647C", ldbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2804FF7F", ldbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("28FC017C", ldbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("090064E8", ldu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0180E1EB", ldu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FD7F1FE8", ldu(r0, ptr(r31, 32764)));
  TEST_INSTRUCTION("6A28647C", ldux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6A00E17F", ldux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6AF01F7C", ldux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2A28647C", ldx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2A00FF7F", ldx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2AF8017C", ldx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("000024C8", lfd(f1, ptr(r4)));
  TEST_INSTRUCTION("FFFFFFCB", lfd(f31, ptr(r31, -1)));
  TEST_INSTRUCTION("FF7F01C8", lfd(f0, ptr(r1, 32767)));
  TEST_INSTRUCTION("AE2C247C", lfdx(f1, ptr(r4, r5)));
  TEST_INSTRUCTION("AE04FF7F", lfdx(f31, ptr(r31, r0)));
  TEST_INSTRUCTION("000024C0", lfs(f1, ptr(r4)));
  TEST_INSTRUCTION("FFFFFFC3", lfs(f31, ptr(r31, -1)));
  TEST_INSTRUCTION("FF7F01C0", lfs(f0, ptr(r1, 32767)));
  TEST_INSTRUCTION("2E2C247C", lfsx(f1, ptr(r4, r5)));
  TEST_INSTRUCTION("2E04FF7F", lfsx(f31, ptr(r31, r0)));
  TEST_INSTRUCTION("000064A8", lha(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF64A8", lha(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFFAB", lha(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("008001A8", lha(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("E828647C", lharx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("E800FF7F", lharx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("E8F8017C", lharx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("080064AC", lhau(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E1AF", lhau(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1FAC", lhau(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("EE2A647C", lhaux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("EE02E17F", lhaux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("EEF21F7C", lhaux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("AE2A647C", lhax(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AE02FF7F", lhax(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("AEFA017C", lhax(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("2C2E647C", lhbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2C06FF7F", lhbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2CFE017C", lhbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("000064A0", lhz(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF64A0", lhz(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFFA3", lhz(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("008001A0", lhz(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("080064A4", lhzu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E1A7", lhzu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1FA4", lhzu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("6E2A647C", lhzux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6E02E17F", lhzux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6EF21F7C", lhzux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2E2A647C", lhzx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2E02FF7F", lhzx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2EFA017C", lhzx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("01006038", li(r3, 1));
  TEST_INSTRUCTION("0080E03B", li(r31, -32768));
  TEST_INSTRUCTION("FF7F0038", li(r0, 32767));
  TEST_INSTRUCTION("0100603C", lis(r3, 1));
  TEST_INSTRUCTION("0080E03F", lis(r31, -32768));
  TEST_INSTRUCTION("FF7F003C", lis(r0, 32767));
  TEST_INSTRUCTION("020064E8", lwa(r3, ptr(r4)));
  TEST_INSTRUCTION("0A0064E8", lwa(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("028001E8", lwa(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("FE7FFFEB", lwa(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("2828647C", lwarx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2800FF7F", lwarx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("28F8017C", lwarx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("EA2A647C", lwaux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("EA02E17F", lwaux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("EAF21F7C", lwaux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("AA2A647C", lwax(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AA02FF7F", lwax(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("AAFA017C", lwax(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("2C2C647C", lwbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2C04FF7F", lwbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2CFC017C", lwbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("00006480", lwz(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF6480", lwz(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFF83", lwz(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("00800180", lwz(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("08006484", lwzu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E187", lwzu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1F84", lwzu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("6E28647C", lwzux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6E00E17F", lwzux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6EF01F7C", lwzux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2E28647C", lwzx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2E00FF7F", lwzx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2EF8017C", lwzx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("0000044C", mcrf(cr0, cr1));
  TEST_INSTRUCTION("0000984F", mcrf(cr7, cr6));
  TEST_INSTRUCTION("8004007C", mcrxrx(cr0));
  TEST_INSTRUCTION("8004807F", mcrxrx(cr7));
  TEST_INSTRUCTION("2600607C", mfcr(r3));
  TEST_INSTRUCTION("2600E07F", mfcr(r31));
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
  TEST_INSTRUCTION("B82B837C", nand(r3, r4, r5));
  TEST_INSTRUCTION("B80B1F7C", nand(r31, r0, r1));
  TEST_INSTRUCTION("B943E67C", nand_rc(r6, r7, r8));
  TEST_INSTRUCTION("D000647C", neg(r3, r4));
  TEST_INSTRUCTION("D000E07F", neg(r31, r0));
  TEST_INSTRUCTION("D100C77C", neg_rc(r6, r7));
  TEST_INSTRUCTION("D004C77C", nego(r6, r7));
  TEST_INSTRUCTION("D104C77C", nego_rc(r6, r7));
  TEST_INSTRUCTION("F828837C", nor(r3, r4, r5));
  TEST_INSTRUCTION("F8081F7C", nor(r31, r0, r1));
  TEST_INSTRUCTION("F940E67C", nor_rc(r6, r7, r8));
  TEST_INSTRUCTION("782B837C", or_(r3, r4, r5));
  TEST_INSTRUCTION("780B1F7C", or_(r31, r0, r1));
  TEST_INSTRUCTION("7943E67C", or_rc(r6, r7, r8));
  TEST_INSTRUCTION("382B837C", orc(r3, r4, r5));
  TEST_INSTRUCTION("380B1F7C", orc(r31, r0, r1));
  TEST_INSTRUCTION("3943E67C", orc_rc(r6, r7, r8));
  TEST_INSTRUCTION("FFFF8360", ori(r3, r4, 65535));
  TEST_INSTRUCTION("0100DF63", ori(r31, r30, 1));
  TEST_INSTRUCTION("FFFF8364", oris(r3, r4, 65535));
  TEST_INSTRUCTION("0100E067", oris(r0, r31, 1));
  TEST_INSTRUCTION("F400837C", popcntb(r3, r4));
  TEST_INSTRUCTION("F4001F7C", popcntb(r31, r0));
  TEST_INSTRUCTION("F403837C", popcntd(r3, r4));
  TEST_INSTRUCTION("F4031F7C", popcntd(r31, r0));
  TEST_INSTRUCTION("F402837C", popcntw(r3, r4));
  TEST_INSTRUCTION("F4021F7C", popcntw(r31, r0));
  TEST_INSTRUCTION("90288378", rldcl(r3, r4, r5, 2));
  TEST_INSTRUCTION("F00F1F78", rldcl(r31, r0, r1, 63));
  TEST_INSTRUCTION("3040E678", rldcl(r6, r7, r8, 32));
  TEST_INSTRUCTION("7140E678", rldcl_rc(r6, r7, r8, 33));
  TEST_INSTRUCTION("92288378", rldcr(r3, r4, r5, 2));
  TEST_INSTRUCTION("F20F1F78", rldcr(r31, r0, r1, 63));
  TEST_INSTRUCTION("3240E678", rldcr(r6, r7, r8, 32));
  TEST_INSTRUCTION("7340E678", rldcr_rc(r6, r7, r8, 33));
  TEST_INSTRUCTION("88088378", rldic(r3, r4, 1, 2));
  TEST_INSTRUCTION("6AF81F78", rldic(r31, r0, 63, 33));
  TEST_INSTRUCTION("EA07C578", rldic(r5, r6, 32, 63));
  TEST_INSTRUCTION("CB0FE678", rldic_rc(r6, r7, 33, 31));
  TEST_INSTRUCTION("80088378", rldicl(r3, r4, 1, 2));
  TEST_INSTRUCTION("62F81F78", rldicl(r31, r0, 63, 33));
  TEST_INSTRUCTION("E207C578", rldicl(r5, r6, 32, 63));
  TEST_INSTRUCTION("C30FE678", rldicl_rc(r6, r7, 33, 31));
  TEST_INSTRUCTION("84088378", rldicr(r3, r4, 1, 2));
  TEST_INSTRUCTION("66F81F78", rldicr(r31, r0, 63, 33));
  TEST_INSTRUCTION("E607C578", rldicr(r5, r6, 32, 63));
  TEST_INSTRUCTION("C70FE678", rldicr_rc(r6, r7, 33, 31));
  TEST_INSTRUCTION("8C088378", rldimi(r3, r4, 1, 2));
  TEST_INSTRUCTION("6EF81F78", rldimi(r31, r0, 63, 33));
  TEST_INSTRUCTION("EE07C578", rldimi(r5, r6, 32, 63));
  TEST_INSTRUCTION("CF0FE678", rldimi_rc(r6, r7, 33, 31));
  TEST_INSTRUCTION("86088350", rlwimi(r3, r4, 1, 2, 3));
  TEST_INSTRUCTION("BAFF1F50", rlwimi(r31, r0, 31, 30, 29));
  TEST_INSTRUCTION("3144E650", rlwimi_rc(r6, r7, 8, 16, 24));
  TEST_INSTRUCTION("86088354", rlwinm(r3, r4, 1, 2, 3));
  TEST_INSTRUCTION("BAFF1F54", rlwinm(r31, r0, 31, 30, 29));
  TEST_INSTRUCTION("3144E654", rlwinm_rc(r6, r7, 8, 16, 24));
  TEST_INSTRUCTION("8628835C", rlwnm(r3, r4, r5, 2, 3));
  TEST_INSTRUCTION("FC0F1F5C", rlwnm(r31, r0, r1, 31, 30));
  TEST_INSTRUCTION("3144E65C", rlwnm_rc(r6, r7, r8, 16, 24));
  TEST_INSTRUCTION("0001607C", setb(r3, cr0));
  TEST_INSTRUCTION("0001FC7F", setb(r31, cr7));
  TEST_INSTRUCTION("3628837C", sld(r3, r4, r5));
  TEST_INSTRUCTION("36081F7C", sld(r31, r0, r1));
  TEST_INSTRUCTION("3740E67C", sld_rc(r6, r7, r8));
  TEST_INSTRUCTION("3028837C", slw(r3, r4, r5));
  TEST_INSTRUCTION("30081F7C", slw(r31, r0, r1));
  TEST_INSTRUCTION("3140E67C", slw_rc(r6, r7, r8));
  TEST_INSTRUCTION("342E837C", srad(r3, r4, r5));
  TEST_INSTRUCTION("340E1F7C", srad(r31, r0, r1));
  TEST_INSTRUCTION("3546E67C", srad_rc(r6, r7, r8));
  TEST_INSTRUCTION("740E837C", sradi(r3, r4, 1));
  TEST_INSTRUCTION("76FE1F7C", sradi(r31, r0, 63));
  TEST_INSTRUCTION("7606C57C", sradi(r5, r6, 32));
  TEST_INSTRUCTION("770EE67C", sradi_rc(r6, r7, 33));
  TEST_INSTRUCTION("302E837C", sraw(r3, r4, r5));
  TEST_INSTRUCTION("300E1F7C", sraw(r31, r0, r1));
  TEST_INSTRUCTION("3146E67C", sraw_rc(r6, r7, r8));
  TEST_INSTRUCTION("700E837C", srawi(r3, r4, 1));
  TEST_INSTRUCTION("70FE1F7C", srawi(r31, r0, 31));
  TEST_INSTRUCTION("718EE67C", srawi_rc(r6, r7, 17));
  TEST_INSTRUCTION("362C837C", srd(r3, r4, r5));
  TEST_INSTRUCTION("360C1F7C", srd(r31, r0, r1));
  TEST_INSTRUCTION("3744E67C", srd_rc(r6, r7, r8));
  TEST_INSTRUCTION("302C837C", srw(r3, r4, r5));
  TEST_INSTRUCTION("300C1F7C", srw(r31, r0, r1));
  TEST_INSTRUCTION("3144E67C", srw_rc(r6, r7, r8));
  TEST_INSTRUCTION("00006498", stb(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF6498", stb(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFF9B", stb(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("00800198", stb(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("6D2D647C", stbcx_rc(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6D05FF7F", stbcx_rc(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("6DFD017C", stbcx_rc(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("0800649C", stbu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E19F", stbu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1F9C", stbu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("EE29647C", stbux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("EE01E17F", stbux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("EEF11F7C", stbux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("AE29647C", stbx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AE01FF7F", stbx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("AEF9017C", stbx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("000064F8", std(r3, ptr(r4)));
  TEST_INSTRUCTION("080064F8", std(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("100001F8", std(r0, ptr(r1, 16)));
  TEST_INSTRUCTION("0080FFFB", std(r31, ptr(r31, -32768)));
  TEST_INSTRUCTION("282D647C", stdbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2805FF7F", stdbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("28FD017C", stdbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("AD29647C", stdcx_rc(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AD01FF7F", stdcx_rc(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("ADF9017C", stdcx_rc(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("090064F8", stdu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0180E1FB", stdu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FD7F1FF8", stdu(r0, ptr(r31, 32764)));
  TEST_INSTRUCTION("6A29647C", stdux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6A01E17F", stdux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6AF11F7C", stdux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2A29647C", stdx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2A01FF7F", stdx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2AF9017C", stdx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("000024D8", stfd(f1, ptr(r4)));
  TEST_INSTRUCTION("FFFFFFDB", stfd(f31, ptr(r31, -1)));
  TEST_INSTRUCTION("FF7F01D8", stfd(f0, ptr(r1, 32767)));
  TEST_INSTRUCTION("AE2D247C", stfdx(f1, ptr(r4, r5)));
  TEST_INSTRUCTION("AE05FF7F", stfdx(f31, ptr(r31, r0)));
  TEST_INSTRUCTION("000024D0", stfs(f1, ptr(r4)));
  TEST_INSTRUCTION("FFFFFFD3", stfs(f31, ptr(r31, -1)));
  TEST_INSTRUCTION("FF7F01D0", stfs(f0, ptr(r1, 32767)));
  TEST_INSTRUCTION("2E2D247C", stfsx(f1, ptr(r4, r5)));
  TEST_INSTRUCTION("2E05FF7F", stfsx(f31, ptr(r31, r0)));
  TEST_INSTRUCTION("000064B0", sth(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF64B0", sth(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFFB3", sth(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("008001B0", sth(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("2C2F647C", sthbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2C07FF7F", sthbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2CFF017C", sthbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("AD2D647C", sthcx_rc(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("AD05FF7F", sthcx_rc(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("ADFD017C", sthcx_rc(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("080064B4", sthu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E1B7", sthu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1FB4", sthu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("6E2B647C", sthux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6E03E17F", sthux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6EF31F7C", sthux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2E2B647C", sthx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2E03FF7F", sthx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2EFB017C", sthx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("00006490", stw(r3, ptr(r4)));
  TEST_INSTRUCTION("FFFF6490", stw(r3, ptr(r4, -1)));
  TEST_INSTRUCTION("FF7FFF93", stw(r31, ptr(r31, 32767)));
  TEST_INSTRUCTION("00800190", stw(r0, ptr(r1, -32768)));
  TEST_INSTRUCTION("2C2D647C", stwbrx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2C05FF7F", stwbrx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2CFD017C", stwbrx(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("2D29647C", stwcx_rc(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2D01FF7F", stwcx_rc(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2DF9017C", stwcx_rc(r0, ptr(r1, r31)));
  TEST_INSTRUCTION("08006494", stwu(r3, ptr(r4, 8)));
  TEST_INSTRUCTION("0080E197", stwu(r31, ptr(r1, -32768)));
  TEST_INSTRUCTION("FF7F1F94", stwu(r0, ptr(r31, 32767)));
  TEST_INSTRUCTION("6E29647C", stwux(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("6E01E17F", stwux(r31, ptr(r1, r0)));
  TEST_INSTRUCTION("6EF11F7C", stwux(r0, ptr(r31, r30)));
  TEST_INSTRUCTION("2E29647C", stwx(r3, ptr(r4, r5)));
  TEST_INSTRUCTION("2E01FF7F", stwx(r31, ptr(r31, r0)));
  TEST_INSTRUCTION("2EF9017C", stwx(r0, ptr(r1, r31)));
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
  TEST_INSTRUCTION("782A837C", xor_(r3, r4, r5));
  TEST_INSTRUCTION("780A1F7C", xor_(r31, r0, r1));
  TEST_INSTRUCTION("7942E67C", xor_rc(r6, r7, r8));
  TEST_INSTRUCTION("FFFF8368", xori(r3, r4, 65535));
  TEST_INSTRUCTION("0100E06B", xori(r0, r31, 1));
  TEST_INSTRUCTION("FFFF836C", xoris(r3, r4, 65535));
  TEST_INSTRUCTION("0100E06F", xoris(r0, r31, 1));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_extended(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("2004804E", bctr());
  TEST_INSTRUCTION("2104804E", bctrl());
  TEST_INSTRUCTION("2000804E", blr());
  TEST_INSTRUCTION("2100804E", blrl());
  TEST_INSTRUCTION("20008378", clrldi(r3, r4, 32));
  TEST_INSTRUCTION("3E048354", clrlwi(r3, r4, 16));
  TEST_INSTRUCTION("64078378", clrrdi(r3, r4, 2));
  TEST_INSTRUCTION("0020A37F", cmpd(cr7, r3, r4));
  TEST_INSTRUCTION("0020237C", cmpd(r3, r4));
  TEST_INSTRUCTION("FBFFA32F", cmpdi(cr7, r3, -5));
  TEST_INSTRUCTION("FBFF232C", cmpdi(r3, -5));
  TEST_INSTRUCTION("4020A37F", cmpld(cr7, r3, r4));
  TEST_INSTRUCTION("4020237C", cmpld(r3, r4));
  TEST_INSTRUCTION("0F00A32B", cmpldi(cr7, r3, 15));
  TEST_INSTRUCTION("0F002328", cmpldi(r3, 15));
  TEST_INSTRUCTION("4020837F", cmplw(cr7, r3, r4));
  TEST_INSTRUCTION("4020037C", cmplw(r3, r4));
  TEST_INSTRUCTION("0F00832B", cmplwi(cr7, r3, 15));
  TEST_INSTRUCTION("0F000328", cmplwi(r3, 15));
  TEST_INSTRUCTION("0020837F", cmpw(cr7, r3, r4));
  TEST_INSTRUCTION("0020037C", cmpw(r3, r4));
  TEST_INSTRUCTION("FBFF832F", cmpwi(cr7, r3, -5));
  TEST_INSTRUCTION("FBFF032C", cmpwi(r3, -5));
  TEST_INSTRUCTION("0232424C", crand(cr0.eq(), cr0.eq(), cr1.eq()));
  TEST_INSTRUCTION("8231C64C", crclr(cr1.eq()));
  TEST_INSTRUCTION("8213C24C", crmove(cr1.eq(), cr0.eq()));
  TEST_INSTRUCTION("4210C24C", crnot(cr1.eq(), cr0.eq()));
  TEST_INSTRUCTION("4232C64C", crset(cr1.eq()));
  TEST_INSTRUCTION("1E28647C", isel(r3, r4, r5, cr0.lt()));
  TEST_INSTRUCTION("A602897D", mfctr(r12));
  TEST_INSTRUCTION("A602087C", mflr(r0));
  TEST_INSTRUCTION("A603897D", mtctr(r12));
  TEST_INSTRUCTION("7823837C", mr(r3, r4));
  TEST_INSTRUCTION("A603087C", mtlr(r0));
  TEST_INSTRUCTION("00000060", nop());
  TEST_INSTRUCTION("F820837C", not_(r3, r4));
  TEST_INSTRUCTION("00388378", rotldi(r3, r4, 7));
  TEST_INSTRUCTION("A42E8378", sldi(r3, r4, 5));
  TEST_INSTRUCTION("06F88378", sldi(r3, r4, 63));
  TEST_INSTRUCTION("34288354", slwi(r3, r4, 5));
  TEST_INSTRUCTION("42D98378", srdi(r3, r4, 5));
  TEST_INSTRUCTION("00008378", srdi(r3, r4, 0));
  TEST_INSTRUCTION("7ED98354", srwi(r3, r4, 5));
  TEST_INSTRUCTION("3E008354", srwi(r3, r4, 0));
  TEST_INSTRUCTION("5020657C", sub(r3, r4, r5));
  TEST_INSTRUCTION("5120657C", sub_rc(r3, r4, r5));
  TEST_INSTRUCTION("5024657C", subo(r3, r4, r5));
  TEST_INSTRUCTION("5124657C", subo_rc(r3, r4, r5));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_rel(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  test_sequence(tester, "branches to a bound label", "00000060FCFFFF4BF9FFFF4BF4FF8241F1FF9E40", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bind(target));
    ASMJIT_PROPAGATE(a.nop());
    ASMJIT_PROPAGATE(a.b(target));
    ASMJIT_PROPAGATE(a.bl(target));
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    return a.bcl(4, 30, target);
  });

  test_sequence(tester, "branches to a label bound later", "14000048110000480C00824109009E4000000060", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.b(target));
    ASMJIT_PROPAGATE(a.bl(target));
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    ASMJIT_PROPAGATE(a.bcl(4, 30, target));
    ASMJIT_PROPAGATE(a.nop());
    return a.bind(target);
  });

  test_sequence(tester, "conditional branch extended mnemonics",
    "400082413C009E413800824034009E40300080412C009C412800804024009C40"
    "200081411C009D411800814014009D40100083410C009F410800834004009F40", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.beq(target));
    ASMJIT_PROPAGATE(a.beq(cr7, target));
    ASMJIT_PROPAGATE(a.bne(target));
    ASMJIT_PROPAGATE(a.bne(cr7, target));
    ASMJIT_PROPAGATE(a.blt(target));
    ASMJIT_PROPAGATE(a.blt(cr7, target));
    ASMJIT_PROPAGATE(a.bge(target));
    ASMJIT_PROPAGATE(a.bge(cr7, target));
    ASMJIT_PROPAGATE(a.bgt(target));
    ASMJIT_PROPAGATE(a.bgt(cr7, target));
    ASMJIT_PROPAGATE(a.ble(target));
    ASMJIT_PROPAGATE(a.ble(cr7, target));
    ASMJIT_PROPAGATE(a.bso(target));
    ASMJIT_PROPAGATE(a.bso(cr7, target));
    ASMJIT_PROPAGATE(a.bns(target));
    ASMJIT_PROPAGATE(a.bns(cr7, target));
    return a.bind(target);
  });

  // A conditional branch reaches from -32768 to +32764 bytes.
  test_word_at(tester, "bc to a label 32768 bytes back", 32768, "00808241", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bind(target));
    ASMJIT_PROPAGATE(emit_nops(a, 8192));
    return a.bc(12, 2, target);
  });

  test_word_at(tester, "bc to a label 32764 bytes ahead", 0, "FC7F8241", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    ASMJIT_PROPAGATE(emit_nops(a, 8190));
    return a.bind(target);
  });

  test_invalid_sequence(tester, "bc to a label 32772 bytes back", Error::kInvalidDisplacement, [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bind(target));
    ASMJIT_PROPAGATE(emit_nops(a, 8193));
    return a.bc(12, 2, target);
  });

  test_invalid_sequence(tester, "bc to a label 32768 bytes ahead", Error::kInvalidDisplacement, [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    ASMJIT_PROPAGATE(emit_nops(a, 8191));
    return a.bind(target);
  });

  FAIL_INSTRUCTION(Error::kInvalidLabel, b(Label(1234)));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bc(32, 0, tester.L0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bc(12, 32, tester.L0));
}

static void ASMJIT_NOINLINE test_ppc64_assembler_invalid(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  // R0 would be read as zero.
  FAIL_INSTRUCTION(Error::kInvalidPhysId, addi(r3, r0, 1));
  FAIL_INSTRUCTION(Error::kInvalidPhysId, addis(r3, r0, 1));
  FAIL_INSTRUCTION(Error::kInvalidPhysId, isel(r3, r0, r5, cr0.eq()));
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
  FAIL_INSTRUCTION(Error::kInvalidImmediate, andi_rc(r3, r4, -1));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, srawi(r3, r4, 32));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, sradi(r3, r4, 64));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rldicl(r3, r4, 64, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rldicl(r3, r4, 0, 64));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rldcl(r3, r4, r5, 64));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rlwinm(r3, r4, 32, 0, 31));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rlwinm(r3, r4, 0, 32, 31));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rlwinm(r3, r4, 0, 0, 32));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, rlwnm(r3, r4, r5, 0, 32));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, sldi(r3, r4, 64));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, srdi(r3, r4, 65));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, slwi(r3, r4, 32));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, cmp(cr0, 2, r3, r4));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, cmpdi(r3, 32768));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, cmpldi(r3, -1));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, cmpldi(r3, 65536));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, crand(32, 0, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, isel(r3, r4, r5, 32));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(32, 0, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(20, 32, 0));
  FAIL_INSTRUCTION(Error::kInvalidImmediate, bclr(20, 0, 4));

  // Displacement must be a multiple of 4 that fits into 16 bits.
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, -1)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, 2)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, ld(r3, ptr(r4, 32768)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, std(r3, ptr(r4, -32772)));

  FAIL_INSTRUCTION(Error::kInvalidDisplacement, lwa(r3, ptr(r4, 2)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, lbz(r3, ptr(r4, 32768)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, stw(r3, ptr(r4, -32769)));
  FAIL_INSTRUCTION(Error::kInvalidDisplacement, lfd(f1, ptr(r4, 32768)));

  // These instructions only provide base + displacement addressing.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, ptr(r4, r5)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, ptr(tester.L0)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, lbz(r3, ptr(r4, r5)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, lfd(f1, ptr(r4, r5)));

  // These instructions only provide base + index addressing.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ldx(r3, ptr(r4)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, ldx(r3, ptr(r4, 8)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, stdx(r3, ptr(r4, 8)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, lfdx(f1, ptr(r4, 8)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, ldx(r3, ptr(tester.L0)));

  // R0 used as a base of an indexed address would be read as zero as well.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ldx(r3, ptr(r0, r5)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, lbz(r3, ptr(r0, 8)));
  FAIL_INSTRUCTION(Error::kInvalidAddress, stdu(r3, ptr(r0, 8)));

  // A load that updates its base register cannot load into that register.
  FAIL_INSTRUCTION(Error::kOverlappedRegs, lbzu(r3, ptr(r3, 8)));
  FAIL_INSTRUCTION(Error::kOverlappedRegs, ldu(r3, ptr(r3, 8)));
  FAIL_INSTRUCTION(Error::kOverlappedRegs, ldux(r3, ptr(r3, r5)));

  // There are only 8 condition register fields.
  FAIL_INSTRUCTION(Error::kInvalidPhysId, cmpd(cr(8), r3, r4));
  FAIL_INSTRUCTION(Error::kInvalidPhysId, mcrf(cr0, cr(8)));

  // Floating point register used as a general purpose register.
  FAIL_INSTRUCTION(Error::kInvalidAddress, ld(r3, Mem(f4, 8)));
}

// Expected bytes were produced by GNU assembler (`as -mpower9 -mbig`).
static void ASMJIT_NOINLINE test_ppc64_assembler_big_endian(AssemblerTester<ppc::Assembler>& tester) noexcept {
  using namespace ppc;

  TEST_INSTRUCTION("7C642A14", add(r3, r4, r5));
  TEST_INSTRUCTION("38618000", addi(r3, r1, -32768));
  TEST_INSTRUCTION("4E800020", blr());
  TEST_INSTRUCTION("7FA32000", cmpd(cr7, r3, r4));
  TEST_INSTRUCTION("4C423202", crand(cr0.eq(), cr0.eq(), cr1.eq()));
  TEST_INSTRUCTION("7C64281E", isel(r3, r4, r5, cr0.lt()));
  TEST_INSTRUCTION("EBFF7FFC", ld(r31, ptr(r31, 32764)));
  TEST_INSTRUCTION("7C0802A6", mflr(r0));
  TEST_INSTRUCTION("7C64602A", ldx(r3, ptr(r4, r12)));
  TEST_INSTRUCTION("C8240008", lfd(f1, ptr(r4, 8)));
  TEST_INSTRUCTION("7D8903A6", mtctr(r12));
  TEST_INSTRUCTION("7C643DD3", mulldo_rc(r3, r4, r7));
  TEST_INSTRUCTION("60000000", nop());
  TEST_INSTRUCTION("786407C6", sldi(r4, r3, 32));
  TEST_INSTRUCTION("F8010010", std(r0, ptr(r1, 16)));
  TEST_INSTRUCTION("F821FFA1", stdu(r1, ptr(r1, -96)));

  test_sequence(tester, "branches to a bound label", "600000004BFFFFFC4BFFFFF94182FFF4409EFFF1", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.bind(target));
    ASMJIT_PROPAGATE(a.nop());
    ASMJIT_PROPAGATE(a.b(target));
    ASMJIT_PROPAGATE(a.bl(target));
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    return a.bcl(4, 30, target);
  });

  test_sequence(tester, "branches to a label bound later", "48000014480000114182000C409E000960000000", [](Assembler& a) {
    Label target = a.new_label();
    ASMJIT_PROPAGATE(a.b(target));
    ASMJIT_PROPAGATE(a.bl(target));
    ASMJIT_PROPAGATE(a.bc(12, 2, target));
    ASMJIT_PROPAGATE(a.bcl(4, 30, target));
    ASMJIT_PROPAGATE(a.nop());
    return a.bind(target);
  });
}

bool test_ppc64_assembler(const TestSettings& settings) noexcept {
  using namespace ppc;

  AssemblerTester<Assembler> tester_le(Arch::kPPC64_LE, settings);
  tester_le.print_header("PPC64_LE");

  test_ppc64_assembler_base(tester_le);
  test_ppc64_assembler_extended(tester_le);
  test_ppc64_assembler_rel(tester_le);
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
