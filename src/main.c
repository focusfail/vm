#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIN_INT(v) v, 0, 0, 0, 0, 0, 0, 0

static constexpr size_t STACK_MAX = 1024;

typedef uint8_t opcode_t;

typedef struct {
  size_t sp;
  int64_t stack[STACK_MAX];

  size_t pc;
  uint8_t* rom;
  size_t rom_len;
} vm_t;

enum : opcode_t {
 OP_NOP = 0x0,
 OP_HALT,
 OP_PUSH,
 OP_POP,
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
 OP_CALL,
 OP_RET,
 OP_PRINT,
 OP_LAST,
};

typedef int32_t (*instr_fn_t)(vm_t*, int64_t*);
 
typedef struct {
  opcode_t opcode;
  const char* mnemonic;
  bool has_imm;
} instr_t;

static constexpr instr_t instructions[OP_LAST] = {
  [OP_NOP   ] = (instr_t) {OP_NOP, "NOP", false},
  [OP_HALT  ] = (instr_t) {OP_HALT, "HALT", false},
  [OP_PUSH  ] = (instr_t) {OP_PUSH, "PUSH", true},
  [OP_POP   ] = (instr_t) {OP_POP, "POP", false},
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
  [OP_CALL  ] = (instr_t) {OP_CALL, "CALL", true},
  [OP_RET   ] = (instr_t) {OP_RET, "RET", false},
  [OP_PRINT ] = (instr_t) {OP_PRINT, "PRINT", false},
};


int32_t fetch(vm_t* vm, instr_t* instr_out, int64_t* operand_out) {
  opcode_t opcode;
  memcpy(&opcode, &vm->rom[vm->pc], sizeof(opcode_t));

  if (opcode >= OP_LAST) { // is valid opcode?
    printf("invaldi");
    return 1;
  }

  instr_t instr = instructions[opcode];

  if (instr.has_imm) {
    // TODO: check for truncated immediate
    memcpy(operand_out, &vm->rom[vm->pc + 1], sizeof(*operand_out));
  } else {
    *operand_out = 0;
  }

  *instr_out = instr;

  int32_t stride = 1;
  stride += instr.has_imm ? sizeof(int64_t) : 0;

  return stride;
}

bool push(vm_t* vm, int64_t v) {
  if (vm->sp + 1 >= STACK_MAX) {
    printf("stack overflow\n");
    return false; 
  }

  vm->stack[vm->sp++] = v;
  return true;
}

bool pop(vm_t* vm, int64_t* v_out) {
  if (vm->sp < 1) {
    printf("stack underflow\n");
    return false; 
  }

  vm->sp--;
  
  if (v_out) {
    *v_out = vm->stack[vm->sp];
  }
  
  return true;
}

bool execute(vm_t* vm, const instr_t* instr, const int64_t* operand) {
  switch (instr->opcode) {
    case (OP_PUSH): return push(vm, *operand);
    case (OP_POP): return pop(vm, nullptr);
    case (OP_ADD):
    case (OP_SUB):
    case (OP_MUL):
    case (OP_DIV):
    case (OP_MOD):
    case (OP_EQ):
    case (OP_LT):
    case (OP_GT): {
      int64_t b, a;
      if (!pop(vm, &b)) return false; 
      if (!pop(vm, &a)) return false;

      // TODO: check for integer over-/underflow
      int64_t r = 0;
      switch (instr->opcode) {
        case (OP_ADD): r = a + b; break;
        case (OP_SUB): r = a - b; break;
        case (OP_MUL): r = a * b; break;
        case (OP_DIV): r = a / b; break;
        case (OP_MOD): r = a % b; break;
        case (OP_EQ): r = a == b; break;
        case (OP_LT): r = a < b; break;
        case (OP_GT): r = a > b; break;
      }
      return push(vm, r);
    } break;
    case (OP_JMP): {
      int64_t addr = *operand;      
      if (addr < 0 || addr >= vm->rom_len) {
        printf("address out of range");
        return false;
      }
      vm->pc = addr;
    } break; 
    case (OP_JZ): {
      int64_t addr = *operand;      
      if (addr < 0 || addr >= vm->rom_len) {
        printf("address out of range");
        return false;
      }

      int64_t v;
      if (!pop(vm, &v)) return false;
      if (v == 0) {
        vm->pc = addr;
      }
    } break; 
    case (OP_PRINT): {
      int64_t v;
      pop(vm, &v);
      push(vm, v);
      printf("-> %li", v);
    } break;
    default:
  }

  return true;
}

bool run_vm(vm_t* vm, uint8_t* bin, size_t bin_len) {
  vm->rom = bin;
  vm->rom_len = bin_len;

  while (vm->pc < vm->rom_len) {
    instr_t instr;
    int64_t operand; 
    int32_t stride = fetch(vm, &instr, &operand);
    vm->pc += stride;

    if (instr.has_imm) {
      printf("%s %li ", instr.mnemonic, operand);
    } else {
      printf("%s ", instr.mnemonic);
    }

    if (!execute(vm, &instr, &operand)) {
      return false;
    }

    printf("\n");
  }

  return true;
}

bool vm_assert(vm_t* vm, uint8_t* bin, size_t bin_len) {
  return true;
}

int main(void) {
   uint8_t bin[] = {
    OP_PUSH, BIN_INT(6), // repeate to simulate 64 bit value in binary
    OP_PUSH, BIN_INT(7),
    OP_ADD,
    OP_PRINT,
    OP_JMP, BIN_INT(9),
    OP_HALT,
  };
  // uint8_t bin[] = {
  //   OP_PUSH, BIN_INT(1),
  //   OP_JZ, BIN_INT(0),
  // };
  int32_t bin_len = (sizeof(bin) / sizeof(bin[0]));

  vm_t vm = (vm_t){.pc = 0, .sp = 0};
  run_vm(&vm, bin, bin_len);
  return EXIT_SUCCESS;
}
