// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include <asmjit/core/api-build_p.h>
#if !defined(ASMJIT_NO_PPC64)

#include <asmjit/ppc/ppc64instdb_p.h>

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

namespace InstDB {

// ppc::InstDB - InstInfoTable
// ===========================

#define INST(id, opcode, encoding) { opcode, uint8_t(kEncoding##encoding), { 0u, 0u, 0u } }

// The order must match `Inst::Id`.
const InstInfo _inst_info_table[] = {
  // +--------+------------+----------+
  // | Inst   | Opcode     | Encoding |
  // +--------+------------+----------+
  INST(None   , 0x00000000u, None     ),
  INST(Add    , 0x7C000214u, RtRaRb   ),
  INST(Addi   , 0x38000000u, RtRaSi   ),
  INST(Addis  , 0x3C000000u, RtRaSi   ),
  INST(Bclr   , 0x4C000020u, BoBiBh   ),
  INST(Ld     , 0xE8000000u, RtMemDs  ),
  INST(Mfspr  , 0x7C0002A6u, RtSpr    ),
  INST(Mtspr  , 0x7C0003A6u, SprRs    ),
  INST(Ori    , 0x60000000u, RaRsUi   ),
  INST(Std    , 0xF8000000u, RtMemDs  )
};

#undef INST

static_assert(ASMJIT_ARRAY_SIZE(_inst_info_table) == Inst::_kIdCount, "ppc::InstDB table must match ppc::Inst::Id");

} // {InstDB}

ASMJIT_END_SUB_NAMESPACE

#endif // !ASMJIT_NO_PPC64
