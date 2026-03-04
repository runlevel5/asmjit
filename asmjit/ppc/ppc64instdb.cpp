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
//  +--------------+---------------+------------+--------------+
//  | Id           | Name          | Opcode     | Encoding     |
//  +--------------+---------------+------------+--------------+
#define ASMJIT_PPC64_INST_LIST(X)                            \
  X(None        , ""           , 0x00000000u, None        )  \
  X(Add         , "add"        , 0x7C000214u, RtRaRb      )  \
  X(Add_rc      , "add."       , 0x7C000215u, RtRaRb      )  \
  X(Addc        , "addc"       , 0x7C000014u, RtRaRb      )  \
  X(Addc_rc     , "addc."      , 0x7C000015u, RtRaRb      )  \
  X(Addco       , "addco"      , 0x7C000414u, RtRaRb      )  \
  X(Addco_rc    , "addco."     , 0x7C000415u, RtRaRb      )  \
  X(Adde        , "adde"       , 0x7C000114u, RtRaRb      )  \
  X(Adde_rc     , "adde."      , 0x7C000115u, RtRaRb      )  \
  X(Addeo       , "addeo"      , 0x7C000514u, RtRaRb      )  \
  X(Addeo_rc    , "addeo."     , 0x7C000515u, RtRaRb      )  \
  X(Addi        , "addi"       , 0x38000000u, RtRaSiNoR0  )  \
  X(Addic       , "addic"      , 0x30000000u, RtRaSi      )  \
  X(Addic_rc    , "addic."     , 0x34000000u, RtRaSi      )  \
  X(Addis       , "addis"      , 0x3C000000u, RtRaSiNoR0  )  \
  X(Addme       , "addme"      , 0x7C0001D4u, RtRa        )  \
  X(Addme_rc    , "addme."     , 0x7C0001D5u, RtRa        )  \
  X(Addmeo      , "addmeo"     , 0x7C0005D4u, RtRa        )  \
  X(Addmeo_rc   , "addmeo."    , 0x7C0005D5u, RtRa        )  \
  X(Addo        , "addo"       , 0x7C000614u, RtRaRb      )  \
  X(Addo_rc     , "addo."      , 0x7C000615u, RtRaRb      )  \
  X(Addze       , "addze"      , 0x7C000194u, RtRa        )  \
  X(Addze_rc    , "addze."     , 0x7C000195u, RtRa        )  \
  X(Addzeo      , "addzeo"     , 0x7C000594u, RtRa        )  \
  X(Addzeo_rc   , "addzeo."    , 0x7C000595u, RtRa        )  \
  X(And         , "and"        , 0x7C000038u, RaRsRb      )  \
  X(And_rc      , "and."       , 0x7C000039u, RaRsRb      )  \
  X(Andc        , "andc"       , 0x7C000078u, RaRsRb      )  \
  X(Andc_rc     , "andc."      , 0x7C000079u, RaRsRb      )  \
  X(Andi_rc     , "andi."      , 0x70000000u, RaRsUi      )  \
  X(Andis_rc    , "andis."     , 0x74000000u, RaRsUi      )  \
  X(Bclr        , "bclr"       , 0x4C000020u, BoBiBh      )  \
  X(Cntlzd      , "cntlzd"     , 0x7C000074u, RaRs        )  \
  X(Cntlzd_rc   , "cntlzd."    , 0x7C000075u, RaRs        )  \
  X(Cntlzw      , "cntlzw"     , 0x7C000034u, RaRs        )  \
  X(Cntlzw_rc   , "cntlzw."    , 0x7C000035u, RaRs        )  \
  X(Cnttzd      , "cnttzd"     , 0x7C000474u, RaRs        )  \
  X(Cnttzd_rc   , "cnttzd."    , 0x7C000475u, RaRs        )  \
  X(Cnttzw      , "cnttzw"     , 0x7C000434u, RaRs        )  \
  X(Cnttzw_rc   , "cnttzw."    , 0x7C000435u, RaRs        )  \
  X(Divd        , "divd"       , 0x7C0003D2u, RtRaRb      )  \
  X(Divd_rc     , "divd."      , 0x7C0003D3u, RtRaRb      )  \
  X(Divdo       , "divdo"      , 0x7C0007D2u, RtRaRb      )  \
  X(Divdo_rc    , "divdo."     , 0x7C0007D3u, RtRaRb      )  \
  X(Divdu       , "divdu"      , 0x7C000392u, RtRaRb      )  \
  X(Divdu_rc    , "divdu."     , 0x7C000393u, RtRaRb      )  \
  X(Divduo      , "divduo"     , 0x7C000792u, RtRaRb      )  \
  X(Divduo_rc   , "divduo."    , 0x7C000793u, RtRaRb      )  \
  X(Divw        , "divw"       , 0x7C0003D6u, RtRaRb      )  \
  X(Divw_rc     , "divw."      , 0x7C0003D7u, RtRaRb      )  \
  X(Divwo       , "divwo"      , 0x7C0007D6u, RtRaRb      )  \
  X(Divwo_rc    , "divwo."     , 0x7C0007D7u, RtRaRb      )  \
  X(Divwu       , "divwu"      , 0x7C000396u, RtRaRb      )  \
  X(Divwu_rc    , "divwu."     , 0x7C000397u, RtRaRb      )  \
  X(Divwuo      , "divwuo"     , 0x7C000796u, RtRaRb      )  \
  X(Divwuo_rc   , "divwuo."    , 0x7C000797u, RtRaRb      )  \
  X(Eqv         , "eqv"        , 0x7C000238u, RaRsRb      )  \
  X(Eqv_rc      , "eqv."       , 0x7C000239u, RaRsRb      )  \
  X(Extsb       , "extsb"      , 0x7C000774u, RaRs        )  \
  X(Extsb_rc    , "extsb."     , 0x7C000775u, RaRs        )  \
  X(Extsh       , "extsh"      , 0x7C000734u, RaRs        )  \
  X(Extsh_rc    , "extsh."     , 0x7C000735u, RaRs        )  \
  X(Extsw       , "extsw"      , 0x7C0007B4u, RaRs        )  \
  X(Extsw_rc    , "extsw."     , 0x7C0007B5u, RaRs        )  \
  X(Ld          , "ld"         , 0xE8000000u, RtMemDs     )  \
  X(Li          , "li"         , 0x38000000u, RtSi        )  \
  X(Lis         , "lis"        , 0x3C000000u, RtSi        )  \
  X(Mfspr       , "mfspr"      , 0x7C0002A6u, RtSpr       )  \
  X(Modsd       , "modsd"      , 0x7C000612u, RtRaRb      )  \
  X(Modsw       , "modsw"      , 0x7C000616u, RtRaRb      )  \
  X(Modud       , "modud"      , 0x7C000212u, RtRaRb      )  \
  X(Moduw       , "moduw"      , 0x7C000216u, RtRaRb      )  \
  X(Mtspr       , "mtspr"      , 0x7C0003A6u, SprRs       )  \
  X(Mulhd       , "mulhd"      , 0x7C000092u, RtRaRb      )  \
  X(Mulhd_rc    , "mulhd."     , 0x7C000093u, RtRaRb      )  \
  X(Mulhdu      , "mulhdu"     , 0x7C000012u, RtRaRb      )  \
  X(Mulhdu_rc   , "mulhdu."    , 0x7C000013u, RtRaRb      )  \
  X(Mulhw       , "mulhw"      , 0x7C000096u, RtRaRb      )  \
  X(Mulhw_rc    , "mulhw."     , 0x7C000097u, RtRaRb      )  \
  X(Mulhwu      , "mulhwu"     , 0x7C000016u, RtRaRb      )  \
  X(Mulhwu_rc   , "mulhwu."    , 0x7C000017u, RtRaRb      )  \
  X(Mulld       , "mulld"      , 0x7C0001D2u, RtRaRb      )  \
  X(Mulld_rc    , "mulld."     , 0x7C0001D3u, RtRaRb      )  \
  X(Mulldo      , "mulldo"     , 0x7C0005D2u, RtRaRb      )  \
  X(Mulldo_rc   , "mulldo."    , 0x7C0005D3u, RtRaRb      )  \
  X(Mulli       , "mulli"      , 0x1C000000u, RtRaSi      )  \
  X(Mullw       , "mullw"      , 0x7C0001D6u, RtRaRb      )  \
  X(Mullw_rc    , "mullw."     , 0x7C0001D7u, RtRaRb      )  \
  X(Mullwo      , "mullwo"     , 0x7C0005D6u, RtRaRb      )  \
  X(Mullwo_rc   , "mullwo."    , 0x7C0005D7u, RtRaRb      )  \
  X(Nand        , "nand"       , 0x7C0003B8u, RaRsRb      )  \
  X(Nand_rc     , "nand."      , 0x7C0003B9u, RaRsRb      )  \
  X(Neg         , "neg"        , 0x7C0000D0u, RtRa        )  \
  X(Neg_rc      , "neg."       , 0x7C0000D1u, RtRa        )  \
  X(Nego        , "nego"       , 0x7C0004D0u, RtRa        )  \
  X(Nego_rc     , "nego."      , 0x7C0004D1u, RtRa        )  \
  X(Nor         , "nor"        , 0x7C0000F8u, RaRsRb      )  \
  X(Nor_rc      , "nor."       , 0x7C0000F9u, RaRsRb      )  \
  X(Or          , "or"         , 0x7C000378u, RaRsRb      )  \
  X(Or_rc       , "or."        , 0x7C000379u, RaRsRb      )  \
  X(Orc         , "orc"        , 0x7C000338u, RaRsRb      )  \
  X(Orc_rc      , "orc."       , 0x7C000339u, RaRsRb      )  \
  X(Ori         , "ori"        , 0x60000000u, RaRsUi      )  \
  X(Oris        , "oris"       , 0x64000000u, RaRsUi      )  \
  X(Popcntb     , "popcntb"    , 0x7C0000F4u, RaRs        )  \
  X(Popcntd     , "popcntd"    , 0x7C0003F4u, RaRs        )  \
  X(Popcntw     , "popcntw"    , 0x7C0002F4u, RaRs        )  \
  X(Rldcl       , "rldcl"      , 0x78000010u, RaRsRbMb6   )  \
  X(Rldcl_rc    , "rldcl."     , 0x78000011u, RaRsRbMb6   )  \
  X(Rldcr       , "rldcr"      , 0x78000012u, RaRsRbMb6   )  \
  X(Rldcr_rc    , "rldcr."     , 0x78000013u, RaRsRbMb6   )  \
  X(Rldic       , "rldic"      , 0x78000008u, RaRsSh6Mb6  )  \
  X(Rldic_rc    , "rldic."     , 0x78000009u, RaRsSh6Mb6  )  \
  X(Rldicl      , "rldicl"     , 0x78000000u, RaRsSh6Mb6  )  \
  X(Rldicl_rc   , "rldicl."    , 0x78000001u, RaRsSh6Mb6  )  \
  X(Rldicr      , "rldicr"     , 0x78000004u, RaRsSh6Mb6  )  \
  X(Rldicr_rc   , "rldicr."    , 0x78000005u, RaRsSh6Mb6  )  \
  X(Rldimi      , "rldimi"     , 0x7800000Cu, RaRsSh6Mb6  )  \
  X(Rldimi_rc   , "rldimi."    , 0x7800000Du, RaRsSh6Mb6  )  \
  X(Rlwimi      , "rlwimi"     , 0x50000000u, RaRsSh5MbMe )  \
  X(Rlwimi_rc   , "rlwimi."    , 0x50000001u, RaRsSh5MbMe )  \
  X(Rlwinm      , "rlwinm"     , 0x54000000u, RaRsSh5MbMe )  \
  X(Rlwinm_rc   , "rlwinm."    , 0x54000001u, RaRsSh5MbMe )  \
  X(Rlwnm       , "rlwnm"      , 0x5C000000u, RaRsRbMbMe  )  \
  X(Rlwnm_rc    , "rlwnm."     , 0x5C000001u, RaRsRbMbMe  )  \
  X(Sld         , "sld"        , 0x7C000036u, RaRsRb      )  \
  X(Sld_rc      , "sld."       , 0x7C000037u, RaRsRb      )  \
  X(Slw         , "slw"        , 0x7C000030u, RaRsRb      )  \
  X(Slw_rc      , "slw."       , 0x7C000031u, RaRsRb      )  \
  X(Srad        , "srad"       , 0x7C000634u, RaRsRb      )  \
  X(Srad_rc     , "srad."      , 0x7C000635u, RaRsRb      )  \
  X(Sradi       , "sradi"      , 0x7C000674u, RaRsSh6     )  \
  X(Sradi_rc    , "sradi."     , 0x7C000675u, RaRsSh6     )  \
  X(Sraw        , "sraw"       , 0x7C000630u, RaRsRb      )  \
  X(Sraw_rc     , "sraw."      , 0x7C000631u, RaRsRb      )  \
  X(Srawi       , "srawi"      , 0x7C000670u, RaRsSh5     )  \
  X(Srawi_rc    , "srawi."     , 0x7C000671u, RaRsSh5     )  \
  X(Srd         , "srd"        , 0x7C000436u, RaRsRb      )  \
  X(Srd_rc      , "srd."       , 0x7C000437u, RaRsRb      )  \
  X(Srw         , "srw"        , 0x7C000430u, RaRsRb      )  \
  X(Srw_rc      , "srw."       , 0x7C000431u, RaRsRb      )  \
  X(Std         , "std"        , 0xF8000000u, RtMemDs     )  \
  X(Subf        , "subf"       , 0x7C000050u, RtRaRb      )  \
  X(Subf_rc     , "subf."      , 0x7C000051u, RtRaRb      )  \
  X(Subfc       , "subfc"      , 0x7C000010u, RtRaRb      )  \
  X(Subfc_rc    , "subfc."     , 0x7C000011u, RtRaRb      )  \
  X(Subfco      , "subfco"     , 0x7C000410u, RtRaRb      )  \
  X(Subfco_rc   , "subfco."    , 0x7C000411u, RtRaRb      )  \
  X(Subfe       , "subfe"      , 0x7C000110u, RtRaRb      )  \
  X(Subfe_rc    , "subfe."     , 0x7C000111u, RtRaRb      )  \
  X(Subfeo      , "subfeo"     , 0x7C000510u, RtRaRb      )  \
  X(Subfeo_rc   , "subfeo."    , 0x7C000511u, RtRaRb      )  \
  X(Subfic      , "subfic"     , 0x20000000u, RtRaSi      )  \
  X(Subfme      , "subfme"     , 0x7C0001D0u, RtRa        )  \
  X(Subfme_rc   , "subfme."    , 0x7C0001D1u, RtRa        )  \
  X(Subfmeo     , "subfmeo"    , 0x7C0005D0u, RtRa        )  \
  X(Subfmeo_rc  , "subfmeo."   , 0x7C0005D1u, RtRa        )  \
  X(Subfo       , "subfo"      , 0x7C000450u, RtRaRb      )  \
  X(Subfo_rc    , "subfo."     , 0x7C000451u, RtRaRb      )  \
  X(Subfze      , "subfze"     , 0x7C000190u, RtRa        )  \
  X(Subfze_rc   , "subfze."    , 0x7C000191u, RtRa        )  \
  X(Subfzeo     , "subfzeo"    , 0x7C000590u, RtRa        )  \
  X(Subfzeo_rc  , "subfzeo."   , 0x7C000591u, RtRa        )  \
  X(Xor         , "xor"        , 0x7C000278u, RaRsRb      )  \
  X(Xor_rc      , "xor."       , 0x7C000279u, RaRsRb      )  \
  X(Xori        , "xori"       , 0x68000000u, RaRsUi      )  \
  X(Xoris       , "xoris"      , 0x6C000000u, RaRsUi      )

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
