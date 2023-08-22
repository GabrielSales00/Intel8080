#include <stdio.h>
#include <stdlib.h>
#include "TradutorInstruções8080.h"

int main(int argc, char **argv) {
    FILE *hex;
    int i, size;
    unsigned char *mem;
    hex = fopen(argv[1], "rb");
    if (hex == NULL) {
        printf("ouu that's not-\n");
    }
    else {
        printf("we are so back!\n");
        printf("\n");
    }
    //descobrir quantidade de caracteres para usar em funções posteriores
    fseek(hex, 0, SEEK_END);
    size = ftell(hex);
    printf("A posição final é %d\n", size);
    printf("\n");
    fseek(hex, 0, SEEK_SET);
    //lê o fluxo de hex e o copia para a memória (mem)
    mem = malloc(sizeof(char)*size);
    fread(mem, size, 1, hex);
    fclose(hex);
    //imprime a tradução de hex para assembly legível
    printf("\n");
    for(i = 0; i < size; i) {
        i += dissassemblerIntel8080(mem, i);
        printf("\n");
    }
    return 0;
}
