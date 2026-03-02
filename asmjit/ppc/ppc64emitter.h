// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_PPC_PPC64EMITTER_H_INCLUDED
#define ASMJIT_PPC_PPC64EMITTER_H_INCLUDED

#include <asmjit/core/emitter.h>
#include <asmjit/support/support.h>
#include <asmjit/ppc/ppc64globals.h>
#include <asmjit/ppc/ppc64operand.h>

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

#define ASMJIT_INST_0x(NAME, ID) \
  inline Error NAME() { return _emitter()->_emitI(Inst::kId##ID); }

#define ASMJIT_INST_1x(NAME, ID, T0) \
  inline Error NAME(const T0& o0) { return _emitter()->_emitI(Inst::kId##ID, o0); }

#define ASMJIT_INST_2x(NAME, ID, T0, T1) \
  inline Error NAME(const T0& o0, const T1& o1) { return _emitter()->_emitI(Inst::kId##ID, o0, o1); }

#define ASMJIT_INST_3x(NAME, ID, T0, T1, T2) \
  inline Error NAME(const T0& o0, const T1& o1, const T2& o2) { return _emitter()->_emitI(Inst::kId##ID, o0, o1, o2); }

//! \addtogroup asmjit_ppc64
//! \{

//! PPC64 emitter.
//!
//! NOTE: This class cannot be instantiated, you can only cast to it and use it as emitter that emits to
//! \ref Assembler.
template<typename This>
struct EmitterExplicitT {
  //! \cond

  // These two are unfortunately reported by the sanitizer. We know what we do, however, the sanitizer doesn't.
  // I have tried to use reinterpret_cast instead, but that would generate bad code when compiled by MSC.
  ASMJIT_ATTRIBUTE_NO_SANITIZE_UNDEF ASMJIT_INLINE_NODEBUG This* _emitter() noexcept { return static_cast<This*>(this); }
  ASMJIT_ATTRIBUTE_NO_SANITIZE_UNDEF ASMJIT_INLINE_NODEBUG const This* _emitter() const noexcept { return static_cast<const This*>(this); }

  //! \endcond

  //! \name General Purpose Instructions
  //! \{

  ASMJIT_INST_3x(add, Add, Gp, Gp, Gp)
  ASMJIT_INST_3x(add_rc, Add_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(addc, Addc, Gp, Gp, Gp)
  ASMJIT_INST_3x(addc_rc, Addc_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(addco, Addco, Gp, Gp, Gp)
  ASMJIT_INST_3x(addco_rc, Addco_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(adde, Adde, Gp, Gp, Gp)
  ASMJIT_INST_3x(adde_rc, Adde_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(addeo, Addeo, Gp, Gp, Gp)
  ASMJIT_INST_3x(addeo_rc, Addeo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(addi, Addi, Gp, Gp, Imm)
  ASMJIT_INST_3x(addic, Addic, Gp, Gp, Imm)
  ASMJIT_INST_3x(addic_rc, Addic_rc, Gp, Gp, Imm)
  ASMJIT_INST_3x(addis, Addis, Gp, Gp, Imm)
  ASMJIT_INST_2x(addme, Addme, Gp, Gp)
  ASMJIT_INST_2x(addme_rc, Addme_rc, Gp, Gp)
  ASMJIT_INST_2x(addmeo, Addmeo, Gp, Gp)
  ASMJIT_INST_2x(addmeo_rc, Addmeo_rc, Gp, Gp)
  ASMJIT_INST_3x(addo, Addo, Gp, Gp, Gp)
  ASMJIT_INST_3x(addo_rc, Addo_rc, Gp, Gp, Gp)
  ASMJIT_INST_2x(addze, Addze, Gp, Gp)
  ASMJIT_INST_2x(addze_rc, Addze_rc, Gp, Gp)
  ASMJIT_INST_2x(addzeo, Addzeo, Gp, Gp)
  ASMJIT_INST_2x(addzeo_rc, Addzeo_rc, Gp, Gp)
  ASMJIT_INST_3x(bclr, Bclr, Imm, Imm, Imm)
  ASMJIT_INST_3x(divd, Divd, Gp, Gp, Gp)
  ASMJIT_INST_3x(divd_rc, Divd_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divdo, Divdo, Gp, Gp, Gp)
  ASMJIT_INST_3x(divdo_rc, Divdo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divdu, Divdu, Gp, Gp, Gp)
  ASMJIT_INST_3x(divdu_rc, Divdu_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divduo, Divduo, Gp, Gp, Gp)
  ASMJIT_INST_3x(divduo_rc, Divduo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divw, Divw, Gp, Gp, Gp)
  ASMJIT_INST_3x(divw_rc, Divw_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwo, Divwo, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwo_rc, Divwo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwu, Divwu, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwu_rc, Divwu_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwuo, Divwuo, Gp, Gp, Gp)
  ASMJIT_INST_3x(divwuo_rc, Divwuo_rc, Gp, Gp, Gp)
  ASMJIT_INST_2x(ld, Ld, Gp, Mem)
  ASMJIT_INST_2x(li, Li, Gp, Imm)
  ASMJIT_INST_2x(lis, Lis, Gp, Imm)
  ASMJIT_INST_2x(mfspr, Mfspr, Gp, Imm)
  ASMJIT_INST_3x(modsd, Modsd, Gp, Gp, Gp)
  ASMJIT_INST_3x(modsw, Modsw, Gp, Gp, Gp)
  ASMJIT_INST_3x(modud, Modud, Gp, Gp, Gp)
  ASMJIT_INST_3x(moduw, Moduw, Gp, Gp, Gp)
  ASMJIT_INST_2x(mtspr, Mtspr, Imm, Gp)
  ASMJIT_INST_3x(mulhd, Mulhd, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhd_rc, Mulhd_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhdu, Mulhdu, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhdu_rc, Mulhdu_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhw, Mulhw, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhw_rc, Mulhw_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhwu, Mulhwu, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulhwu_rc, Mulhwu_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulld, Mulld, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulld_rc, Mulld_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulldo, Mulldo, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulldo_rc, Mulldo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mulli, Mulli, Gp, Gp, Imm)
  ASMJIT_INST_3x(mullw, Mullw, Gp, Gp, Gp)
  ASMJIT_INST_3x(mullw_rc, Mullw_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(mullwo, Mullwo, Gp, Gp, Gp)
  ASMJIT_INST_3x(mullwo_rc, Mullwo_rc, Gp, Gp, Gp)
  ASMJIT_INST_2x(neg, Neg, Gp, Gp)
  ASMJIT_INST_2x(neg_rc, Neg_rc, Gp, Gp)
  ASMJIT_INST_2x(nego, Nego, Gp, Gp)
  ASMJIT_INST_2x(nego_rc, Nego_rc, Gp, Gp)
  ASMJIT_INST_3x(ori, Ori, Gp, Gp, Imm)
  ASMJIT_INST_2x(std, Std, Gp, Mem)
  ASMJIT_INST_3x(subf, Subf, Gp, Gp, Gp)
  ASMJIT_INST_3x(subf_rc, Subf_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfc, Subfc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfc_rc, Subfc_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfco, Subfco, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfco_rc, Subfco_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfe, Subfe, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfe_rc, Subfe_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfeo, Subfeo, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfeo_rc, Subfeo_rc, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfic, Subfic, Gp, Gp, Imm)
  ASMJIT_INST_2x(subfme, Subfme, Gp, Gp)
  ASMJIT_INST_2x(subfme_rc, Subfme_rc, Gp, Gp)
  ASMJIT_INST_2x(subfmeo, Subfmeo, Gp, Gp)
  ASMJIT_INST_2x(subfmeo_rc, Subfmeo_rc, Gp, Gp)
  ASMJIT_INST_3x(subfo, Subfo, Gp, Gp, Gp)
  ASMJIT_INST_3x(subfo_rc, Subfo_rc, Gp, Gp, Gp)
  ASMJIT_INST_2x(subfze, Subfze, Gp, Gp)
  ASMJIT_INST_2x(subfze_rc, Subfze_rc, Gp, Gp)
  ASMJIT_INST_2x(subfzeo, Subfzeo, Gp, Gp)
  ASMJIT_INST_2x(subfzeo_rc, Subfzeo_rc, Gp, Gp)

  //! \}

  //! \name Extended Mnemonics
  //!
  //! Each of these emits the base instruction it's an alias of.
  //!
  //! \{

  //! Emits `bclr 20, 0, 0` (branch unconditionally to the address in the link register).
  inline Error blr() { return bclr(Imm(20), Imm(0), Imm(0)); }

  //! Emits `mfspr rt, CTR`.
  inline Error mfctr(const Gp& rt) { return mfspr(rt, Imm(SPR::kCTR)); }
  //! Emits `mfspr rt, LR`.
  inline Error mflr(const Gp& rt) { return mfspr(rt, Imm(SPR::kLR)); }
  //! Emits `mtspr CTR, rs`.
  inline Error mtctr(const Gp& rs) { return mtspr(Imm(SPR::kCTR), rs); }
  //! Emits `mtspr LR, rs`.
  inline Error mtlr(const Gp& rs) { return mtspr(Imm(SPR::kLR), rs); }

  //! Emits `ori r0, r0, 0`.
  inline Error nop() { return ori(regs::r0, regs::r0, Imm(0)); }

  //! Emits `subf rt, rb, ra` (rt = ra - rb).
  inline Error sub(const Gp& rt, const Gp& ra, const Gp& rb) { return subf(rt, rb, ra); }
  //! Emits `subf. rt, rb, ra` (rt = ra - rb).
  inline Error sub_rc(const Gp& rt, const Gp& ra, const Gp& rb) { return subf_rc(rt, rb, ra); }
  //! Emits `subfo rt, rb, ra` (rt = ra - rb).
  inline Error subo(const Gp& rt, const Gp& ra, const Gp& rb) { return subfo(rt, rb, ra); }
  //! Emits `subfo. rt, rb, ra` (rt = ra - rb).
  inline Error subo_rc(const Gp& rt, const Gp& ra, const Gp& rb) { return subfo_rc(rt, rb, ra); }

  //! \}
};

//! Emitter (PPC64).
//!
//! \note This class cannot be instantiated, you can only cast to it and use it as emitter that emits to
//! `ppc::Assembler`.
class Emitter : public BaseEmitter, public EmitterExplicitT<Emitter> {
  ASMJIT_NONCONSTRUCTIBLE(Emitter)
};

//! \}

#undef ASMJIT_INST_0x
#undef ASMJIT_INST_1x
#undef ASMJIT_INST_2x
#undef ASMJIT_INST_3x

ASMJIT_END_SUB_NAMESPACE

#endif // ASMJIT_PPC_PPC64EMITTER_H_INCLUDED
