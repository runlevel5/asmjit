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
    kIdAdd_rc,                           //!< Instruction 'add.'.
    kIdAddc,                             //!< Instruction 'addc'.
    kIdAddc_rc,                          //!< Instruction 'addc.'.
    kIdAddco,                            //!< Instruction 'addco'.
    kIdAddco_rc,                         //!< Instruction 'addco.'.
    kIdAdde,                             //!< Instruction 'adde'.
    kIdAdde_rc,                          //!< Instruction 'adde.'.
    kIdAddeo,                            //!< Instruction 'addeo'.
    kIdAddeo_rc,                         //!< Instruction 'addeo.'.
    kIdAddi,                             //!< Instruction 'addi'.
    kIdAddic,                            //!< Instruction 'addic'.
    kIdAddic_rc,                         //!< Instruction 'addic.'.
    kIdAddis,                            //!< Instruction 'addis'.
    kIdAddme,                            //!< Instruction 'addme'.
    kIdAddme_rc,                         //!< Instruction 'addme.'.
    kIdAddmeo,                           //!< Instruction 'addmeo'.
    kIdAddmeo_rc,                        //!< Instruction 'addmeo.'.
    kIdAddo,                             //!< Instruction 'addo'.
    kIdAddo_rc,                          //!< Instruction 'addo.'.
    kIdAddze,                            //!< Instruction 'addze'.
    kIdAddze_rc,                         //!< Instruction 'addze.'.
    kIdAddzeo,                           //!< Instruction 'addzeo'.
    kIdAddzeo_rc,                        //!< Instruction 'addzeo.'.
    kIdBclr,                             //!< Instruction 'bclr'.
    kIdDivd,                             //!< Instruction 'divd'.
    kIdDivd_rc,                          //!< Instruction 'divd.'.
    kIdDivdo,                            //!< Instruction 'divdo'.
    kIdDivdo_rc,                         //!< Instruction 'divdo.'.
    kIdDivdu,                            //!< Instruction 'divdu'.
    kIdDivdu_rc,                         //!< Instruction 'divdu.'.
    kIdDivduo,                           //!< Instruction 'divduo'.
    kIdDivduo_rc,                        //!< Instruction 'divduo.'.
    kIdDivw,                             //!< Instruction 'divw'.
    kIdDivw_rc,                          //!< Instruction 'divw.'.
    kIdDivwo,                            //!< Instruction 'divwo'.
    kIdDivwo_rc,                         //!< Instruction 'divwo.'.
    kIdDivwu,                            //!< Instruction 'divwu'.
    kIdDivwu_rc,                         //!< Instruction 'divwu.'.
    kIdDivwuo,                           //!< Instruction 'divwuo'.
    kIdDivwuo_rc,                        //!< Instruction 'divwuo.'.
    kIdLd,                               //!< Instruction 'ld'.
    kIdLi,                               //!< Instruction 'li'.
    kIdLis,                              //!< Instruction 'lis'.
    kIdMfspr,                            //!< Instruction 'mfspr'.
    kIdModsd,                            //!< Instruction 'modsd'.
    kIdModsw,                            //!< Instruction 'modsw'.
    kIdModud,                            //!< Instruction 'modud'.
    kIdModuw,                            //!< Instruction 'moduw'.
    kIdMtspr,                            //!< Instruction 'mtspr'.
    kIdMulhd,                            //!< Instruction 'mulhd'.
    kIdMulhd_rc,                         //!< Instruction 'mulhd.'.
    kIdMulhdu,                           //!< Instruction 'mulhdu'.
    kIdMulhdu_rc,                        //!< Instruction 'mulhdu.'.
    kIdMulhw,                            //!< Instruction 'mulhw'.
    kIdMulhw_rc,                         //!< Instruction 'mulhw.'.
    kIdMulhwu,                           //!< Instruction 'mulhwu'.
    kIdMulhwu_rc,                        //!< Instruction 'mulhwu.'.
    kIdMulld,                            //!< Instruction 'mulld'.
    kIdMulld_rc,                         //!< Instruction 'mulld.'.
    kIdMulldo,                           //!< Instruction 'mulldo'.
    kIdMulldo_rc,                        //!< Instruction 'mulldo.'.
    kIdMulli,                            //!< Instruction 'mulli'.
    kIdMullw,                            //!< Instruction 'mullw'.
    kIdMullw_rc,                         //!< Instruction 'mullw.'.
    kIdMullwo,                           //!< Instruction 'mullwo'.
    kIdMullwo_rc,                        //!< Instruction 'mullwo.'.
    kIdNeg,                              //!< Instruction 'neg'.
    kIdNeg_rc,                           //!< Instruction 'neg.'.
    kIdNego,                             //!< Instruction 'nego'.
    kIdNego_rc,                          //!< Instruction 'nego.'.
    kIdOri,                              //!< Instruction 'ori'.
    kIdStd,                              //!< Instruction 'std'.
    kIdSubf,                             //!< Instruction 'subf'.
    kIdSubf_rc,                          //!< Instruction 'subf.'.
    kIdSubfc,                            //!< Instruction 'subfc'.
    kIdSubfc_rc,                         //!< Instruction 'subfc.'.
    kIdSubfco,                           //!< Instruction 'subfco'.
    kIdSubfco_rc,                        //!< Instruction 'subfco.'.
    kIdSubfe,                            //!< Instruction 'subfe'.
    kIdSubfe_rc,                         //!< Instruction 'subfe.'.
    kIdSubfeo,                           //!< Instruction 'subfeo'.
    kIdSubfeo_rc,                        //!< Instruction 'subfeo.'.
    kIdSubfic,                           //!< Instruction 'subfic'.
    kIdSubfme,                           //!< Instruction 'subfme'.
    kIdSubfme_rc,                        //!< Instruction 'subfme.'.
    kIdSubfmeo,                          //!< Instruction 'subfmeo'.
    kIdSubfmeo_rc,                       //!< Instruction 'subfmeo.'.
    kIdSubfo,                            //!< Instruction 'subfo'.
    kIdSubfo_rc,                         //!< Instruction 'subfo.'.
    kIdSubfze,                           //!< Instruction 'subfze'.
    kIdSubfze_rc,                        //!< Instruction 'subfze.'.
    kIdSubfzeo,                          //!< Instruction 'subfzeo'.
    kIdSubfzeo_rc,                       //!< Instruction 'subfzeo.'.
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
