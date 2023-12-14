#ifndef P0_CMDLIST_H
#define P0_CMDLIST_H
#define NULL_CMD (-1)
#define NUM_COMMANDS 40
#include "string.h"
/////////////DEFINICIÓN DE LOS TIPOS DE DATOS A USAR/////////////

typedef char cmdName;
typedef char cmdHelp;
typedef int cmdId;

typedef struct command {
    cmdId Id;
    char Name[10];
    cmdHelp Help[5000];
}cmd;

/////////////DECLARACIÓN DE FUNCIONES BÁSICAS PARA OPERAR CON EL TAD/////////////

//Esta función crea un array de structs con toda la información de los comandos disponibles en el shell.
struct command* initializeCommands(int numCommands);

//Esta función imprime todos los nombres de los comandos del array.
void printAllCmds(struct command comandos[]);

//Esta función busca un comando en el array en función del nombre y devuelve su Id.
cmdId searchCmdID(cmdName *Name,struct command comandos[]);

/////////////DECLARACIÓN DE FUNCIONES AUXILIARES QUE USAN FUNCIONES BÁSICAS/////////////

//Esta función imprime ayuda sobre un comando concreto.
void printHelp(cmdName* inputCmd,struct command comandos[]);

#endif