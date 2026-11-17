#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instr.h"
#include "vm.h"

int32_t fetch(vm_t* vm, instr_t* instr_out, int64_t* operand_out) {
  opcode_t opcode;
  memcpy(&opcode, &vm->rom[vm->pc], sizeof(opcode_t));

  if (opcode >= OP_LAST) { // is valid opcode?
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

bool peek(vm_t* vm, int64_t* v_out) {
  if (vm->sp < 1) {
    printf("stack underflow\n");
    return false;
  }

  *v_out = vm->stack[vm->sp - 1];
  return true; 
}

void print_stack(vm_t* vm) {
  for (int i = 0; i < vm->sp; i++) {
    printf("%li\n", vm->stack[i]);
  }
}

bool execute(vm_t* vm, const instr_t* instr, const int64_t* operand) {
  switch (instr->opcode) {
    case (OP_HALT): return false;
    case (OP_PUSH): return push(vm, *operand);
    case (OP_POP): return pop(vm, nullptr);
    case (OP_DUP): {
      int64_t v;
      if (!peek(vm, &v)) return false; 
      push(vm, v);
    } break;
    case (OP_DUPN): {
      int64_t n = *operand;
      int64_t start = vm->sp - n - 1; 
      for (int64_t i = 0; i < n; i++) {
        push(vm, vm->stack[start + i]);
      }
    } break;
    case (OP_SWAP): {
        int64_t a, b;
        if (!pop(vm, &a)) return false;
        if (!pop(vm, &b)) return false;
        if (!push(vm, a)) return false;
        if (!push(vm, b)) return false;
    } break;
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
    case (OP_JNZ): {
      int64_t addr = *operand;      
      if (addr < 0 || addr >= vm->rom_len) {
        printf("address out of range");
        return false;
      }

      int64_t v;
      if (!pop(vm, &v)) return false;
      if (v != 0) {
        vm->pc = addr;
      }
    } break; 
    case (OP_PRINT): {
      int64_t v;
      pop(vm, &v);
      printf("%li\n", v);
    } break;
    case (OP_CALL): {
      if (vm->cp >= CALLS_MAX) {
        fprintf(stderr, "call stack overflow\n");
        return false;
      }
      vm->calls[vm->cp++] = vm->pc;
      vm->pc = *operand;
    } break;
    case (OP_RET): {
      if (vm->cp == 0) {
        fprintf(stderr, "call stack underflow\n");
        return false;
      }
      vm->pc = vm->calls[--vm->cp];
    } break;
  }

  return true;
}

vm_t* vm_with_rom(vm_rom_t rom) {
  vm_t* vm = malloc(sizeof(*vm));
  vm->pc = 0;
  vm->sp = 0;
  vm->cp = 0;
  vm->rom = rom.rom;
  vm->rom_len = rom.len;
  vm->verbose = false;
  return vm;
}

void vm_cleanup(vm_t *vm) {
  if (vm) {
    if (vm->rom) {
      free(vm->rom);
    }
    free(vm);
  }
}

bool vm_run(vm_t* vm) {

  while (vm->pc < vm->rom_len) {
    instr_t instr;
    int64_t operand; 
    int32_t stride = fetch(vm, &instr, &operand);
    vm->pc += stride;

    if (vm->verbose) {
    if (instr.has_imm) {
      printf("%s %li\n", instr.mnemonic, operand);
    } else {
      printf("%s\n", instr.mnemonic);
    }
    } 
    if (!execute(vm, &instr, &operand)) {
      return false;
    }
  }

  return true;
}
