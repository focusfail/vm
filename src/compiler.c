#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "util.h"

bool strncaseeq(const char *a, size_t a_len, const char *b, size_t b_len) {
  if (a_len != b_len)
    return false;

  for (int i = 0; i < a_len; i++) {
    int al = tolower(a[i]);
    int bl = tolower(b[i]);
    if (al != bl)
      return false;
  }

  return true;
}

bool next(lex_t *lex) {
  return lex->ptr - lex->text < lex->text_len && *lex->ptr != '\0';
}

void skip_ws(lex_t *lex) {
  while (*lex->ptr == ' ' || *lex->ptr == '\n' || *lex->ptr == '\t' ||
         *lex->ptr == '\r')
    lex->ptr++;
}

void skip_blank(lex_t *lex) {
  while (*lex->ptr == ' ' || *lex->ptr == '\t' || *lex->ptr == '\r')
    lex->ptr++;
}

static bool is_word(char c) { return isalnum((unsigned char)c) || c == '_'; }

bool parse_label_def(ctx_t *ctx) {
  lex_t *lex = &ctx->lex;

  skip_ws(lex);
  char *save = lex->ptr;
  const char *start = lex->ptr;

  while (next(lex) && *lex->ptr != '\n' && *lex->ptr != ':') {
    lex->ptr++;
  }

  if (*lex->ptr != ':') {
    lex->ptr = save;
    return false;
  }

  size_t len = lex->ptr - start;
  while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t')) {
    len--; // trim
  }

  int idx = -1;
  for (int i = 0; i < ctx->num_labels; i++) {
    if (strncaseeq(ctx->labels[i].name, ctx->labels[i].name_len, start, len)) {
      idx = i;
      break;
    }
  }
  if (idx < 0 || ctx->num_tokens >= MAX_TOKENS) return false;

  ctx->labels[idx].tok_idx = ctx->num_tokens;
  ctx->tokens[ctx->num_tokens++] = (tok_t){.kind = TOK_LABEL_DEF, .label_idx = idx};
  return true;}

bool parse_opcode(lex_t *lex, opcode_t *out_opcode) {
  skip_ws(lex);
  const char *start = lex->ptr;

  while (next(lex) && is_word(*lex->ptr)) {
    lex->ptr++;
  }

  size_t len = lex->ptr - start;
  if (len == 0) {
    return false;
  }

  for (int i = 0; i < OP_LAST; i++) {
    const instr_t *instr = &instructions[i];
    if (strncaseeq(instr->mnemonic, strlen(instr->mnemonic), start, len)) {
      *out_opcode = instr->opcode;
      return true;
    }
  }
  return false;
}

bool parse_number(lex_t *lex, int64_t *out_number) {
  skip_blank(lex);
  char *save = lex->ptr;

  if (!isdigit((unsigned char)*lex->ptr)) {
    return false;
  }
  int64_t num = 0;
  while (next(lex) && isdigit((unsigned char)*lex->ptr)) {
    num = num * 10 + (*lex->ptr - '0');
    lex->ptr++;
  }

  if (is_word(*lex->ptr)) {
    lex->ptr = save;
    return false;
  }

  *out_number = num;
  return true;
}

bool parse_word(lex_t *lex, char **out_str, size_t *out_len) {
  skip_blank(lex);
  char *start = lex->ptr;

  while (next(lex) && is_word(*lex->ptr)) {
    lex->ptr++;
  }
  if (lex->ptr == start) {
    return false;
  }
  *out_str = start;
  *out_len = lex->ptr - start;
  return true;
}

// returns -1 on error, 0 for no operand, 1 for operand
int parse_instruction(lex_t *lex, label_t *labels, int num_labels,
                      opcode_t *out_opcode, int64_t *out_imm) {
  if (!parse_opcode(lex, out_opcode)) {
    return -1;
  }
  if (!instructions[*out_opcode].has_imm) {
    return 0;
  }
  if (parse_number(lex, out_imm)) {
    return 1;
  }
  char *word;
  size_t len;
  if (parse_word(lex, &word, &len)) {
    for (int i = 0; i < num_labels; i++) {
      if (strncaseeq(labels[i].name, labels[i].name_len, word, len)) {
        *out_imm = i;
        return 2;
      }
    }
    fprintf(stderr, "unknown label\n");
  }
  return -1;
}

bool tokenize(ctx_t *ctx) {
  lex_t *lex = &ctx->lex;

  lex->ptr = lex->text;

  for (;;) {
    skip_ws(lex);
    if (!next(lex)) {
      break;
    }
    if (parse_label_def(ctx)) { // label line: already recorded, skip it
      lex->ptr++;
      continue;
    }

    opcode_t opc;
    int64_t imm;
    int r = parse_instruction(lex, ctx->labels, ctx->num_labels, &opc, &imm);
    if (r < 0) {
      fprintf(stderr, "invalid instruction at offset %td\n",
              lex->ptr - lex->text);
      return false;
    }

    if (ctx->num_tokens + 2 > MAX_TOKENS) {
      fprintf(stderr, "too many tokens\n");
      return false;
    }

    ctx->tokens[ctx->num_tokens++] = (tok_t){.kind = TOK_OPCODE, .opc = opc};
    if (r == 1) {
      ctx->tokens[ctx->num_tokens++] = (tok_t){.kind = TOK_OPERAND, .imm = imm};
    }
    if (r == 2) {
      ctx->tokens[ctx->num_tokens++] =
          (tok_t){.kind = TOK_LABEL, .label_idx = imm};
    }
  }

  return true;
}

bool collect_labels(ctx_t *ctx) {
  lex_t *lex = &ctx->lex;
  lex->ptr = lex->text;

  while (next(lex)) {
    skip_ws(lex);
    if (!next(lex)) break;

    const char *start = lex->ptr;
    while (next(lex) && *lex->ptr != '\n' && *lex->ptr != ':') lex->ptr++;

    if (*lex->ptr == ':') {
      size_t len = lex->ptr - start;
      while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t')) len--;

      if (ctx->num_labels >= MAX_LABELS) {
        fprintf(stderr, "too many labels\n");
        return false;
      }
      ctx->labels[ctx->num_labels++] = (label_t){.name = start, .name_len = len};
      lex->ptr++;
    }
    while (next(lex) && *lex->ptr != '\n') lex->ptr++;   // rest of line
  }
  return true;
}

vm_rom_t code_gen(ctx_t *ctx) {
  tok_t *toks = ctx->tokens;
  int num_toks = ctx->num_tokens;

  uint8_t *bin = malloc(1024 * 1024);
  int64_t offs = 0;

  int64_t *label_offs = malloc(ctx->num_labels * sizeof(*label_offs));

  for (int i = 0; i < num_toks; i++) {
    tok_t tok = toks[i];

    switch (tok.kind) {
    case (TOK_LABEL_DEF): {
      label_offs[tok.label_idx] = offs;
    } break;
    case (TOK_LABEL): {
      offs += sizeof(int64_t);
    } break;
    case (TOK_OPCODE): {
      memcpy(bin + offs, &tok.opc, sizeof(tok.opc));
      offs += sizeof(opcode_t);
    } break;
    case (TOK_OPERAND): {
      memcpy(bin + offs, &tok.imm, sizeof(tok.imm));
      offs += sizeof(int64_t);
    } break;
    default:
      continue;
    }
  }

  offs = 0;
  for (int i = 0; i < num_toks; i++) {
    tok_t tok = toks[i];

    switch (tok.kind) {
    case (TOK_LABEL): {
      int64_t lbloff = label_offs[tok.label_idx];
      memcpy(bin + offs, &lbloff, sizeof(lbloff));
      offs += sizeof(int64_t);
    } break;
    case (TOK_LABEL_DEF):
      continue;
    case (TOK_OPCODE):
      offs += sizeof(opcode_t);
      break;
    case (TOK_OPERAND):
      offs += sizeof(int64_t);
      break;
    default:
      continue;
    }
  }

  free(label_offs);
  return (vm_rom_t){.rom = bin, .len = offs};
}

ctx_t *ctx_with_file(const char *filename) {
  char *data;
  size_t size;
  read_file(filename, &size, &data);

  ctx_t *ctx = malloc(sizeof(*ctx));
  ctx->num_labels = 0;
  ctx->num_tokens = 0;
  ctx->lex.text = data;
  ctx->lex.text_len = size;

  return ctx;
}

void ctx_cleanup(ctx_t *ctx) {
  if (ctx) {
    if (ctx->lex.text) {
      free(ctx->lex.text);
    }
    free(ctx);
  }
}

vm_rom_t compile(ctx_t *ctx) {
  collect_labels(ctx);
  
  if (!tokenize(ctx)) {
    exit(1);
  }

  return code_gen(ctx);
}
