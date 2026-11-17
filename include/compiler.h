#pragma once

#include <stdio.h>
#include <stdint.h>

#include "instr.h"

#define MAX_LABELS 256
#define MAX_TOKENS 4096

typedef struct {
  char *text;
  char *ptr;
  size_t text_len;
} lex_t;

typedef struct {
  const char *name;
  size_t name_len;
  int tok_idx;
} label_t;

typedef enum {
  TOK_INVALID,
  TOK_OPCODE,
  TOK_OPERAND,
  TOK_LABEL_DEF,
  TOK_LABEL,
} tok_kind_t;

typedef struct {
  tok_kind_t kind;
  union {
    int64_t label_idx;
    int64_t imm;
    opcode_t opc;
  };
} tok_t;

typedef struct {
  tok_t tokens[MAX_TOKENS];
  label_t labels[MAX_LABELS];
  int num_labels;
  int num_tokens;
  lex_t lex;
} ctx_t;

ctx_t* ctx_with_file(const char* filename);
void ctx_cleanup(ctx_t* ctx);
vm_rom_t compile(ctx_t *ctx);
