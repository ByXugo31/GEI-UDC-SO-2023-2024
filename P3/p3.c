/////////////AUTHORS/////////////
/*
Hugo Viqueira Ans
hugo.ans@udc.es
Angel Varela Sanchez
angel.varela.sanchez@udc.es
*/

#include "cmdList.h"
#include "cmnds.h"
#include "List.h"
#include "stdio.h"
#include "unistd.h"
#include "pwd.h"
/////////////DECLARACIÓN DE FUNCIONES AUXILIARES/////////////

//Funcion para ejecutar el comando command.
//Este comando se trata de forma distinta a los demás ya que llama a otro comando del historial y lo ejecuta.
void exectCommand(char *cmd[],char *arg3[],bool *terminado,List *HistoryList,List *FileList,List *MemoryList,List *ProcessList,List *EnvVar,struct command *commands);

//Función para trocear una cadena de caracteres ***PROPORCIONADA POR EL ENUNCIADO***
int TrocearCadena(char * cadena,char * trozos[]);

/////////////FUNCIONES QUE LEEN Y EJECUTAN COMANDOS/////////////
void exectCMD(char *cmd[],char *arg3[],bool *terminado,List *HistoryList,List *FileList,List *MemoryList,List *ProcessList,List *EnvVar,struct command *commands){
    //Insertamos el comando al historial.
    insertCommandToList(cmd, HistoryList);
    /*A continuación, se busca en el array de comandos el comando con el nombre introducido, según su id ejecutará un comando u otro.
    Excepto en los casos comentados (que son casos especiales) se llama a la función insertCommandToList, a la que se le pasan
    el nombre del comando, los argumentos, la lista del Historial y la función del comando al que se llama, si la función
    ha detectado un error, esta devuelve false, haciendo que el comando no se inserte al historial.*/

    switch (searchCmdID(cmd[0],commands)) {
        case 0:
            authors(cmd[1]);
            return;
        case 1:
            pid(cmd[1]);
            return;
        case 2:
            Chdir(cmd[1]);
            return;
        case 3:
            dateANDtime(0);
            return;
        case 4:
            dateANDtime(1);
            return;
        case 5:
            hist(cmd[1],HistoryList);
            return;
        case 6:
            //La función command funciona de manera completamente distinta a las demás, explicación de su funcionamiento en la función exectCommand.
            exectCommand(cmd,arg3,terminado,HistoryList,FileList,MemoryList,ProcessList,EnvVar,commands);
            return;
        case 7:
            Cmd_open(cmd,FileList);
            return;
        case 8:
            Cmd_close(cmd,FileList);
            return;
        case 9:
            Cmd_dup(cmd,FileList);
            return;
        case 10:
            listOpen(*FileList);
            return;
        case 11:
            infosys();
            return;
        case 12:
            help(cmd[1]);
            return;
        case 13:
            //Cuando se introduce quit,bye o exit, tan solo se cambia el estado de terminado a true, se libera la memoria de los comandos y se termina la función.
            *terminado=true;
            //Liberamos memoria.
            free(commands);
            printf("***DELETING COMMAND HISTORY***\n");
            deleteList(HistoryList);
            printf("***COMMAND HISTORY SUCCESSFULLY DELETED***\n\n");
            printf("***DELETING FILE LIST***\n");
            deleteFileList(FileList);
            printf("***FILE LIST SUCCESSFULLY DELETED***\n\n");
            printf("***DEALLOCATING RESERVED MEMORY***\n");
            deleteMemList(MemoryList);
            printf("***MEMORY LIST SUCCESSFULLY DEALLOCATED***\n\n");
            printf("***DELETING PROCESS LIST***\n");
            deleteMemList(ProcessList);
            printf("***PROCESS LIST SUCCESSFULLY DELETED***\n\n");
            deleteListVars(EnvVar);
            printf("Ending process...\n");
            return;
        case 16:
            create(cmd);
            return;
        case 17:
            Cmd_stat(cmd);
            return;
        case 18:
            list(cmd);
            return;
        case 19:
            delete(cmd);
            return;
        case 20:
            Cmd_malloc(cmd,MemoryList);
            return;
        case 21:
            shared(cmd,MemoryList);
            return;
        case 22:
            my_mmap(cmd,MemoryList);
            return;
        case 23:
            Cmd_read(cmd);
            return;
        case 24:
            Cmd_write(cmd);
            return;
        case 25:
            memdump(cmd);
            return;
        case 26:
            memfill(cmd);
            return;
        case 27:
            mem(cmd,*MemoryList);
            return;
        case 28:
            recurse(cmd);
            return;
        case 29:
            uid(cmd);
            return;
        case 30:
            showvar(cmd,arg3);
            return;
        case 31:
            changevar(cmd,arg3,EnvVar);
            return;
        case 32:
            subsvar(cmd,arg3);
            return;
        case 33:
            showenv(cmd,arg3);
            return;
        case 34:
            Cmd_fork(ProcessList);
            return;
        case 35:
            Cmd_exec(cmd);
            return;
        case 36:
            jobs(ProcessList);
            return;
        case 37:
            deljobs(cmd,ProcessList);
            return;
        case 38:
            job(cmd,ProcessList);
            return;
        default:
            execExternal(cmd,commands,HistoryList,FileList,MemoryList,ProcessList);
            break;
    }
}

void input(bool *terminado,char *arg3[],List *HistoryList,List *FileList,List *MemoryList,List *ProcessList,List *EnvVar,struct command *commands) {
    char input[MAX_LENGTH];
    fgets(input,sizeof(input),stdin);
    char *trozos[MAX_LENGTH];
    int trozo=TrocearCadena(input,trozos);
    if (trozo>0) exectCMD(trozos,arg3,terminado,HistoryList,FileList,MemoryList,ProcessList,EnvVar,commands);
    else perror("Input a command, type help to see a list of commands.\n");
}

/////////////MAIN/////////////

int main(int argc, char *argv[], char *arg3[]) {
    List HistoryList;
    List FileList;
    List MemoryList;
    List ProcessList;
    List EnvVars;
    //Definimos la condición que termina o no el bucle.
    bool terminado = false;
    //Inicializamos las listas de ficheros e historial.
    createEmptyList(&FileList);
    createEmptyList(&HistoryList);
    createEmptyList(&MemoryList);
    createEmptyList(&ProcessList);
    createEmptyList(&EnvVars);
    //Cargamos en la lista de ficheros los 3 elementos que se inician al inicializar el programa.
    initializeFileList(&FileList);
    //Bucle que mientras que terminado valga false, sigue ejecutando la terminal.
    printf("Type help to see the list of commands.\n");
    //Se inicializa el array de structs con los comandos.
    struct command *commands = initializeCommands(NUM_COMMANDS);
    char buffer[2048];
    size_t size=sizeof(buffer);
    struct passwd *usrE;
    usrE=getpwuid(geteuid());
    while (!terminado){
        printf("[%s]@%s » ",getcwd(buffer, size),usrE->pw_name);
        input(&terminado,arg3,&HistoryList,&FileList,&MemoryList,&ProcessList,&EnvVars,commands);
    }
    return 0;
}

/////////////FUNCIONES AUXILIARES/////////////

int TrocearCadena(char * cadena, char * trozos[]) {
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL) return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL) i++;
    return i;
}

void exectCommand(char *cmd[],char *arg3[],bool *terminado,List *HistoryList,List *FileList,List *MemoryList,List *ProcessList,List *EnvVar, struct command *commands){
    //Primero se comprueba que se pueda ejecutar el comando command.
    char *aux[MAX_LENGTH];
    if (exectCMDcommand(cmd[1],HistoryList)) {
        //Si se puede ejecutar, creamos un duplicado del comando que se introdujo previamente.
        Item comnd;
        comnd=command(cmd[1],*HistoryList);
        for (int i=0;i<256;i++) aux[i]=comnd.data[i];
        //Y pasamos cada uno de los componentes del duplicado como argumento para ejecutar la función exectCMD.
        exectCMD(aux,arg3,terminado,HistoryList,FileList,MemoryList,ProcessList,EnvVar,commands);
    } else return;
}