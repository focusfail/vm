# vm

stack machine vm

## instructions
Stack effects are `( before -- after )`, top of stack on the right. Operands are 64-bit immediates: an integer literal `n` or a label `l` (`name:`, forward references allowed). Mnemonics are case-insensitive. Binary ops compute `a op b` with `b` on top; comparisons push `1` or `0`.

| Instr | Effect | Description |
|---|---|---|
| `NOP` | `( -- )` | Do nothing |
| `HALT` | `( -- )` | Stop the VM |
| `PUSH n` | `( -- n )` | Push `n` |
| `POP` | `( x -- )` | Discard top |
| `DUP` | `( x -- x x )` | Duplicate top |
| `DUPN n` | `( x1..xn -- x1..xn x1..xn )` | Duplicate top `n` values |
| `SWAP` | `( a b -- b a )` | Swap top two |
| `ADD` `SUB` `MUL` `DIV` `MOD` | `( a b -- a op b )` | Arithmetic (integer `/`, `%`) |
| `EQ` `LT` `GT` | `( a b -- a op b )` | Compare, push `1` or `0` |
| `JMP l` | `( -- )` | Jump to `l` |
| `JZ l` | `( x -- )` | Pop `x`, jump to `l` if `x == 0` |
| `JNZ l` | `( x -- )` | Pop `x`, jump to `l` if `x != 0` |
| `CALL l` | `( -- )` | Push return address on call stack, jump to `l` |
| `RET` | `( -- )` | Pop call stack, jump there |
| `PRINT` | `( x -- )` | Pop and print `x` followed by a newline |

