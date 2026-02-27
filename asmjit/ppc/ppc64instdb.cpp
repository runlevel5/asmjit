// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include <asmjit/core/api-build_p.h>
#if !defined(ASMJIT_NO_PPC64)

#include <asmjit/ppc/ppc64instdb_p.h>

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

namespace InstDB {

// ppc::InstDB - Instruction List
// ==============================

// One row per instruction, in the order of `Inst::Id` (verified below). The opcode is the instruction word having
// all operand fields zeroed.
//
//  +-------+---------+------------+----------+
//  | Id    | Name    | Opcode     | Encoding |
//  +-------+---------+------------+----------+
#define ASMJIT_PPC64_INST_LIST(X)             \
  X(None  , ""      , 0x00000000u, None    )  \
  X(Add   , "add"   , 0x7C000214u, RtRaRb  )  \
  X(Addi  , "addi"  , 0x38000000u, RtRaSi  )  \
  X(Addis , "addis" , 0x3C000000u, RtRaSi  )  \
  X(Bclr  , "bclr"  , 0x4C000020u, BoBiBh  )  \
  X(Ld    , "ld"    , 0xE8000000u, RtMemDs )  \
  X(Mfspr , "mfspr" , 0x7C0002A6u, RtSpr   )  \
  X(Mtspr , "mtspr" , 0x7C0003A6u, SprRs   )  \
  X(Ori   , "ori"   , 0x60000000u, RaRsUi  )  \
  X(Std   , "std"   , 0xF8000000u, RtMemDs )

// ppc::InstDB - InstInfoTable
// ===========================

#define X(id, name, opcode, encoding) { opcode, uint8_t(kEncoding##encoding), { 0u, 0u, 0u } },
const InstInfo _inst_info_table[] = { ASMJIT_PPC64_INST_LIST(X) };
#undef X

// ppc::InstDB - InstNameTable
// ===========================

#ifndef ASMJIT_NO_TEXT
#define X(id, name, opcode, encoding) name,
const char _inst_name_table[][kMaxInstNameSize + 1u] = { ASMJIT_PPC64_INST_LIST(X) };
#undef X
#endif // !ASMJIT_NO_TEXT

// ppc::InstDB - Verification
// ==========================

#define X(id, name, opcode, encoding) Inst::kId##id,
static constexpr uint32_t inst_list_ids[] = { ASMJIT_PPC64_INST_LIST(X) };
#undef X

static constexpr bool inst_list_matches_inst_ids() noexcept {
  for (uint32_t i = 0; i < ASMJIT_ARRAY_SIZE(inst_list_ids); i++) {
    if (inst_list_ids[i] != i) {
      return false;
    }
  }
  return ASMJIT_ARRAY_SIZE(inst_list_ids) == Inst::_kIdCount;
}

static_assert(inst_list_matches_inst_ids(), "ppc::InstDB instruction list must match ppc::Inst::Id");

#undef ASMJIT_PPC64_INST_LIST

} // {InstDB}

ASMJIT_END_SUB_NAMESPACE

#endif // !ASMJIT_NO_PPC64
