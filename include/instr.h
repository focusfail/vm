#pragma once

#include <stdint.h>
#include <stdio.h>

typedef uint8_t opcode_t;


typedef struct {
  uint8_t* rom;
  size_t len;
} vm_rom_t;

enum : opcode_t {
 OP_NOP = 0x0,
 OP_HALT,
 OP_PUSH,
 OP_POP,
 OP_DUP,
 OP_DUPN,
 OP_SWAP,
 OP_ADD,
 OP_SUB,
 OP_MUL,
 OP_DIV,
 OP_MOD,
 OP_EQ,
 OP_LT,
 OP_GT,
 OP_JMP,
 OP_JZ,
 OP_JNZ,
 OP_CALL,
 OP_RET,
 OP_PRINT,
 OP_LAST,
};
 
typedef struct {
  opcode_t opcode;
  const char* mnemonic;
  bool has_imm;
} instr_t;

constexpr instr_t instructions[OP_LAST] = {
  [OP_NOP   ] = (instr_t) {OP_NOP, "NOP", false},
  [OP_HALT  ] = (instr_t) {OP_HALT, "HALT", false},
  [OP_PUSH  ] = (instr_t) {OP_PUSH, "PUSH", true},
  [OP_POP   ] = (instr_t) {OP_POP, "POP", false},
  [OP_DUP   ] = (instr_t) {OP_DUP, "DUP", false},
  [OP_DUPN  ] = (instr_t) {OP_DUPN, "DUPN", true},
  [OP_SWAP  ] = (instr_t) {OP_SWAP, "SWAP", false},
  [OP_ADD   ] = (instr_t) {OP_ADD, "ADD", false},
  [OP_SUB   ] = (instr_t) {OP_SUB, "SUB", false},
  [OP_MUL   ] = (instr_t) {OP_MUL, "MUL", false},
  [OP_DIV   ] = (instr_t) {OP_DIV, "DIV", false},
  [OP_MOD   ] = (instr_t) {OP_MOD, "MOD", false},
  [OP_EQ    ] = (instr_t) {OP_EQ, "EQ", false},
  [OP_LT    ] = (instr_t) {OP_LT, "LT", false},
  [OP_GT    ] = (instr_t) {OP_GT, "GT", false},
  [OP_JMP   ] = (instr_t) {OP_JMP, "JMP", true},
  [OP_JZ    ] = (instr_t) {OP_JZ, "JZ", true},
  [OP_JNZ   ] = (instr_t) {OP_JNZ, "JNZ", true},
  [OP_CALL  ] = (instr_t) {OP_CALL, "CALL", true},
  [OP_RET   ] = (instr_t) {OP_RET, "RET", false},
  [OP_PRINT ] = (instr_t) {OP_PRINT, "PRINT", false},
};
