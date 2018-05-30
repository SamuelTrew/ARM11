#include "assemble.h"

/* Debug */
void printSymtab(SymbolTable s) {
  int i = 0;
  do {
    printf("%s, %u", s.symbols[i].name, s.symbols[i].val);
    i++;
  } while (i < s.size);
}

int contains(char *string, char c)
{
  int length = strlen(string);
  for (int i = 0; i < length; i++) {
    if (string[i] == c) return 1;
  }
  return 0;
}

//operation code table
int optab(char *string) {
  return 1;
}

int main(int argc, char **argv)
{
  FILE* src;
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  uint8_t locctr = 0; // Starting address for memory allocations
  SymbolTable symtab = {0, calloc(DEFAULT_MAP_SIZE, sizeof(Symbol))};

  /* Tests for correct amount of input variables */
  if (argc != 3) {
    fprintf(stderr, "Usage = %s input output\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* Opens the file */
  src = fopen(argv[1], "r");

  /* Checks the file exists */
  if (src == NULL) {
    fprintf(stderr, "File Load Failure\n");
    return EXIT_FAILURE;
  }

  /* Symbol Tablulation Loop */
  while (fgets(buffer, BUFFER_SIZE, src) != NULL) {
    char *opcode = strtok(buffer, " ");
    char *operands = strtok(NULL, " ");
    if (*opcode != ';') { // If it is not a comment
      if (opcode[strlen(opcode) - 2] == ':') { // If it is a label
        opcode[strlen(opcode) - 2] = 0;
        printf("Label = %s\n", opcode);
        if (symtabContains(symtab, opcode)) { // If the label is in symbtab
          fprintf(stderr, "Duplicate symbol, aborting assembly.\n");
          return -1;
        } else {
          Symbol new = {opcode, locctr};
          symtabAdd(symtab, new);
        }
      } else if (optab(opcode)) {
        printf("Opcode = %s, Operands = %s\n", opcode, operands);
        locctr += 32; // Length of instruction
      } else {
        printf("Unrecognized opcode %s\n", opcode);
      }
    }
  }

  printSymtab(symtab);

  /* Translation Loop */
  while (fgets(buffer, BUFFER_SIZE, src) != NULL) {

  }

  fclose(src);
  free(buffer);

}

/*** Lookup Functions ***/
int symtabContains(SymbolTable m, char *k)
{
  for (int i = 0; i < m.size; i++) {
    if (m.symbols[i].name == k)
      return 1;
  }
  return 0;
}

void symtabAdd(SymbolTable m, Symbol e)
{
  printf("Adding %s, %u\n", e.name, e.val);
  m.symbols[m.size] = e;
  m.size++;
}

uint8_t getKeyVal(SymbolTable m, char *k) {
  for (int i = 0; i < m.size; i++) {
    if (m.symbols[i].name == k)
      return m.symbols[i].val;
  }
  return -1;
}

/*
int branchInstruction() {
  char[](label);
  label =
}
*/
