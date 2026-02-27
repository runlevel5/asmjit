// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_PPC_PPC64GLOBALS_H_INCLUDED
#define ASMJIT_PPC_PPC64GLOBALS_H_INCLUDED

#include <asmjit/core/globals.h>
#include <asmjit/core/inst.h>

//! \namespace asmjit::ppc
//! \ingroup asmjit_ppc64
//!
//! PPC64 backend.

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

//! \addtogroup asmjit_ppc64
//! \{

//! PPC64 instruction.
//!
//! \note Only used to hold PPC-specific enumerations and static functions.
namespace Inst {
  //! Instruction id.
  //!
  //! \note The order must match the instruction list in `ppc64instdb.cpp`.
  enum Id : uint32_t {
    kIdNone = 0,                         //!< Instruction ''.
    kIdAdd,                              //!< Instruction 'add'.
    kIdAddi,                             //!< Instruction 'addi'.
    kIdAddis,                            //!< Instruction 'addis'.
    kIdBclr,                             //!< Instruction 'bclr'.
    kIdLd,                               //!< Instruction 'ld'.
    kIdMfspr,                            //!< Instruction 'mfspr'.
    kIdMtspr,                            //!< Instruction 'mtspr'.
    kIdOri,                              //!< Instruction 'ori'.
    kIdStd,                              //!< Instruction 'std'.
    _kIdCount
  };

  //! Tests whether the `inst_id` is defined (counts also Inst::kIdNone, which must be zero).
  [[nodiscard]]
  static ASMJIT_INLINE_NODEBUG bool is_defined_id(InstId inst_id) noexcept { return inst_id < _kIdCount; }
}

//! Special purpose register ids used by `mfspr` and `mtspr` instructions.
namespace SPR {
  //! Special purpose register id.
  enum Id : uint32_t {
    //! Fixed-point exception register.
    kXER = 1,
    //! Link register.
    kLR = 8,
    //! Count register.
    kCTR = 9
  };
}

//! \}

ASMJIT_END_SUB_NAMESPACE

#endif // ASMJIT_PPC_PPC64GLOBALS_H_INCLUDED
