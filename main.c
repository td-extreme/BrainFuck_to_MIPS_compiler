#include "stdio.h"
#include "stdlib.h"

#define STACK_MAX 100

struct Stack {
  int     data[STACK_MAX];
  int     size;
};
typedef struct Stack Stack;

void Stack_Init(Stack *S)
{
  S->size = 0;
}

int Stack_Top(Stack *S)
{
  if (S->size == 0) {
    fprintf(stderr, "Error: stack empty\n");
    return -1;
  }

  return S->data[S->size-1];
}

void Stack_Push(Stack *S, int d)
{
  if (S->size < STACK_MAX)
    S->data[S->size++] = d;
  else
    fprintf(stderr, "Error: stack full\n");
}

void Stack_Pop(Stack *S)
{
  if (S->size == 0)
    fprintf(stderr, "Error: stack empty\n");
  else
    S->size--;
}

FILE *input;
FILE *output;

int loopCounter;
Stack nestedLoops;

int main(int argc, char *argv[]) {
  if( argc == 3 ) {
    printf("Convert BrainFuck file %s to %s mips assembly file.\n", argv[1], argv[2]);
  } else {
    printf("Must supply two arguments: brainfuck_file.bf output_file.asm\n");
  }

  input = fopen(argv[1], "r");
  output = fopen(argv[2], "w");

  loopCounter = 0;
  Stack_Init(&nestedLoops);

  fprintf(output, ".data\nuser_input:\t.space 2\nbf_memmory:\t.space 30000\n");
  fprintf(output, ".text\n.globl main\nmain:\n\tla\t$s0, bf_memmory\n");
  int currentLoopCount;
  char command;
  while ((command = getc(input)) != EOF) {
    printf("%c", command);
    switch (command) {
      case '<':
        fprintf(output, "\taddi\t$s0, $s0, -1\n");
        break;

      case '>':
        fprintf(output, "\taddi\t$s0, $s0, 1\n");
        break;

      case '+':
        fprintf(output, "\tlb\t$t0, 0($s0)\n\taddi\t$t0, $t0 1\n\tsb\t$t0, 0($s0)\n");
        break;

      case '-':
        fprintf(output, "\tlb\t$t0, 0($s0)\n\taddi\t$t0, $t0 -1\n\tsb\t$t0, 0($s0)\n");
        break;

      case '.':
        fprintf(output, "\tlb\t$a0, 0($s0)\n\tli\t$v0, 11\n\tsyscall\n");
        break;

      case ',':
        fprintf(output, "\tla\t$a0, user_input\n\tli\t$a1, 2\n\tli\t$v0, 8\n\tsyscall\n");
        fprintf(output, "\tlb\t$t0, 0($a0)\n\tsb\t$t0, 0($s0)\n");
        break;

      case '[':
        Stack_Push(&nestedLoops, ++loopCounter);
        fprintf(output, "start_loop_%d:\n", loopCounter);
        fprintf(output, "\tlb\t$t0, 0($s0)\n\tbeqz\t$t0, exit_loop_%d\n", loopCounter);
       break;

      case ']':
        currentLoopCount = Stack_Top(&nestedLoops);
        fprintf(output, "\tj\tstart_loop_%d\n", currentLoopCount);
        fprintf(output, "exit_loop_%d:\n", currentLoopCount);
        Stack_Pop(&nestedLoops);
       break;

    }
  }

  fprintf(output, "\tli\t$v0, 10\n\tsyscall");
  return 0;
}
