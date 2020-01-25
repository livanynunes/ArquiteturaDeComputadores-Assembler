#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void organizaLabelTable(char* token, int endereco);
void assembler_pass_one(const char* nomeArquivo);
void assembler_pass_two(const char* nomeArquivo);
int verificaoperacao(const char* token);
int verificaRegistrador(const char* token);
int getTipoOp(const char* token);
void geraObjCode(const char* arquivoSaida,int endereco,int tipoInstrucao,char label[30],char opcode[20],char operando1[5],char operando2[5],char num_bytes[10],char constante[10]);
void findOpEnd(const char* token, char* codigo);
void findEndegistrador(const char* token, char* codigo);
int findLabelEnd(const char* label);
void fim(const char* endArquivo, int endereco);
int verificaConstante(const char* token);
