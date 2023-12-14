#ifndef CMNDS_H
#define CMNDS_H

#include "List.h"
#include "cmdList.h"
#include "stdbool.h"
#include "stdlib.h"

/////////////FUNCIONES AUXILIARES/////////////
void *cadtop(char *cadena);

/////////////DECLARACIÓN DE FUNCIONES AUXILIARES PARA CREAR E INSERTAR OBJETOS A UNA LISTA/////////////

//Función que inserta o no en el historial el comando introducido.
//Si un comando ha devuelto un mensaje de error, este no se insertará en el historial, ese es el motivo de que las funciones de comandos estén definidas como bool.
void insertCommandToList(char *cmd[],List *HistoryList);

//Funcion que inserta un fichero en la lista de ficheros abiertos.
void insertFileToHistory(char *cmd[],List *FileList, int df);

/////////////DECLARACIÓN DE LAS FUNCIONES DE LOS COMANDOS/////////////

//Comando authors.
void authors(char *arg);

//Comando pid.
void pid(char *arg);

//Comando chdir.
void Chdir(char *arg);

//Comandos date y time.
void dateANDtime(int id);

//Comando hist.
void hist(char *arg, List *HistoryList);

//Comando command.
//Esta función devuelve un elemento en función de su id en el historial, si no existe devuelve un elemento nulo.
Item command(char *arg, List HistoryList);
//Esta función verifica si la función command ha devuelto un comando válido (true) o no (false)
bool exectCMDcommand(char *arg,List *HistoryList);

//Comando open.
void Cmd_open (char * tr[],List *FileList);

//Comando close.
void Cmd_close (char *tr[],List *FileList);

//Comando dup.
void Cmd_dup (char *tr[],List *FileList);

//Comando listopen.
void listOpen(List FileList);

//Comando infosys.
void infosys();

//Comando help.
void help(char *arg);

//Comando create.
void create(char *tr[]);

//Comando stat.
void Cmd_stat(char *tr[]);

//Comando list.
void list(char *tr[]);

//Comandos delete y deltree.
void delete(char *tr[]);

void freeAux(char* type, int num, List *MemoryList);

//Comando malloc.
void Cmd_malloc(char *tr[],List *MemoryList);

//Comando shared.
void shared(char *tr[],List *MemoryList);

void my_mmap(char *tr[],List *MemoryList);

//Comando read.
void Cmd_read(char *tr[]);

//Comando write.
void Cmd_write(char *tr[]);

//Comando memdump.
void memdump(char *tr[]);

//Comando memfill.
void memfill(char *tr[]);

//Comando mem.
void mem(char *tr[],List MemoryList);

//Comando recurse.
void recurse(char *tr[]);

void uid(char *tr[]);

void showvar(char *tr[],char *arg3[]);

void changevar(char *tr[],char *arg3[],List *EnvList);

void subsvar(char *tr[],char *arg3[],List *EnvList);

void showenv(char *tr[],char *arg3[]);

void Cmd_fork(List *p);

void Cmd_exec(char *tr[]);

void jobs(List *ProcessList);

void deljobs(char *tr[],List *ProcessList);

void job(char *tr[],List *ProcessList);

void execExternal(char *tr[],cmd *cmds,List *h,List *f,List *m,List *p,List *env);
#endif