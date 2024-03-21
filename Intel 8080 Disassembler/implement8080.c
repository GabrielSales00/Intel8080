#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/** declara um conjunto de estruturas de dados (bit fields)
 * consistindo de 1 bit
 * */

typedef struct conditionCodes {
    uint8_t z:1; //ZERO FLAG - se o resultado da operação for 0
    uint8_t s:1; //SIGN FLAG - se o resultado da operação mudar o sétimo bit
    uint8_t p:1; //PARITY FLAG - quando a resposta tem paridade par ou impar
    uint8_t cy:1; //CARRY FLAG - quando a operação resulta em emprestar ou adicionar um bit a mais (como em subtração)
    uint8_t ac:1;
    uint8_t pad:3;
} ConditionCodes;

typedef struct state8080 {
    uint8_t a, b, c, d, e, h, l, *memory, int_enable;
    uint16_t sp, pc;
    ConditionCodes conditionCode;
} State8080;

//Qualquer instrução que eu não tiver implementado deverá chamar essa função
void unimplementedInstructionException(State8080 * state) {
    printf("Erro: Instrução não definida. \n");
    exit(1);
}


/** Implementações das funções em hexadecimal
 *  Não há necessidade de se implementar tudo, apenas algumas
*/
int Emulate8080Op(State8080 * state) {
    uint16_t pc = state -> pc;
    unsigned char * code = &state -> memory[pc];

    switch(* code) {
        case 0x00:      //NOP
            break;
        case 0x01:      //LXI B, word, avançar 2 bytes
            state->c = code[1];
            state-> b = code[2];
            state->pc += 2;
            break;

        case 0x05:  //DCR   B
            uint8_t res = state->b - 1;
            state->conditionCode.z = (res == 0);
            state->conditionCode.s = (0x80 == (res & 0x80)); //& = AND bitwise
            state->conditionCode.p = parity(res, 8);
            state->b = res;
            break;

        case 0x06:  //MVI   B, byte
            state->b = code[1];
            state->pc++;
            break;
        
        case 0x09:  //DAD   B
            uint32_t h1 = (state -> h << 8) | state-> l;
            uint32_t bc = (state -> b << 8) | state -> c;
            uint32_t res = h1 + bc;
            state -> h = (res & 0xff00) >> 8;
            state -> l = res & 0xff;
            state -> conditionCode.cy = ((res & 0xffff0000) > 0);
            break;

        case 0x0d:  //DCR   C
            uint8_t res = state -> c - 1;
            state -> conditionCode.z = (res == 0);
            state -> conditionCode.s = (0x80 == (res & 0x80));
            state -> conditionCode.p = parity(res, 8);
            state -> c = res;
            break;

        case 0x0e:  //MVI C, byte (mover qualquer coisa para c)
            state -> c = code[1];
            state -> pc++;
            break;
        
        case 0x0f:  //RRC
            uint8_t res = state->a;
            state ->a = ((res & 1) << 7) | (res >> 1);
            state -> conditionCode.cy = (1 == (res & 1));
            break;

        case 0x11:     //LXI    D, word
            state -> e = code[1];
            state->d = code[2];
            state -> pc += 2;
            break;

        case 0x13:  //INX   D
            state -> e++;
            if(state -> e == 0) {
                state -> d++;
            }
            break;
        
        case 0x19:      //DAD
            uint32_t hl = (state -> h << 8) | state -> l;
            uint32_t de = (state -> d << 8) | state -> e;
            uint32_t res = hl + de;
            state -> h = (res & 0xff00) >> 8;
            state -> l = res & 0xff;
            state -> conditionCode.cy = ((res & 0xffff0000) != 0);
            break;
        
        case 0x1a:  //LDAX
            uint16_t off = (state -> d << 8) | state -> e;
            state -> a = state -> memory [off];
            break;
        
        case 0x21: //LXI
            state -> l = code [1];
            state -> h = code [2];
            state -> pc += 2;
            break;

        case 0x23: // INX
            state -> l++;
            if (state -> l == 0) {
                state -> h++;
            }
            break;

        case 0x26: //MVI H, byte; move para H o código
            state -> h = code[1];
            state->pc++;
            break;
    
        case 0x29: //DAD H
            uint32_t hl = (state -> h << 8) | state -> l; //concatena os valores de h e l em hl
            uint32_t res = hl + hl;
            state -> h = (res & 0xff00) >> 8;
            state -> l = res & 0xff;
            state -> conditionCode.cy = ((res & 0xffff0000) != 0);
            break;
        
        case 0x31: //LXI SP, word
            state -> sp = (code[2] << 8) | code[1];
            state -> pc += 2;
            break;
        
        

        default:
            unimplementedInstructionException(state);
            break;
    }
}

