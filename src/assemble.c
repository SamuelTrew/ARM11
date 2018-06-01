#include "assemble.h"

/* Debug */
void printSymtab(SymbolTable s) {
  int i = 0;
  do {
    printf("%s, %u\n", s.symbols[i].name, s.symbols[i].val);
    i++;
  } while (i < s.size);
}

int contains(char *string, char c) {
  int length = strlen(string);
  for (int i = 0; i < length; i++) {
    if (string[i] == c) return 1;
  }
  return 0;
}

int typeCheck(char* opcode) {
  opcode[3] = 0;
  if (equals(opcode, "add")
    || equals(opcode, "sub")
    || equals(opcode, "rsb")
    || equals(opcode, "and")
    || equals(opcode, "eor")
    || equals(opcode, "orr")
    || equals(opcode, "mov")
    || equals(opcode, "tst")
    || equals(opcode, "teq")
    || equals(opcode, "cmp")
    || equals(opcode, "lsl"))
    return 0;
  else if (equals(opcode, "mul") || equals(opcode, "mla"))
    return 1;
  else if (equals(opcode, "ldr") || equals(opcode, "str"))
    return 2;
  opcode[1] = 0;
  if (equals(opcode, "b")) return 3;
  return -1;
}

int main(int argc, char **argv) {
  FILE *src;
  FILE *dest;
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  uint8_t locctr = 0; // Starting address for memory allocations
  SymbolTable symtab = {0, calloc(DEFAULT_MAP_SIZE, sizeof(Symbol))};
  int size = 0; // Current pointer for symtab

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
    if (operands != NULL) {
      operands[strlen(operands) - 1] = 0;
    }
    if (*opcode != ';') { // If it is not a comment
      if (opcode[strlen(opcode) - 2] == ':') { // If it is a label
        opcode[strlen(opcode) - 2] = 0;
        printf("Label = %s\n", opcode);
        if (symtabContains(symtab, opcode)) { // If the label is in symbtab
          fprintf(stderr, "Duplicate symbol, aborting assembly.\n");
          return -1;
        } else {
          strcpy(symtab.symbols[size].name, opcode);
          symtab.symbols[size].val = locctr;
          size++;
          symtab.size = size;
        }
      } else {
        if (optabCheck(opcode)) {
          printf("Opcode: %s, operands: %s, type: %u\n", opcode, operands, typeCheck(opcode));
          locctr+=32;
        } else {
          if (strlen(opcode) == 3) {
            opcode[1] = 0;
          } else if (strlen(opcode) == 5){
            opcode[3] = 0;
          } else {
            printf("Unrecognized opcode %s\n", opcode);
          }
          if (optabCheck(opcode)) {
            printf("Opcode: %s, operands: %s, type: %u\n", opcode, operands, typeCheck(opcode));
            locctr+=32;
          } else {
                    printf("Unrecognized opcode %s\n", opcode);
          }
        }
      }
    }
  }

  printSymtab(symtab);

  uint32_t *bmem = calloc(16384, sizeof(uint32_t)); //Text Record
  int *pmem = 0;

  dest = fopen(argv[2], "w!");


  /* Translation Loop */
  while (fgets(buffer, BUFFER_SIZE, src) != NULL) {
    uint32_t instr;
    char *opcode = strtok(buffer, " ");
    char *operands = strtok(NULL, " ");
    char *op1;
    char *op2;
    char *op3;
    char *op4;
    int type = typeCheck(opcode);
    if (*opcode != ';') {
      if (optabcheck(opcode)) {
        if (opcode[strlen(opcode) - 2] == ';') {
          opcode[strlen(opcode) - 2] = 0;
          printf("Label = %s\n", opcode);
          if (symtabContains(symtab, opcode)) {
            if (type == 2) {
              bmem = getKeyVal(symtab, opcode) | optab(opcode);
            } else if (type == 3) {
              bmem = getKeyVal(symtab, opcode) | optab(opcode);
            }
          } else {
            bmem = optab(opcode);
          }
        } else {
          bmem = optab(opcode);
          instr |= getCond(opcode);
          //gotta implement it for each time of instruction ffs
          //use helper method
        }
      } else if (opcode == "byte" || opcode == "word") {
        bmem = optab(opcode);
        if ( > strlen(bmem)) { //overflow check cba right now
          //TODO: write bmem to object program
          free(bmem);
        } strcat(bmem, opcode);
      }
      fputs("lisitng line", dest); //TODO: write listing line
      fgets(bmem, strlen(bmem), src); //read next input line
    }
    fwrite(bmem, sizeof(char), sizeof(bmem), dest);
  }
  fclose(src);
  free(buffer);
}


/*** Lookup Functions ***/
int symtabContains(SymbolTable m, char *k) {
  for (int i = 0; i < m.size; i++) {
    if (m.symbols[i].name == k)
      return 1;
  }
  return 0;
}



uint8_t getKeyVal(SymbolTable m, char *k) {
  for (int i = 0; i < m.size; i++) {
    if (m.symbols[i].name == k)
      return m.symbols[i].val;
  }
  return -1;
}


//don't touch this Sam is writing bad code

//check if strings are equals
//switch statement on char array
//return the machine code

int equals(char *a, char *b) {
  int length = strlen(a);
  int length2 = strlen(b);
  if (length != length2) {
    return 0;
  }
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i]) return 0;
  }
  return 1;
}


//the code below is subject to change depending if we actually need to keep track of
// the instruction format, available addressing modes and length information

//OPTAB (operation code table)
int optab(char* a)
{
  if(equals(a, "add")) {
    return 0x800000;
  } else if (equals(a, "sub")) {
    return 0x400000;
  } else if (equals(a, "rsb")) {
    return 0x600000;
  } else if (equals(a, "and")) {
    return 0x0;
  } else if (equals(a, "eor")) {
    return 0x200000;
  } else if (equals(a, "orr")) {
    return 0x1800000;
  } else if (equals(a, "mov")) {
    return 0x1A00000;
  } else if (equals(a, "tst")) {
    return 0x1000000;
  } else if (equals(a, "teq")) {
    return 0x1200000;
  } else if (equals(a, "cmp")) {
    return 0x1400000;
  } else if (equals(a, "mul")) {
    return 0xE0000090;
  } else if (equals(a, "mla")) {
    return 0xE0200090;
  } else if (equals(a, "ldr")) {
    return 0x4100000;
  } else if (equals(a, "str")) {
    return 0x4000000;
  } else if (equals(a, "b")) {
    return 0xEA000000;
  } else if (equals(a, "lsl")) {
    return 0x1A00000;           //unsure
  }
  return -1;
}

int optabCheck(char* a)
{
  if(optab(a) != -1) {
    return 1;
  }
  return 0;
}
