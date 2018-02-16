//===- WriterUtils.h --------------------------------------------*- C++ -*-===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_WASM_WRITERUTILS_H
#define LLD_WASM_WRITERUTILS_H

#include "llvm/ADT/Twine.h"
#include "llvm/Object/Wasm.h"
#include "llvm/Support/raw_ostream.h"

using llvm::raw_ostream;

// Needed for WasmSignatureDenseMapInfo
inline bool operator==(const llvm::wasm::WasmSignature &LHS,
                       const llvm::wasm::WasmSignature &RHS) {
  return LHS.ReturnType == RHS.ReturnType && LHS.ParamTypes == RHS.ParamTypes;
}

inline bool operator!=(const llvm::wasm::WasmSignature &LHS,
                       const llvm::wasm::WasmSignature &RHS) {
  return !(LHS == RHS);
}

namespace lld {
namespace wasm {

struct OutputRelocation {
  llvm::wasm::WasmRelocation Reloc;
  uint32_t NewIndex;
  uint32_t Value;
};

void debugWrite(uint64_t Offset, llvm::Twine Msg);

void writeUleb128(raw_ostream &OS, uint32_t Number, const char *Msg);

void writeSleb128(raw_ostream &OS, int32_t Number, const char *Msg);

void writeBytes(raw_ostream &OS, const char *Bytes, size_t count,
                const char *Msg = nullptr);

void writeStr(raw_ostream &OS, const llvm::StringRef String,
              const char *Msg = nullptr);

void writeU8(raw_ostream &OS, uint8_t byte, const char *Msg);

void writeU32(raw_ostream &OS, uint32_t Number, const char *Msg);

void writeValueType(raw_ostream &OS, int32_t Type, const char *Msg);

void writeSig(raw_ostream &OS, const llvm::wasm::WasmSignature &Sig);

void writeInitExpr(raw_ostream &OS, const llvm::wasm::WasmInitExpr &InitExpr);

void writeLimits(raw_ostream &OS, const llvm::wasm::WasmLimits &Limits);

void writeGlobalType(raw_ostream &OS, const llvm::wasm::WasmGlobalType &Type);

void writeGlobal(raw_ostream &OS, const llvm::wasm::WasmGlobal &Global);

void writeImport(raw_ostream &OS, const llvm::wasm::WasmImport &Import);

void writeExport(raw_ostream &OS, const llvm::wasm::WasmExport &Export);

void writeReloc(raw_ostream &OS, const OutputRelocation &Reloc);

} // namespace wasm

std::string toString(const llvm::wasm::ValType Type);
std::string toString(const llvm::wasm::WasmSignature &Sig);

} // namespace lld

#endif // LLD_WASM_WRITERUTILS_H
