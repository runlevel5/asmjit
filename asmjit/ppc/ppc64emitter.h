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
  ASMJIT_INST_3x(addi, Addi, Gp, Gp, Imm)
  ASMJIT_INST_3x(addis, Addis, Gp, Gp, Imm)
  ASMJIT_INST_3x(bclr, Bclr, Imm, Imm, Imm)
  ASMJIT_INST_2x(ld, Ld, Gp, Mem)
  ASMJIT_INST_2x(mfspr, Mfspr, Gp, Imm)
  ASMJIT_INST_2x(mtspr, Mtspr, Imm, Gp)
  ASMJIT_INST_3x(ori, Ori, Gp, Gp, Imm)
  ASMJIT_INST_2x(std, Std, Gp, Mem)

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
