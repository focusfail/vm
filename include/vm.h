#pragma once

#include <stdint.h>
#include <stdio.h>

#include "instr.h"

constexpr size_t STACK_MAX = 1024;

typedef struct {
  size_t sp;
  int64_t stack[STACK_MAX];

  size_t pc;
  uint8_t* rom;
  size_t rom_len;

  bool verbose;
} vm_t;

vm_t* vm_with_rom(vm_rom_t rom);
void vm_cleanup(vm_t* vm);
bool vm_run(vm_t* vm);
