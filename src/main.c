#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum : uint8_t {
  OP_NOP = 0x0,
  OP_PUSH = 0x10,
  OP_POP = 0x11,
  OP_ADD = 0x20,
  OP_SUB = 0x21,
  OP_MUL = 0x22,
  OP_DIV = 0x23,
  OP_MOD = 0x24,
  OP_EQ = 0x30,
  OP_LT = 0x31,
  OP_GT = 0x32,
  OP_JMP = 0x40,
  OP_JZ = 0x41,
  OP_CALL = 0x42,
  OP_RET = 0x43,
  OP_PRINT = 0x50,
  OP_HALT = 0xff,
} opcode_t;

const char* opcode_strings[256] = {
  [0x0] = "NOP",
  [0x10] = "PUSH",
  [0x11] = "POP",
  [0x20] = "ADD",
  [0x21] = "SUB",
  [0x22] = "MUL",
  [0x23] = "DIV",
  [0x24] = "MOD",
  [0x30] = "EQ",
  [0x31] = "LT",
  [0x32] = "GT",
  [0x40] = "JMP",
  [0x41] = "JZ",
  [0x42] = "CALL",
  [0x43] = "RET",
  [0x50] = "PRINT",
  [0xff] = "HALT",
};

int main(void) {
  opcode_t instrs[] = {
    OP_PUSH, 1,
    OP_PUSH, 2,
    OP_PUSH, 3,
    OP_PRINT,
    OP_HALT,
  };

  int pc = 0;
  while (pc < (sizeof(instrs) / sizeof(instrs[0])) && instrs[pc] != OP_HALT) {
    
    
    pc++;
  }
  

  return EXIT_SUCCESS;
}
