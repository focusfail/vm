#include <stddef.h>
#include <stdint.h>

#include "compiler.h"
#include "vm.h"

int main(void) {
  ctx_t* ctx = ctx_with_file("/home/focus/pp/vm/test.code");
  vm_t* vm = vm_with_rom(compile(ctx));
  vm_run(vm);
  vm_cleanup(vm);
  ctx_cleanup(ctx);
}
