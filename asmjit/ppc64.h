// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef ASMJIT_PPC64_H_INCLUDED
#define ASMJIT_PPC64_H_INCLUDED

//! \addtogroup asmjit_ppc64
//!
//! \note The PPC64 backend is under construction, only operands are provided at the moment.
//!
//! ### Register Operands
//!
//!   - \ref ppc::Gp - General purpose register (64-bit).
//!   - \ref ppc::Vec - Floating point register (64-bit).
//!
//! ### Memory Operands
//!
//!   - \ref ppc::Mem - PPC64 memory operand that provides `base + displacement`, `base + index`, and label based
//!     addressing.

#include <asmjit/core.h>

#include <asmjit/asmjit-scope-begin.h>
#include <asmjit/ppc/ppc64operand.h>
#include <asmjit/asmjit-scope-end.h>

#endif // ASMJIT_PPC64_H_INCLUDED
