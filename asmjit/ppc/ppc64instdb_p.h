// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_PPC_PPC64INSTDB_P_H_INCLUDED
#define ASMJIT_PPC_PPC64INSTDB_P_H_INCLUDED

#include <asmjit/ppc/ppc64instdb.h>

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

//! \cond INTERNAL
//! \addtogroup asmjit_ppc64
//! \{

namespace InstDB {

//! Instruction encoding (PPC64).
//!
//! The encoding describes which operands the instruction accepts and where they are placed in the instruction
//! word. Operands are always passed in the order used by the assembly language.
enum EncodingId : uint32_t {
  //! Invalid instruction.
  kEncodingNone = 0,

  //! `inst RT, RA, RB` - three general purpose registers.
  kEncodingRtRaRb,
  //! `inst RT, RA, SI` - RA cannot be R0 (the instruction would read it as zero), SI is a signed 16-bit immediate.
  kEncodingRtRaSi,
  //! `inst RA, RS, UI` - UI is an unsigned 16-bit immediate.
  kEncodingRaRsUi,
  //! `inst RT|RS, DS(RA)` - load or store, DS is a signed 16-bit displacement that must be a multiple of 4.
  kEncodingRtMemDs,
  //! `inst RT, SPR` - SPR is a special purpose register number.
  kEncodingRtSpr,
  //! `inst SPR, RS` - SPR is a special purpose register number.
  kEncodingSprRs,
  //! `inst BO, BI, BH` - branch to LR or CTR, operands are immediates.
  kEncodingBoBiBh
};

} // {InstDB}

//! \}
//! \endcond

ASMJIT_END_SUB_NAMESPACE

#endif // ASMJIT_PPC_PPC64INSTDB_P_H_INCLUDED
