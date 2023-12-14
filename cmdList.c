#include "cmdList.h"
#include "stdio.h"
#include "stdlib.h"

struct command* initializeCommands(int numCommands) {
    //Reservamos memoria para los comandos.
    struct command* comandos = malloc(numCommands*sizeof(struct command));
    //Inicializamos cada una de las posiciones del array de structs.
    comandos[0].Id = 0;
    strncpy(comandos[0].Name, "authors", sizeof(comandos[0].Name));
    strncpy(comandos[0].Help, "Prints the names and logins of the program authors. authors -l prints only the logins and authors -n prints only the names.", sizeof(comandos[0].Help));

    comandos[1].Id = 1;
    strncpy(comandos[1].Name, "pid", sizeof(comandos[1].Name));
    strncpy(comandos[1].Help, "Prints the pid of the process executing the shell. pid -p prints the pid of the shell’s parent process.", sizeof(comandos[1].Help));

    comandos[2].Id = 2;
    strncpy(comandos[2].Name, "chdir", sizeof(comandos[2].Name));
    strncpy(comandos[2].Help, "Changes the current working directory of the shell to [dir] (using [dir] as an argument). When invoked without arguments it prints the current working directory.", sizeof(comandos[2].Help));

    comandos[3].Id = 3;
    strncpy(comandos[3].Name, "date", sizeof(comandos[3].Name));
    strncpy(comandos[3].Help, "Displays the current date.", sizeof(comandos[3].Help));

    comandos[4].Id = 4;
    strncpy(comandos[4].Name, "time", sizeof(comandos[4].Name));
    strncpy(comandos[4].Help, "Displays the current time.", sizeof(comandos[4].Help));

    comandos[5].Id = 5;
    strncpy(comandos[5].Name, "hist", sizeof(comandos[5].Name));
    strncpy(comandos[5].Help, "Displays the history of executed commands without arguments, passing -c as an argument clears the command History, passing -N prints the first N commands.", sizeof(comandos[5].Help));

    comandos[6].Id = 6;
    strncpy(comandos[6].Name, "command", sizeof(comandos[6].Name));
    strncpy(comandos[6].Help, "Repeats command number 'N'.", sizeof(comandos[6].Help));

    comandos[7].Id = 7;
    strncpy(comandos[7].Name, "open", sizeof(comandos[7].Name));
    strncpy(comandos[7].Help, "Opens a file. The only valid modes are 'cr', 'ap', 'ex', 'ro', 'rw', 'wo', and 'tr'.", sizeof(comandos[7].Help));

    comandos[8].Id = 8;
    strncpy(comandos[8].Name, "close", sizeof(comandos[8].Name));
    strncpy(comandos[8].Help, "Closes the 'df' file descriptor.", sizeof(comandos[8].Help));

    comandos[9].Id = 9;
    strncpy(comandos[9].Name, "dup", sizeof(comandos[9].Name));
    strncpy(comandos[9].Help, "Duplicates the df file descriptor.", sizeof(comandos[9].Help));

    comandos[10].Id = 10;
    strncpy(comandos[10].Name, "listopen", sizeof(comandos[10].Name));
    strncpy(comandos[10].Help, "Lists the shell open files.", sizeof(comandos[10].Help));

    comandos[11].Id = 11;
    strncpy(comandos[11].Name, "infosys", sizeof(comandos[11].Name));
    strncpy(comandos[11].Help, "Prints information on the machine running the shell.", sizeof(comandos[11].Help));

    comandos[12].Id = 12;
    strncpy(comandos[12].Name, "help", sizeof(comandos[12].Name));
    strncpy(comandos[12].Help, "Write the list of available commands, type help (command) to see more details about a command.", sizeof(comandos[12].Help));

    comandos[13].Id = 13;
    strncpy(comandos[13].Name, "quit", sizeof(comandos[13].Name));
    strncpy(comandos[13].Help, "Ends the shell.", sizeof(comandos[13].Help));

    comandos[14].Id = 13;
    strncpy(comandos[14].Name, "exit", sizeof(comandos[14].Name));
    strncpy(comandos[14].Help, "Ends the shell.", sizeof(comandos[14].Help));

    comandos[15].Id = 13;
    strncpy(comandos[15].Name, "bye", sizeof(comandos[15].Name));
    strncpy(comandos[15].Help, "Ends the shell.", sizeof(comandos[15].Help));

    comandos[16].Id = 16;
    strncpy(comandos[16].Name, "create", sizeof(comandos[16].Name));
    strncpy(comandos[16].Help, "Creates a file when using -f (file name).", sizeof(comandos[16].Help));

    comandos[17].Id = 17;
    strncpy(comandos[17].Name, "stat", sizeof(comandos[17].Name));
    strncpy(comandos[17].Help, "Use stat (-long)(-link)(-acc) (filename)\n-long: long listing. -acc: accestime. -link: if it is symbolic link, the contained path.", sizeof(comandos[17].Help));

    comandos[18].Id = 18;
    strncpy(comandos[18].Name, "list", sizeof(comandos[18].Name));
    strncpy(comandos[18].Help, "Lists content from the directory. Use list (-reca)(-recb)(-hid)(-long)(-link)(-acc) (directoryname).\n-reca: recursive before. -recb: recursive after. -hid: shows hidden files. -long: long listing. -acc: accestime. -link: if it is symbolic link, the contained path.", sizeof(comandos[18].Help));

    comandos[19].Id = 19;
    strncpy(comandos[19].Name, "delete", sizeof(comandos[19].Name));
    strncpy(comandos[19].Help, "Delete files or empty directories. Use delete (filename).", sizeof(comandos[19].Help));

    comandos[20].Id = 19;
    strncpy(comandos[20].Name, "deltree", sizeof(comandos[20].Name));
    strncpy(comandos[20].Help, "Use deltree (filename) to delete non-empty files or directories.", sizeof(comandos[20].Help));

    comandos[21].Id = 20;
    strncpy(comandos[21].Name, "malloc", sizeof(comandos[21].Name));
    strncpy(comandos[21].Help, "malloc [-free] [tam] allocates memory with size tam with malloc, using -free frees memory, no arguments prints all memory allocated with malloc.", sizeof(comandos[21].Help));

    comandos[22].Id = 21;
    strncpy(comandos[22].Name, "shared", sizeof(comandos[22].Name));
    strncpy(comandos[22].Help, "shared [-free|-create|-delkey] cl [tam] allocates shared memory with key cl, -create cl tam: allocates shared memory with key cl and size tam, -free cl: deallocates memory with key cl, -delkey: deletes the key but don't dealocate memory, no arguments prints all memory allocated with shared.",sizeof(comandos[22].Help));

    comandos[23].Id = 22;
    strncpy(comandos[23].Name, "mmap", sizeof(comandos[23].Name));
    strncpy(comandos[23].Help, "mmap [-free] fich prm maps file fich with perms prm, -free fich: unmaps file fich, no arguments prints all memory allocated with mmap.", sizeof(comandos[23].Help));

    comandos[24].Id = 23;
    strncpy(comandos[24].Name, "read", sizeof(comandos[24].Name));
    strncpy(comandos[24].Help, "read fiche addr cont Reads cont bytes from fich to direction addr.", sizeof(comandos[24].Help));

    comandos[25].Id = 24;
    strncpy(comandos[25].Name, "write", sizeof(comandos[25].Name));
    strncpy(comandos[25].Help, "write [-o] fiche addr cont Writes cont bytes from direction direccion addr a fich (-o overwrites).", sizeof(comandos[25].Help));

    comandos[26].Id = 25;
    strncpy(comandos[26].Name, "memdump", sizeof(comandos[26].Name));
    strncpy(comandos[26].Help, "memdump addr cont Dumps on screen content (cont bytes) of memory addr.", sizeof(comandos[26].Help));

    comandos[27].Id = 26;
    strncpy(comandos[27].Name, "memfill", sizeof(comandos[27].Name));
    strncpy(comandos[27].Help, "memfill addr cont byte Fills memory addr with cont bytes of byte byte.", sizeof(comandos[27].Help));

    comandos[28].Id = 27;
    strncpy(comandos[28].Name, "mem", sizeof(comandos[28].Name));
    strncpy(comandos[28].Help, "mem [-blocks|-funcs|-vars|-all|-pmap] .. Displays memory details of process, -blocks: allocated memory blocks, -funcs: directions of functions, -vars: directions of variables, -all: all, -pmap: does pmap, no arguments does the sames as -all.", sizeof(comandos[28].Help));

    comandos[29].Id = 28;
    strncpy(comandos[29].Name, "recurse", sizeof(comandos[29].Name));
    strncpy(comandos[29].Help, "recurse [n] Calls recurse n times.", sizeof(comandos[29].Help));

    comandos[30].Id = 29;
    strncpy(comandos[30].Name, "uid", sizeof(comandos[30].Name));
    strncpy(comandos[30].Help, "uid [-get|-set] [-l] [id] \n Shows or modify user credentials of shell process"
                               "\n-get: shows credentials"
                               "\n-set id: modify credentials to user with id id"
                               "\n-set -l id: modify credentials with username id", sizeof(comandos[30].Help));

    comandos[31].Id = 30;
    strncpy(comandos[31].Name, "showvar", sizeof(comandos[31].Name));
    strncpy(comandos[31].Help, "showvar var Shows value and directions of environ var", sizeof(comandos[31].Help));

    comandos[32].Id = 31;
    strncpy(comandos[32].Name, "changevar", sizeof(comandos[32].Name));
    strncpy(comandos[32].Help, "changevar [-a|-e|-p] var value Changes value of an environ var"
                               "\n-a: uses third main argument"
                               "\n-e: uses environ"
                               "\n-p: uses putenv", sizeof(comandos[32].Help));

    comandos[33].Id = 32;
    strncpy(comandos[33].Name, "subsvar", sizeof(comandos[33].Name));
    strncpy(comandos[33].Help, "subsvar [-a|-e] var1 var2 value Changes environ variable var1 with var2=value p"
                               "\n-a: accede por el tercer arg de main"
                               "\n-e: accede mediante environ", sizeof(comandos[33].Help));

    comandos[34].Id = 33;
    strncpy(comandos[34].Name, "showenv", sizeof(comandos[34].Name));
    strncpy(comandos[34].Help, "showenv [-environ|-addr] Shows process environ"
                               "\n-environ: uses environ (not main arg3)"
                               "\n-addr: shows value and where are stored environ and third main arg", sizeof(comandos[34].Help));

    comandos[35].Id = 34;
    strncpy(comandos[35].Name, "fork", sizeof(comandos[35].Name));
    strncpy(comandos[35].Help, "Shell makes fork and waits child to finish.", sizeof(comandos[35].Help));

    comandos[36].Id = 35;
    strncpy(comandos[36].Name, "exec", sizeof(comandos[36].Name));
    strncpy(comandos[36].Help, "exec ARG ARGS executes ARG with arguments ARGS", sizeof(comandos[36].Help));

    comandos[37].Id = 36;
    strncpy(comandos[37].Name, "jobs", sizeof(comandos[37].Name));
    strncpy(comandos[37].Help, "Shows process list", sizeof(comandos[37].Help));

    comandos[38].Id = 37;
    strncpy(comandos[38].Name, "deljobs", sizeof(comandos[38].Name));
    strncpy(comandos[38].Help, "deljobs [-term][-sig] deletes process from list"
                               "\n-term: TERMINATED process"
                               "\n-sig: process terminated with signal", sizeof(comandos[38].Help));

    comandos[39].Id = 38;
    strncpy(comandos[39].Name, "job", sizeof(comandos[39].Name));
    strncpy(comandos[39].Help, "job [-fg] pid Shows info of a process with pid pid"
                               "\n-fg: moves process to foreground", sizeof(comandos[39].Help));

    return comandos;
}

void printAllCmds(struct command comandos[]){
    printf("Command list:\n");
    //Establecemos un bucle que recorre el array.
    cmdId i=0;
    while (i<NUM_COMMANDS){
        //Mientras que lo recorre va imprimiendo el nombre de cada comando de cada posición i del array.
        printf("-%s\n",comandos[i].Name);
        i++;
    }
}

cmdId searchCmdID(cmdName *Name,struct command comandos[]){
    //Establecemos un bucle que recorre el array.
    cmdId i=0;
    while (i<NUM_COMMANDS){
        //Compara si el nombre que se pasa como parámetro coincide con el de la posición i del array y lo retorna.
        if(strcmp(Name,comandos[i].Name)==0) return comandos[i].Id;
        i++;
    }
    //Si no encuentra el elemento, devuelve nulo.
    return NULL_CMD;
}

int searchCmdPos(cmdName *Name,struct command comandos[]){
    //Establecemos un bucle que recorre el array.
    cmdId i=0;
    while (i<NUM_COMMANDS){
        //Compara si el nombre que se pasa como parámetro coincide con el de la posición i del array y lo retorna.
        if(strcmp(Name,comandos[i].Name)==0) return i;
        i++;
    }
    //Si no encuentra el elemento, devuelve nulo.
    return NULL_CMD;
}

void printHelp(cmdName *inputCmd,struct command comandos[]){
    //Imprime la ayuda del comando deseado.
    printf("-%s:\n%s\n",inputCmd,comandos[searchCmdPos(inputCmd,comandos)].Help);
}