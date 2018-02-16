#include <stdbool.h>
#include <stdio.h>
#include <time.h>


#define NREGS 16
#define STACK_SIZE 1024
#define SP 13
#define LR 14
#define PC 15



int sum_a(char a[], int b);
int sum_array(int a[], int n);
int fib_iter(int n);
int fib_rec(int n);
int find_max(int a[], int n);
int find_str(char a[], char b[]);

int numIWs = 0;
int numData = 0;
int numBranch = 0;
int numMemory = 0;

struct arm_state {
    unsigned int regs[NREGS];
    unsigned int cpsr;
    unsigned char stack[STACK_SIZE];
};

unsigned int getRD(unsigned int iw){
    unsigned int rd = (iw >> 12) & 0xF;
    return rd;
}

unsigned int getRN(unsigned int iw){
    unsigned int rn = (iw >> 16) & 0xF;
    return rn;
}

void init_arm_state(struct arm_state *as, unsigned int *func,
                   unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    int i;

    /* zero out all arm state */
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->cpsr = 0;

    for (i = 0; i < STACK_SIZE; i++) {
        as->stack[i] = 0;
    }

    as->regs[PC] = (unsigned int) func;
    as->regs[SP] = (unsigned int) &as->stack[STACK_SIZE];
    as->regs[LR] = 0;

    as->regs[0] = arg0;
    // printf("regs[0] = %d\n", arg0);
    // printf("value: %c", (*((unsigned int *)arg0)));
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;
}

bool is_sub_inst(unsigned int iw){
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b0010);
}

void armemu_sub(struct arm_state *state){
    //printf("INSIDE ARMEMU_SUB\n");
    unsigned int iw;
    unsigned int rd, rn, rm, imm8;

    iw = *((unsigned int *) state->regs[PC]);
    unsigned int i = (iw >> 25) & 0b0000001;
    if(i == 0b1){
        //printf("inside imm for add \n");
        rd = getRD(iw);
        rn = getRN(iw);
        imm8 = iw & 0b11111111;

        state->regs[rd] = state->regs[rn] - imm8;
        if (rd != PC) {
            state->regs[PC] = state->regs[PC] + 4;
        }
    }else{
        //printf("inside register for add \n");
        rd = getRD(iw);
        rn = getRN(iw);
        rm = iw & 0xF;

        state->regs[rd] = state->regs[rn] - state->regs[rm];
        if (rd != PC) {
            state->regs[PC] = state->regs[PC] + 4;
        }
    }
}


bool is_add_inst(unsigned int iw)
{
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b0100);
}


void armemu_add(struct arm_state *state){
    //printf("INSIDE ARMEMU_ADD\n");
    unsigned int iw;
    unsigned int rd, rn, rm, imm8;

    iw = *((unsigned int *) state->regs[PC]);
    unsigned int i = (iw >> 25) & 0b0000001;
    if(i == 0b1){
        //printf("inside imm for add \n");
        rd = getRD(iw);
        rn = getRN(iw);
        imm8 = iw & 0b11111111;

        state->regs[rd] = state->regs[rn] + imm8;
        if (rd != PC) {
            state->regs[PC] = state->regs[PC] + 4;
        }
    }else{
        //printf("inside register for add \n");
        rd = getRD(iw);
        rn = getRN(iw);
        rm = iw & 0xF;

        state->regs[rd] = state->regs[rn] + state->regs[rm];
        if (rd != PC) {
            state->regs[PC] = state->regs[PC] + 4;
        }
    }
}

bool is_ldr(unsigned int iw){
    unsigned int B, L, op, I;
    op = (iw>>26) & 0b11;
    B = (iw>>22) & 0b1;
    L = (iw>>20) & 0b1;

    return (op == 0b01 && B == 0b0 && L == 0b1);
}

void armemu_ldr(struct arm_state *state){
    //printf("INSIDE ARMEMU_LDR\n");
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int rd = getRD(iw);
    unsigned int rn = getRN(iw);
    
    state->regs[rd] = *((unsigned int *) state->regs[rn]);
    
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }

}

bool is_ldr_b(unsigned int iw){
    unsigned int B, L, op, I;
    op = (iw>>26) & 0b11;
    B = (iw>>22) & 0b1;
    L = (iw>>20) & 0b1;

    return (op == 0b01 && B == 0b1 && L == 0b1);
}

void armemu_ldrb(struct arm_state *state){
    //printf("INSIDE ARMEMU_LDRB\n");
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int rd = getRD(iw);
    unsigned int rn = getRN(iw);
    unsigned int rm = iw & 0xF;   

    unsigned temp = state->regs[rn] + state->regs[rm];
    unsigned int *value = (unsigned int *)temp;
    // state->regs[rd] = (*((unsigned int *) value));
    unsigned int hello = (*(unsigned int *) value);
    state->regs[rd] = hello&0xFF;

    
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }

}

bool is_str(unsigned int iw){
    unsigned int B, L, op, I;
    op = (iw>>26) & 0b11;
    B = (iw>>22) & 0b1;
    L = (iw>>20) & 0b1;

    return (op == 0b01 && B == 0b0 && L == 0b0);
}

void armemu_str(struct arm_state *state){
    //printf("INSIDE ARMEMU_STR\n");
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int rd = getRD(iw);
    unsigned int rn = getRN(iw);
    
    *((unsigned int *) state->regs[rn]) = state->regs[rd];
    
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }

}

bool is_branch(unsigned int iw){
    unsigned int cond;

    cond = (iw >> 25) & 0b111;

    return (cond == 0b101);
}

void armemu_branch(struct arm_state *state){
    unsigned int iw, L;
    iw = *((unsigned int *) state->regs[PC]);
    L = (iw >> 24) & 0b1;
    signed int imm24 = (iw & 0xFFFFFF);

    if((imm24 >> 23) == 0b1){
        //printf("Trying to branch UP ");
        imm24 = imm24 | 0xFF000000;
    }
    if(L == 0b0){
        //("INSIDE ARMEMU_BRANCH\n");
        state->regs[PC] = (state->regs[PC] + 8) + (imm24<<2);
    }else{
        //printf("INSIDE ARMEMU_BRANCH_LINK\n");
        state->regs[LR] = state->regs[PC]+4;
        state->regs[PC] = (state->regs[PC] + 8) + (imm24<<2);
    }    
}

bool is_cmp(unsigned int iw){
    unsigned int op;
    unsigned int opCode;
    op = (iw >> 26) & 0b11;
    opCode = (iw >> 20) & 0b11111;

    return (op == 0) && (opCode == 0b10101);
}

void armemu_cmp(struct arm_state *state){
    //printf("INSIDE ARMEMU_CMP\n");
   
    unsigned int iw, rn, rd, i, imm8;
    iw = *((unsigned int *) state->regs[PC]);
    rn = getRN(iw);
    imm8 = (iw & 0b11111111);
    rd = getRD(iw);
    i = (iw >> 25) & 0b0000001;
    if(i == 0b1){
        int value = (state->regs[rn] - imm8);
        if(value > 0){ //GT 0011 = 3
            //printf("setting cpsr to GT 0011\n");
            state->cpsr = 0b0011;
        }if(value == 0){ //EQ 0100 = 4
            //printf("setting cpsr to EQ 0100\n");
            state->cpsr = 0b0100;
        }if(value < 0){ //LT 0010 = 2
            //printf("setting cpsr to LT 0010\n");
            state->cpsr = 0b0010;
        }
    }
    else{
        unsigned int rm = iw & 0xF;
        int value = (state->regs[rn] - state->regs[rm]);
        if(value > 0){ //GT 0011 = 3
            //printf("setting cpsr to GT 0011\n");
            state->cpsr = 0b0011;
        }if(value == 0){ //EQ 0100 = 4
            //printf("setting cpsr to EQ 0100\n");
            state->cpsr = 0b0100;
        }if(value < 0){ //LT 0010 = 2
            //printf("setting cpsr to LT 0010\n");
            state->cpsr = 0b0010;
        }
    }

    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool check_cpsr(struct arm_state *state){
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    unsigned int firstFour = iw >>28;
    unsigned int cpsr = state->cpsr;
    unsigned int N = (cpsr>>3); 
    unsigned int Z = ((cpsr>>2) & 0b1);
    unsigned int C = ((cpsr>>1) & 0b1);
    unsigned int V = (cpsr & 0b1);

    
    switch(firstFour){
        case(0): //EQ
            //printf("inside EQ\n");
            if(Z == 0b1){
                //printf("passing as EQ\n");
                return true;
            }else{
                return false;
            }
        case(1): //NE
            //printf("inside NE\n");
            if(C == 0b1){
                //printf("passing as NE\n");
                return true;
            }else{
                return false;
            }
        case(10): //GE
            //printf("inside GE\n");
            if((Z == 1) || N != V){
                //printf("passing as GE\n");
                return true;
            }else{
                return false;
            }
        case(12): //GT
            //printf("inside GT\n");
            if(N != V){
                //printf("passing as GT\n");
                return true;
            }else{
                return false;
            }
        case(0b1110):
            //printf("inside EMPTY\n");
            return true;
    }return true;

}

bool is_move(unsigned int iw){
    unsigned int op;
    unsigned int opCode;
    op = (iw >> 26) & 0b11;
    opCode = (iw >> 21) & 0b1111;


    return (op == 0) && (opCode == 0b1101);
}

void amremu_move(struct arm_state *state){
    //printf("INSIDE ARMEMU_MOVE\n");
    unsigned int iw;
    unsigned int i;
    unsigned int rd, rm;

    iw = *((unsigned int *) state->regs[PC]);
    i = (iw >> 25) & 0b1;
    rd = (iw >> 12) & 0xF;
    rm = (iw & 0b111);
    unsigned int imm8 = iw & 0b11111111;

    if(i == 1){
        state->regs[rd] = imm8;
    }else{
        state->regs[rd] =  state->regs[rm];
    }
    
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }


}


bool is_bx_inst(unsigned int iw)
{
    unsigned int bx_code;

    bx_code = (iw >> 4) & 0x00FFFFFF;

    return (bx_code == 0b000100101111111111110001);
}

void armemu_bx(struct arm_state *state)
{
    //printf("INSIDE ARMEMU_BX\n");
    unsigned int iw;
    unsigned int rn;

    iw = *((unsigned int *) state->regs[PC]);
    rn = iw & 0b1111;

    state->regs[PC] = state->regs[rn];
}

void armemu_one(struct arm_state *state){
    unsigned int iw = *((unsigned int *) state->regs[PC]);
    //printf("INSIDE ARMEMU_ONE\n");
    if(!check_cpsr(state)){
        //printf("failed cpsr\n");
        state->regs[PC] = state->regs[PC] + 4;
    }else{
        //printf("passed cpsr\n");
        if(is_bx_inst(iw)) {
            numData++;
            armemu_bx(state);
        }else if (is_add_inst(iw)) {
            numData++;
            armemu_add(state);
        
        }else if(is_sub_inst(iw)){
            numData++;
            armemu_sub(state);
       
        }else if(is_move(iw)){
            numData++;
            amremu_move(state);
          
        }else if(is_cmp(iw)){
            numData++;
            armemu_cmp(state);
        }
        else if(is_branch(iw)){
            numBranch++;
            armemu_branch(state);
        }
        else if(is_ldr(iw)){
            numMemory++;
            armemu_ldr(state);
        }
        else if(is_str(iw)){
            numMemory++;
            armemu_str(state);
        }
        else if(is_ldr_b(iw)){
            numMemory++;
            armemu_ldrb(state);
        }numIWs++;
    }
   
}


unsigned int armemu(struct arm_state *state)
{

    while (state->regs[PC] != 0) {
        armemu_one(state);
    }

    return state->regs[0];
}

int testFib_Iter(struct arm_state state, int a){
    printf("TESTING FIB_ITER\n");
    printf("INPUT = %d\n", a);
    init_arm_state(&state, (unsigned int *) fib_iter, a, 0, 0, 0);
    unsigned int r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", fib_iter(a));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;

    printf("\nINPUT = %d\n", a*2);
    init_arm_state(&state, (unsigned int *) fib_iter, a*2, 0, 0, 0);
    r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", fib_iter(a*2));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;
}

int testFib_Rec(struct arm_state state, int a){
    printf("\nTESTING FIB_REC==============================================\n");
    printf("INPUT = %d\n", a);
    init_arm_state(&state, (unsigned int *) fib_rec, a, 0, 0, 0);
    unsigned int r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", fib_rec(a));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;

    printf("\nINPUT = %d\n", a*2);
    init_arm_state(&state, (unsigned int *) fib_rec, a*2, 0, 0, 0);
    r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", fib_rec(a*2));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;
}

int testSum_Array(struct arm_state state){
    printf("\nTESTING SUM_ARRAY=======================================\n");
    int a[] = {-1, -2, -3};
    printf("INPUT = ");
    int i;
    for (i = 0; i < 3; ++i){
        printf("%d", a[i]);
    }printf("\n");

    int a3[1000];
    for (i = 0; i < 999; ++i)
    {
        a3[i] = 2;
    }a3[999] = 3;
    
    init_arm_state(&state, (unsigned int *) sum_array, (unsigned int) a, 3, 0, 0);
    unsigned int r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", sum_array(a, 3));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;

    init_arm_state(&state, (unsigned int *) sum_array, (unsigned int) a3, 1000, 0, 0);
    r = armemu(&state);
    printf("\nEMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", sum_array(a3, 1000));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;
}

int testFind_Max(struct arm_state state){
    printf("\nTESTING FIND_MAX========================================\n");
    int a[] = {1, 2, 3};
    printf("INPUT = ");
    int i;
    for (i = 0; i < 3; ++i){
        printf("%d", a[i]);
    }printf("\n");
    
    init_arm_state(&state, (unsigned int *) find_max, (unsigned int) a, 3, 0, 0);
    unsigned int r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", find_max(a, 3));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;

    int a3[1000];
    
    for (i = 0; i < 998; ++i)
    {
        a3[i] = 1;
    }a3[999] = 6;
    printf("\nINPUT = ARRAY OF 999 1s and 1 6\n");
    init_arm_state(&state, (unsigned int *) find_max, (unsigned int) a3, 1000, 0, 0);
    r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", find_max(a3, 1000));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;
}

int testFind_Str(struct arm_state state){
    printf("\nTESTING FIND_STR==========================================\n");
    char c[] = "abcdefg";
    char d[] = "fg";
    printf("String = %s\n", c);
    printf("Sub String = %s\n", d);
    
    init_arm_state(&state, (unsigned int *) find_str, (unsigned int) c, (unsigned int) d, 0, 0);
    unsigned int r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", find_str(c, d));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;

    char a2[] = "MYNAMEIS";
    char b2[] = "Alec";
    printf("String = %s\n", a2);
    printf("Sub String = %s\n", b2);
    init_arm_state(&state, (unsigned int *) find_str, (unsigned int) a2, (unsigned int) b2, 0, 0);
    r = armemu(&state);
    printf("EMULATION OUTPUT = %d\n", r);
    printf("ARM OUTPUT = %d\n", find_str(a2, b2));
    printf("Number of instructions executed = %d\n", numIWs);
    printf("Number of branch instructions executed = %d\n", numBranch);
    printf("Number of memory instructions executed = %d\n", numMemory);
    printf("Number of data instructions executed = %d\n", numData);
    numData = 0;
    numMemory = 0;
    numBranch = 0;
    numIWs = 0;
}

                  
    
int main(int argc, char **argv)
{
    struct arm_state state;
    unsigned int r;
    

    testFib_Iter(state, 10);
    testFib_Rec(state, 10);
    testSum_Array(state);
    testFind_Max(state);
    testFind_Str(state);
    //init_arm_state(&state, (unsigned int *) sum_a, (unsigned int) c, (unsigned int) d, 0, 0);   
    //r = armemu(&state);

  
    return 0;
}