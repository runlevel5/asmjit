// This file is part of AsmJit project <https://asmjit.com>
//
// See <asmjit/core.h> or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include <asmjit/core/api-build_p.h>
#if !defined(ASMJIT_NO_PPC64)

#include <asmjit/core/codewriter_p.h>
#include <asmjit/core/emitterutils_p.h>
#include <asmjit/core/formatter.h>
#include <asmjit/core/logger.h>
#include <asmjit/core/misc_p.h>
#include <asmjit/support/support.h>
#include <asmjit/ppc/ppc64assembler.h>
#include <asmjit/ppc/ppc64formatter_p.h>
#include <asmjit/ppc/ppc64instdb_p.h>

ASMJIT_BEGIN_SUB_NAMESPACE(ppc)

// ppc::Assembler - Operand Checks
// ===============================

// Tests whether the operand, which must be a register, is a 64-bit general purpose register.
static ASMJIT_INLINE_NODEBUG bool is_gp64(const Operand_& op) noexcept {
  return op.as<Reg>().is_gp64();
}

// Tests whether the given register ids are ids of physical registers.
static ASMJIT_INLINE_NODEBUG bool is_phys_id(uint32_t id0, uint32_t id1 = 0, uint32_t id2 = 0) noexcept {
  return (id0 | id1 | id2) <= 31u;
}

// ppc::Assembler - Construction & Destruction
// ===========================================

Assembler::Assembler(CodeHolder* code) noexcept : BaseAssembler() {
  _arch_mask = (uint64_t(1) << uint32_t(Arch::kPPC64_LE)) |
               (uint64_t(1) << uint32_t(Arch::kPPC64_BE)) ;

#ifndef ASMJIT_NO_LOGGING
  _funcs.format_instruction = FormatterInternal::format_instruction;
#endif

  if (code) {
    code->attach(this);
  }
}

Assembler::~Assembler() noexcept {}

// ppc::Assembler - Emit
// =====================

#define ENC_OPS2(OP0, OP1) \
  (uint32_t(OperandType::k##OP0) + \
  (uint32_t(OperandType::k##OP1) << 3))

#define ENC_OPS3(OP0, OP1, OP2) \
  (uint32_t(OperandType::k##OP0) + \
  (uint32_t(OperandType::k##OP1) << 3) + \
  (uint32_t(OperandType::k##OP2) << 6))

#define ENC_OPS4(OP0, OP1, OP2, OP3) \
  (uint32_t(OperandType::k##OP0) + \
  (uint32_t(OperandType::k##OP1) << 3) + \
  (uint32_t(OperandType::k##OP2) << 6) + \
  (uint32_t(OperandType::k##OP3) << 9))

Error Assembler::_emit(InstId inst_id, const Operand_& o0, const Operand_& o1, const Operand_& o2, const Operand_* op_ext) {
  // Logging/Validation/Error.
  constexpr InstOptions kRequiresSpecialHandling = InstOptions::kReserved;

  Error err;
  CodeWriter writer(this);

  // Combine all instruction options and also check whether the instruction
  // is valid. All options that require special handling (including invalid
  // instruction) are handled by the next branch.
  InstOptions options = InstOptions(inst_id - 1 >= Inst::_kIdCount - 1) | InstOptions((size_t)(_buffer_end - writer.cursor()) < 4) | inst_options() | forced_inst_options();

  if (inst_id >= Inst::_kIdCount) {
    inst_id = 0;
  }

  const InstDB::InstInfo* inst_info = &InstDB::_inst_info_table[inst_id];
  uint32_t opcode = inst_info->opcode();
  uint32_t isign4;

  const Operand_& o3 = op_ext[EmitterUtils::kOp3];

  if (ASMJIT_UNLIKELY(Support::test(options, kRequiresSpecialHandling))) {
    if (ASMJIT_UNLIKELY(!_code)) {
      return report_error(make_error(Error::kNotInitialized));
    }

    // Unknown instruction.
    if (ASMJIT_UNLIKELY(inst_id == 0)) {
      goto InvalidInstruction;
    }

    // Grow request, happens rarely.
    err = writer.ensure_space(this, 4);
    if (ASMJIT_UNLIKELY(err != Error::kOk)) {
      goto Failed;
    }
  }

  // Signature of the first 4 operands.
  isign4 = (uint32_t(o0.op_type())     ) +
           (uint32_t(o1.op_type()) << 3) +
           (uint32_t(o2.op_type()) << 6) +
           (uint32_t(o3.op_type()) << 9);

  switch (inst_info->_encoding) {
    case InstDB::kEncodingRtRaRb: {
      if (isign4 == ENC_OPS3(Reg, Reg, Reg)) {
        if (!is_gp64(o0) || !is_gp64(o1) || !is_gp64(o2)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id(), o2.id())) {
          goto InvalidPhysId;
        }

        opcode |= (o0.id() << 21) | (o1.id() << 16) | (o2.id() << 11);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRtRa: {
      if (isign4 == ENC_OPS2(Reg, Reg)) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        opcode |= (o0.id() << 21) | (o1.id() << 16);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRtRaSi:
    case InstDB::kEncodingRtRaSiNoR0: {
      if (isign4 == ENC_OPS3(Reg, Reg, Imm)) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        // R0 used as RA would be read as zero and not as a register.
        if (inst_info->_encoding == InstDB::kEncodingRtRaSiNoR0 && o1.id() == 0u) {
          goto InvalidPhysId;
        }

        int64_t imm = o2.as<Imm>().value();
        if (!Support::is_int_n<16>(imm)) {
          goto InvalidImmediate;
        }

        opcode |= (o0.id() << 21) | (o1.id() << 16) | (uint32_t(imm) & 0xFFFFu);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRtSi: {
      if (isign4 == ENC_OPS2(Reg, Imm)) {
        if (!is_gp64(o0)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id())) {
          goto InvalidPhysId;
        }

        int64_t imm = o1.as<Imm>().value();
        if (!Support::is_int_n<16>(imm)) {
          goto InvalidImmediate;
        }

        opcode |= (o0.id() << 21) | (uint32_t(imm) & 0xFFFFu);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRsRb: {
      if (isign4 == ENC_OPS3(Reg, Reg, Reg)) {
        if (!is_gp64(o0) || !is_gp64(o1) || !is_gp64(o2)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id(), o2.id())) {
          goto InvalidPhysId;
        }

        opcode |= (o1.id() << 21) | (o0.id() << 16) | (o2.id() << 11);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRs: {
      if (isign4 == ENC_OPS2(Reg, Reg)) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        opcode |= (o1.id() << 21) | (o0.id() << 16);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRsUi: {
      if (isign4 == ENC_OPS3(Reg, Reg, Imm)) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        int64_t imm = o2.as<Imm>().value();
        if (!Support::is_uint_n<16>(imm)) {
          goto InvalidImmediate;
        }

        opcode |= (o1.id() << 21) | (o0.id() << 16) | uint32_t(imm);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRsSh5:
    case InstDB::kEncodingRaRsSh6: {
      if (isign4 == ENC_OPS3(Reg, Reg, Imm)) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        int64_t sh = o2.as<Imm>().value();
        opcode |= (o1.id() << 21) | (o0.id() << 16);

        if (inst_info->_encoding == InstDB::kEncodingRaRsSh5) {
          if (!Support::is_uint_n<5>(sh)) {
            goto InvalidImmediate;
          }

          opcode |= uint32_t(sh) << 11;
        }
        else {
          if (!Support::is_uint_n<6>(sh)) {
            goto InvalidImmediate;
          }

          // The most significant bit of a 6-bit shift amount is stored separately.
          opcode |= ((uint32_t(sh) & 0x1Fu) << 11) | ((uint32_t(sh) >> 5) << 1);
        }

        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRsSh6Mb6:
    case InstDB::kEncodingRaRsRbMb6: {
      bool has_rb = inst_info->_encoding == InstDB::kEncodingRaRsRbMb6;

      if (isign4 == (has_rb ? ENC_OPS4(Reg, Reg, Reg, Imm) : ENC_OPS4(Reg, Reg, Imm, Imm))) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        int64_t mb = o3.as<Imm>().value();
        if (!Support::is_uint_n<6>(mb)) {
          goto InvalidImmediate;
        }

        // The most significant bit of a 6-bit mask begin (or mask end) is stored last.
        opcode |= (o1.id() << 21) | (o0.id() << 16) | ((uint32_t(mb) & 0x1Fu) << 6) | ((uint32_t(mb) >> 5) << 5);

        if (has_rb) {
          if (!is_gp64(o2)) {
            goto InvalidInstruction;
          }

          if (!is_phys_id(o2.id())) {
            goto InvalidPhysId;
          }

          opcode |= o2.id() << 11;
        }
        else {
          int64_t sh = o2.as<Imm>().value();
          if (!Support::is_uint_n<6>(sh)) {
            goto InvalidImmediate;
          }

          opcode |= ((uint32_t(sh) & 0x1Fu) << 11) | ((uint32_t(sh) >> 5) << 1);
        }

        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRaRsSh5MbMe:
    case InstDB::kEncodingRaRsRbMbMe: {
      bool has_rb = inst_info->_encoding == InstDB::kEncodingRaRsRbMbMe;
      const Operand_& o4 = op_ext[EmitterUtils::kOp4];

      if (isign4 == (has_rb ? ENC_OPS4(Reg, Reg, Reg, Imm) : ENC_OPS4(Reg, Reg, Imm, Imm)) && o4.is_imm()) {
        if (!is_gp64(o0) || !is_gp64(o1)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(o0.id(), o1.id())) {
          goto InvalidPhysId;
        }

        int64_t mb = o3.as<Imm>().value();
        int64_t me = o4.as<Imm>().value();

        if (!Support::is_uint_n<5>(mb) || !Support::is_uint_n<5>(me)) {
          goto InvalidImmediate;
        }

        opcode |= (o1.id() << 21) | (o0.id() << 16) | (uint32_t(mb) << 6) | (uint32_t(me) << 1);

        if (has_rb) {
          if (!is_gp64(o2)) {
            goto InvalidInstruction;
          }

          if (!is_phys_id(o2.id())) {
            goto InvalidPhysId;
          }

          opcode |= o2.id() << 11;
        }
        else {
          int64_t sh = o2.as<Imm>().value();
          if (!Support::is_uint_n<5>(sh)) {
            goto InvalidImmediate;
          }

          opcode |= uint32_t(sh) << 11;
        }

        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRtMemDs: {
      if (isign4 == ENC_OPS2(Reg, Mem)) {
        const Mem& m = o1.as<Mem>();

        if (!is_gp64(o0)) {
          goto InvalidInstruction;
        }

        if (!m.has_base_reg() || m.base_type() != RegType::kGp64 || m.has_index()) {
          goto InvalidAddress;
        }

        if (!is_phys_id(o0.id(), m.base_id())) {
          goto InvalidPhysId;
        }

        // R0 used as a base would be read as zero and not as a register.
        if (m.base_id() == 0u) {
          goto InvalidAddress;
        }

        int64_t disp = m.offset();
        if (!Support::is_int_n<16>(disp) || (disp & 0x3) != 0) {
          goto InvalidDisplacement;
        }

        opcode |= (o0.id() << 21) | (m.base_id() << 16) | (uint32_t(disp) & 0xFFFCu);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingRtSpr:
    case InstDB::kEncodingSprRs: {
      bool spr_first = inst_info->_encoding == InstDB::kEncodingSprRs;

      const Operand_& reg = spr_first ? o1 : o0;
      const Operand_& spr = spr_first ? o0 : o1;

      if (isign4 == (spr_first ? ENC_OPS2(Imm, Reg) : ENC_OPS2(Reg, Imm))) {
        if (!is_gp64(reg)) {
          goto InvalidInstruction;
        }

        if (!is_phys_id(reg.id())) {
          goto InvalidPhysId;
        }

        int64_t spr_id = spr.as<Imm>().value();
        if (!Support::is_uint_n<10>(spr_id)) {
          goto InvalidImmediate;
        }

        // The two 5-bit halves of the SPR number are swapped in the instruction word.
        uint32_t spr_field = ((uint32_t(spr_id) & 0x1Fu) << 5) | (uint32_t(spr_id) >> 5);

        opcode |= (reg.id() << 21) | (spr_field << 11);
        goto EmitOp;
      }

      break;
    }

    case InstDB::kEncodingBoBiBh: {
      if (isign4 == ENC_OPS3(Imm, Imm, Imm)) {
        int64_t bo = o0.as<Imm>().value();
        int64_t bi = o1.as<Imm>().value();
        int64_t bh = o2.as<Imm>().value();

        if (!Support::is_uint_n<5>(bo) || !Support::is_uint_n<5>(bi) || !Support::is_uint_n<2>(bh)) {
          goto InvalidImmediate;
        }

        opcode |= (uint32_t(bo) << 21) | (uint32_t(bi) << 16) | (uint32_t(bh) << 11);
        goto EmitOp;
      }

      break;
    }

    default:
      break;
  }

  goto InvalidInstruction;

  // --------------------------------------------------------------------------
  // [EmitOp - Opcode]
  // --------------------------------------------------------------------------

EmitOp:
  if (_environment.is_big_endian()) {
    writer.emit32u_be(opcode);
  }
  else {
    writer.emit32u_le(opcode);
  }

  // --------------------------------------------------------------------------
  // [Done]
  // --------------------------------------------------------------------------

  if (Support::test(options, InstOptions::kReserved)) {
#ifndef ASMJIT_NO_LOGGING
    if (_logger) {
      EmitterUtils::log_instruction_emitted(this, inst_id, options, o0, o1, o2, op_ext, 0, 0, writer.cursor());
    }
#endif
  }

  reset_state();

  writer.done(this);
  return Error::kOk;

  // --------------------------------------------------------------------------
  // [Error Handler]
  // --------------------------------------------------------------------------

#define ERROR_HANDLER(ERR) ERR: err = make_error(Error::k##ERR); goto Failed;
  ERROR_HANDLER(InvalidAddress)
  ERROR_HANDLER(InvalidDisplacement)
  ERROR_HANDLER(InvalidImmediate)
  ERROR_HANDLER(InvalidInstruction)
  ERROR_HANDLER(InvalidPhysId)
#undef ERROR_HANDLER

Failed:
#ifndef ASMJIT_NO_LOGGING
  return EmitterUtils::log_instruction_failed(this, err, inst_id, options, o0, o1, o2, op_ext);
#else
  reset_state();
  return report_error(err);
#endif
}

#undef ENC_OPS2
#undef ENC_OPS3
#undef ENC_OPS4

// ppc::Assembler - Align
// ======================

Error Assembler::align(AlignMode align_mode, uint32_t alignment) {
  constexpr uint32_t kNopPPC = 0x60000000u; // ori r0, r0, 0

  if (ASMJIT_UNLIKELY(!_code)) {
    return report_error(make_error(Error::kNotInitialized));
  }

  if (ASMJIT_UNLIKELY(uint32_t(align_mode) > uint32_t(AlignMode::kMaxValue))) {
    return report_error(make_error(Error::kInvalidArgument));
  }

  if (alignment <= 1) {
    return Error::kOk;
  }

  if (ASMJIT_UNLIKELY(!Support::is_power_of_2_up_to(alignment, Globals::kMaxAlignment))) {
    return report_error(make_error(Error::kInvalidArgument));
  }

  uint32_t i = uint32_t(Support::align_up_diff<size_t>(offset(), alignment));
  if (i == 0) {
    return Error::kOk;
  }

  CodeWriter writer(this);
  ASMJIT_PROPAGATE(writer.ensure_space(this, i));

  switch (align_mode) {
    case AlignMode::kCode: {
      if (ASMJIT_UNLIKELY(offset() & 0x3u)) {
        return make_error(Error::kInvalidState);
      }

      while (i >= 4) {
        if (_environment.is_big_endian()) {
          writer.emit32u_be(kNopPPC);
        }
        else {
          writer.emit32u_le(kNopPPC);
        }
        i -= 4;
      }

      ASMJIT_ASSERT(i == 0);
      break;
    }

    case AlignMode::kData:
    case AlignMode::kZero:
      writer.emit_zeros(i);
      break;
  }

  writer.done(this);

#ifndef ASMJIT_NO_LOGGING
  if (_logger) {
    StringTmp<128> sb;
    sb.append_chars(' ', _logger->indentation(FormatIndentationGroup::kCode));
    sb.append_format("align %u\n", alignment);
    _logger->log(sb);
  }
#endif

  return Error::kOk;
}

// ppc::Assembler - Events
// =======================

Error Assembler::on_attach(CodeHolder& code) noexcept {
  ASMJIT_PROPAGATE(Base::on_attach(code));

  _instruction_alignment = uint8_t(4);
  return Error::kOk;
}

Error Assembler::on_detach(CodeHolder& code) noexcept {
  return Base::on_detach(code);
}

ASMJIT_END_SUB_NAMESPACE

#endif // !ASMJIT_NO_PPC64
