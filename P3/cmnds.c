#include "time.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/utsname.h"
#include "ctype.h"
#include "fcntl.h"
#include "dirent.h"
#include "cmdList.h"
#include "cmnds.h"
#include "sys/stat.h"
#include "pwd.h"
#include "grp.h"
#include "sys/wait.h"
#include "errno.h"
#include "sys/mman.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "sigstrnum.h"
#include "sys/resource.h"

#define TAMANO 2048

//P2 y P3 no comentadas completamente por falta de tiempo

extern char **environ;

/////////////FUNCIONES AUXILIARES PARA CREAR E INSERTAR OBJETOS A UNA LISTA/////////////
void insertCommandToList(char *cmd[],List *list) {
    Item newItem;
    //Si el historial está vacío la posición del primer comando será 1.
    if (isEmptyList(*list)) newItem.posNum = 1;
        //Si no, esta se corresponderá con la siguiente de la del último.
    else newItem.posNum= getItem(last(*list)).posNum + 1;
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    //A cada posición del array de strings del item se le asigna un elemento de la posición correspondiente del array de entrada.
    for (int i=0;cmd[i]!=NULL && i<MAX_LENGTH;i++) {
        strncpy(newItem.data[i],cmd[i],MAX_LENGTH-1);
        newItem.data[i][MAX_LENGTH-1]='\0';
    }
    //Se inserta el item a la lista.
    insertToList(newItem,LNULL,list);
}

void insertFileToHistory(char *cmd[],List *list,int df) {
    Item newItem;
    //La posNum del item nuevo equivaldrá a la del descriptor que se pase de entrada
    newItem.posNum=df;
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    //A cada posición del array de strings del item se le asigna un elemento de la posición correspondiente del array de entrada.
    for (int i=0;cmd[i]!=NULL&&i<MAX_LENGTH;i++) {
        strncpy(newItem.data[i],cmd[i],MAX_LENGTH-1);
        newItem.data[i][MAX_LENGTH-1]='\0';
    }
    //Se inserta el item a la lista.
    insertToList(newItem, LNULL, list);
}

void insertMallocToMemoryList(void *ptr, int size,List *list) {
    Item newItem;
    newItem.posNum=size;
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    sprintf(newItem.data[0],"%p",ptr);
    sprintf(newItem.data[1],"(%d/%d/%d - %02d:%02d)",localTime->tm_mday, localTime->tm_mon, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min);
    sprintf(newItem.data[2],"%s","Malloc");
    //Se inserta el item a la lista.
    insertToList(newItem, LNULL, list);
}

void insertSharedToMemoryList(void *ptr, key_t key, size_t size,List *list) {
    Item newItem;
    newItem.posNum=(int)size;
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    sprintf(newItem.data[0],"%p",ptr);
    sprintf(newItem.data[1],"(%d/%d/%d - %02d:%02d)",localTime->tm_mday, localTime->tm_mon, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min);
    sprintf(newItem.data[2],"%s","Shared");
    sprintf(newItem.data[3],"key:");
    sprintf(newItem.data[4],"%d",key);
    //Se inserta el item a la lista.
    insertToList(newItem, LNULL, list);
}

void insertMmapToMemoryList(void *ptr,char *file,int df, size_t size,List *list) {
    Item newItem;
    newItem.posNum=(int)size;
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    sprintf(newItem.data[0],"%p",ptr);
    sprintf(newItem.data[1],"(%d/%d/%d - %02d:%02d)",localTime->tm_mday, localTime->tm_mon, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min);
    sprintf(newItem.data[2],"descriptor:");
    sprintf(newItem.data[3],"%d",df);
    sprintf(newItem.data[4],"file name:");
    sprintf(newItem.data[5],"%s",file);
    //Se inserta el item a la lista.
    insertToList(newItem, LNULL, list);
}

void insertProcessToList(char *cmd[],List *list,pid_t pid){
    Item newItem;
    struct passwd *usr;
    usr=getpwuid(getuid());
    time_t startedTime;
    char timestr[20];
    struct tm *startedTimeInfo;
    startedTime=time(NULL);
    startedTimeInfo=localtime(&startedTime);
    strftime(timestr, sizeof(timestr),"%Y/%m/%d %H:%M:%S",startedTimeInfo);
    newItem.posNum = pid;
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    strcpy(newItem.data[0],usr->pw_name);
    strcpy(newItem.data[1],"0");
    strcpy(newItem.data[2],timestr);
    strcpy(newItem.data[3],"ACTIVE");
    strcpy(newItem.data[4],"000");
    //A cada posición del array de strings del item se le asigna un elemento de la posición correspondiente del array de entrada.
    int j=0;
    for (int i=5;cmd[j]!=NULL && i<MAX_LENGTH;i++){
        strncpy(newItem.data[i],cmd[j],MAX_LENGTH-1);
        newItem.data[i][MAX_LENGTH-1]='\0';
        j++;
    }
    //Se inserta el item a la lista.
    insertToList(newItem,LNULL,list);
}

void updateProcessList(Item *item){
    int status;
    char auxPrio[20];
    waitpid(item->posNum,&status,WNOHANG | WUNTRACED | WCONTINUED);
    sprintf(auxPrio,"%d",getpriority(PRIO_PROCESS,item->posNum));
    strcpy(item->data[1],auxPrio);
    int signal_number = WTERMSIG(status);
    if (status==0 && getpriority(PRIO_PROCESS,item->posNum)==-1) sprintf(item->data[3],"TERMINATED");
    else if (WIFSIGNALED(status)) strcpy(item->data[3],"SIGNALED");
    else if (WIFSTOPPED(status)){
        strcpy(item->data[3],"STOPPED");
        strcpy(item->data[4], "019");
        return;
    }
    else if (WIFCONTINUED(status)) strcpy(item->data[3],"ACTIVE");
    if(strcmp(NombreSenal(signal_number),"SIGUNKNOWN")==0) strcpy(item->data[4],"000");
    else strcpy(item->data[4],NombreSenal(signal_number));
}

void insertEnvVar (void *ptr, List *list) {
    Item newItem;
    newItem.posNum=0;
    //Inicializamos el array de strings.
    for (int i=0;i<MAX_LENGTH;i++) newItem.data[i][0]='\0';
    sprintf(newItem.data[0],"%p",ptr);
    //Se inserta el item a la lista.
    insertToList(newItem, LNULL, list);
}

/////////////FUNCIONES AUXILIARES/////////////

//Función que comprueba si un string es un número entero.
bool isNumber(const char *str) {
    //Se recorre el string.
    while (*str) {
        //Si un caracter del string no es un dígito el string no es un numero.
        if (!isdigit(*str)) return false;
        str++;
    }
    //Si se ha finalizado el bucle retornamos true, el string es un numero.
    return true;
}

//Función que analiza si un directorio está vacío o no.
bool isEmptyDir(const char *directory) {
    struct dirent *entry;
    //Definimos un contador e inicializamos a 0.
    int entry_count=0;
    //Leemos el directorio (suponemos que siempre que se está usando esta función le pasamos un directorio válido.
    DIR *d=opendir(directory);
    //Leemos las entradas del directorio.
    while ((entry=readdir(d))!=NULL) {
        //Si la entrada actual es . o .. avanzamos en el bucle sin mirar la siguiente posición.
        if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0) continue;
        //Si la entrada actual no es . o .. avanzamos en el contador.
        entry_count++;
    }
    //Cerramos el directorio.
    closedir(d);
    //Si el fichero está vacio, tiene 0 entradas, devuelve true, si no delvuelve false.
    return entry_count==0;
}

//Función que analiza y devuelve un tipo a partir del modo de un elemento de un directorio **PROPORCIONADA POR EL ENUNCIADO**
char LetraTF (mode_t m){
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

//Funcion que devuelve en forma de array de caracteres los permisos de un elemento de un directorio **PROPORCIONADA POR EL ENUNCIADO**
char * ConvierteModo (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

//Sacada del código de ayuda
void * ObtenerMemoriaShmget (key_t clave, size_t tam,List *MemoryList, bool create)
{
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (create){
        if (tam)     /*tam distito de 0 indica crear */
            flags=flags | IPC_CREAT | IPC_EXCL;
        if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
        if ((id=shmget(clave, tam, flags))==-1)
            return (NULL);
        if ((p=shmat(id,NULL,0))==(void*) -1){
            aux=errno;
            if (tam)
                shmctl(id,IPC_RMID,NULL);
            errno=aux;
            return (NULL);
        }
        shmctl (id,IPC_STAT,&s);
    }
    else {
        if ((id=shmget(clave, tam, flags))==-1){
            return (NULL);
        }
        p = shmat(id,NULL,0);
    }

    insertSharedToMemoryList(p,clave,tam,MemoryList);
    return (p);
}

//Convierte la cadena a un número hexadecimal
void *cadtop(char *cadena) {
    //strtoull de la biblioteca estándar de C para convertir la cadena hexadecimal a un número sin signo de 64 bits
    //(unsigned long long int).
    unsigned long long int direccion = strtoull(cadena, NULL, 16); //16 indica la base hexadecimal.
    //Luego, ese número se interpreta como una dirección de memoria y se convierte en un puntero.
    void *puntero = (void *)direccion;
    return puntero;
}

//Sacada del código de ayuda
int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{
    int pos=0;
    char aux[1000];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

//Sacada del código de ayuda
int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}

//Mod del código de ayuda
int SustituirVariable(char * var, char * var2, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var2);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}

/////////////FUNCIONALIDAD DE LOS COMANDOS/////////////

void authors(char *arg) {
    //Si el argumento es nulo o está vacio...
    if (arg==NULL || strcmp((const char *) arg,"\0")==0){
        //Imprime los nombres y logins.
        printf("- Hugo   @ByXugo31 -> GITHUB    @xxugoo -> ig     IPlayOnToaster_ -> Twitch");
        return;
    }
    //Si el argumento es -l...
    if (strcmp(arg,"-l")==0) {
        //Se imprimen los logins.
        printf("- @ByXugo31 -> GITHUB    @xxugoo -> ig     IPlayOnToaster_ -> Twitch\n");
        return;
    }
    //Si el argumento es -n...
    if (strcmp(arg,"-n")==0) {
        //Se imprimen los nombres.
        printf("- Hugo\n");
        return;
    }
    //Si el argumento no es ni -l ni -n se imprime un error.
    perror("Invalid argument, use -n or -l.");
}

void pid(char *arg) {
    //Si el argumento es nulo o está vacio...
    if (arg==NULL || strcmp((const char *) arg,"\0")==0){
        //Se obtiene e imprime el pid.
        printf("- PID: %d\n",getpid());
        return;
    }
    //Si el argumento es -p imprime el parent pid.
    if (strcmp(arg,"-p")==0) printf("- parent PID: %d\n",getppid());
    //Si el argumento no es un argumento válido imprimimos un error.
    else perror("Invalid argument, use pid or pid -p.");
}

void Chdir(char *arg) {
    //Establecemos un buffer.
    char buffer[1024];
    size_t size=sizeof(buffer);
    //Si el argumento es nulo o está vacio...
    if (arg==NULL || strcmp(arg,"\0")==0){
        //Se almacena en el buffer el directorio actual y se muestra por pantalla.
        printf("\nYou are in:\n%s\n\n", getcwd(buffer, size));
        return;
    }
    //Si se puede acceder, llamamos a la función chdir.
    if (chdir(arg)!=0) {
        //Si no se puede cambiar a ese directorio, imprimimos error.
        perror("Error changing directory.");
        return;
    }
}

void dateANDtime(int id) {
    time_t Time;
    struct tm *date;
    char date_str[20];
    //Almacenamos la fecha de nuestro reloj del sistema en la variable Time.
    Time=time(NULL);
    //Con localtime, separamos la hora, dia, mes, año... en el struct date.
    date=localtime(&Time);
    //A la función le pasamos una id para decidir si queremos la fecha o la hora.
    switch (id) {
        case 0:
            //Utilizando strftime almacenamos en un string las unidades de tiempo en el formato que queramos del struct date (en este caso dia, mes y año).
            strftime(date_str,sizeof(date_str),"%d-%m-%Y",date);
            printf("- current date: %s\n",date_str);
            return;
        case 1:
            //Utilizando strftime almacenamos en un string las unidades de tiempo en el formato que queramos del struct date (en este caso hora, minuto y segundo).
            strftime(date_str,sizeof(date_str),"%H:%M:%S",date);
            printf("- current time: %s\n",date_str);
            return;
        default:
            //Si por cualquier motivo se pasa a la función un id nulo, imprimimos un error.
            perror("Use 0 (displays time) or 1 (displays date) for the id.");
            return;
    }
}

void hist(char *arg,List *list) {
    //Si la lista está vacía salimos de la función retornando true.
    if(isEmptyList(*list)) return;
    //Si el argumento es nulo o vacio...
    if (arg == NULL || strcmp((const char *) arg, "\0")==0) {
        //Imprimimos toda la lista.
        printList(last(*list),*list,0);
        return;
    }
    //Si el argumento es -c...
    if (strcmp(arg,"-c")==0) {
        //Eliminamos la lista.
        deleteList(list);
        return ;
    }
    //Si el primer caracter del argumento es un -
    if(arg[0]=='-') {
        //Si la cadena introducida (empezando por el segundo caracter) es un número...
        if (isNumber(arg+1)) {
            //Se imprime la lista hasta la posición indicada.
            printList(findItem(atoi(arg+1),*list),*list,0);
            return;
        }
        //Si el argumento no es un número imprimimos un error.
        perror("Invalid argument, use hist or hist -c/-N (N = any number).");
        return;
    } else {
        //Si el primer caracter del argumento no es un -, imprimimos un error.
        perror("Invalid argument, use hist or hist -c/-N (N = any number).");
        return;
    }
}

Item command(char *arg,List list) {
    //Si el argumento especificado NO es un número, creamos un elemento vacío con cmdNum = -1.
    if (!isNumber(arg)) {
        Item emptyCommand;
        emptyCommand.posNum=-1;
        return emptyCommand;
    }
    //Si no, se convierte a un número entero.
    int Pos=atoi(arg);
    //Se busca la posición que se indica en el historial.
    Position foundPos=findItem(Pos,list);
    //Si la posición es nula, o mayor a la indicada o la lista está vacia se devuelve un elemento vacío.
    if (foundPos==LNULL || getItem(last(list)).posNum == 1 || Pos > getItem(last(list)).posNum){
        Item emptyCommand;
        emptyCommand.posNum=-1;
        return emptyCommand;
    }
    else return getItem(foundPos);
}

bool exectCMDcommand(char *arg,List *list) {
    //Si la función command() devuelve un elemento cuyo cmdNum sea distinto de -1 significa que el comando se encuentra en el historial, por ende, puede ejecutarse.
    if (command(arg,*list).posNum!=-1) return true;
    else {
        //Si no se cumple lo anterior, imprimimos un error y devolvemos false.
        perror("Command not found on history");
        return false;
    }
}

void Cmd_open (char * tr[],List *list) {
    int df,mode=0,auxIndex=0;
    char *aux[MAX_LENGTH];
    //Si la cadena introducida como argumento es nulo o vacio...
    if (tr[1]==NULL){
        //Se imprime la lista.
        if(isEmptyList(*list)) return;
        printList(last(*list),*list,1);
        return;
    }
    //Inicializa aux.
    for (int i=0;i<MAX_LENGTH;i++) aux[i]=NULL;
    aux[0]=tr[1];
    //Se establece la flag mediante esta cadena de if else.
    //También se almacena en el array aux el modo.
    for (int i=2;tr[i]!=NULL && tr[i][0]!='\0';i++) {
        auxIndex++;
        if (!strcmp(tr[i],"cr")) {
            aux[auxIndex]="O_CREAT";
            mode|=O_CREAT;
        }
        else if (!strcmp(tr[i],"ex")) {
            aux[auxIndex]="O_EXCL";
            mode|=O_EXCL;
        }
        else if (!strcmp(tr[i],"ro")) {
            aux[auxIndex]="O_RDONLY";
            mode|=O_RDONLY;
        }
        else if (!strcmp(tr[i],"wo")) {
            aux[auxIndex]="O_WRONLY";
            mode|=O_WRONLY;
        }
        else if (!strcmp(tr[i],"rw")) {
            aux[auxIndex]="O_RDWR";
            mode|=O_RDWR;
        }
        else if (!strcmp(tr[i],"ap")) {
            aux[auxIndex]="O_APPEND";
            mode|=O_APPEND;
        }
        else if (!strcmp(tr[i],"tr")) {
            aux[auxIndex]="O_TRUNC";
            mode|=O_TRUNC;
        }
    }
    //Si el descriptor del archivo que se desea abrir es -1...
    if ((df=open(tr[1],mode,0777))==-1) {
        //Imprimimos un error, no se puede abrir el fichero.
        perror("File can not be open\n");
        return;
    }
    //Si no, insertamos el archivo en la lista.
    insertFileToHistory(aux,list,df);
    printf("Entry %d added to the open files table\n", getItem(last(*list)).posNum);
}

void Cmd_close (char *tr[],List *list){
    if (tr[1]==NULL || strcmp(tr[1],"\0")==0 || (atoi(tr[1])) < 0){
        //Si está vacio o es nulo, imprimimos la lista de ficheros abiertos.
        if(isEmptyList(*list)) return;
        printList(last(*list),*list,1);
        return;
    }
    //Recorremos el array de entrada.
    for (int i = 1;tr[i]!=NULL && tr[i][0]!='\0';i++) {
        //Si el string en esa posición del array es un número...
        if (isNumber(tr[i])) {
            //Si el descriptor del archivo que se desea cerrar es -1 salta error.
            if(close(atoi(tr[i])) == -1){
                perror("Can not close descriptor");
                return;
            }
            //Si no, se cierra el descriptor, se elimina el elemento de la lista y se da feedback al usuario.
            printf("Closed file: %s\n",tr[i]);
            Position pos = findItem(atoi(tr[i]),*list);
            if(pos==NULL){
                perror("Can not find descriptor");
                return;
            }
            deleteAtPosition(pos,list);
            return;
        //Si no, imprimimos un error.
        } else perror("Can not close descriptor");
    }
}

void Cmd_dup (char *tr[],List *list) {
    int df;
    char *aux[MAX_LENGTH];
    char aux2[MAX_LENGTH*2];
    if (tr[1]==NULL || strcmp(tr[1],"\0")==0 || atoi(tr[1])<0) {
        //Si está vacio o es nulo, imprimimos la lista de ficheros abiertos.
        printList(last(*list),*list,1);
        return;
    }
    //Recorremos el array de entrada.
    for (int i=1;tr[i]!=NULL && tr[i][0]!='\0';i++) {
        //Si el string en esa posición del array es un número...
        if(isNumber(tr[i])) {
            df=dup(atoi(tr[i]));
            if (df!=-1){
                //Si el dup se puede hacer (df!=-1) almacenamos los datos en un string auxiliar, se imprime feedback por pantalla y se inserta a la lista.
                sprintf(aux2,"dup %s (%s)",tr[i], getItem(findItem(atoi(tr[i]), *list)).data[0]);
                aux[0]=aux2;
                aux[1]=NULL;
                printf("Dup descriptor: %s\n",tr[i]);
                insertFileToHistory(aux,list,df);
            } else perror("Can not dup file.");
        //Si no, imprimimos un error...
        } else perror("Can not dup file.");
    }
}

void listOpen(List list) {
    //Si la lista está vacia retoramos, si no la imprimimos.
    if(isEmptyList(list)) return;
    printList(last(list),list,1);
}

void infosys() {
    struct utsname systemInfo;
    //Usando la función uname, almacenamos en el struct systemInfo la información de la máquina.
    uname(&systemInfo);
    printf("- system name: %s\n- node name: %s\n- release: %s\n- version: %s\n- machine: %s\n",systemInfo.sysname,systemInfo.nodename,systemInfo.release,systemInfo.version,systemInfo.machine);
}

void help(char *arg) {
    //Inicializamos los comandos.
    cmd *comnds=initializeCommands();
    //Si el argumento es nulo o vacio.
    if (arg==NULL || strcmp((const char *) arg,"\0")==0) {
        //Imprime la lista.
        printAllCmds(comnds);
        //Liberamos memoria.
        free(comnds);
        return;
    }
    //Si no se puede encontrar el comando del cual se quiere imprimir la ayuda...
    if(searchCmdID(arg,comnds)==NULL_CMD) {
        //Imprimimos un error.
        perror("Command not found.");
        //Liberamos memoria.
        free(comnds);
        return;
    }
    //Si ninguna de las anteriores condiciones se cumple, se llama a la función printHelp.
    printHelp(arg,comnds);
    //Liberamos memoria.
    free(comnds);
}

void create(char *tr[]) {
    int df;
    //Si la primera posición del array está vacía.
    if (tr[1]==NULL || strcmp((const char *) tr[1],"\0")==0) {
        //Imprimimos un error y retornamos.
        perror("Specify directory name or use -f (file name).");
        return;
    }
    //Si es -f...
    if (strcmp(tr[1], "-f") == 0) {
        //Creamos el fichero almacenado en tr[2] con el modo 0666.
        df = creat(tr[2],0666);
        printf("Created file: %s\n",tr[2]);
        close(df);
        return;
    }
    if (mkdir(tr[1],0777) == 0) {
        //Si no, creamos el directorio con los permisos 0777.
        printf("Created directory: %s\n",tr[1]);
        return;
    }
    //Si no se cumple ninguna de estas condiciones imprimimos un error.
    perror("Couldn't create directory.");
}

//Función auxiliar con la funcionalidad de la función stat para facilitar el uso de esta más adelante.
void CMD_stat_aux(char *directory,bool lng,bool link,bool acc) {
    char crtnaccTime[20];
    char lngstr[MAX_LENGTH]="",linkstr[100]="";
    struct stat fstats;
    if (access(directory,R_OK)!=0){
        printf("Permission denied for directory: %s\n", directory);
        return;
    }
    //Si podemos hacer stat del directorio que queremos...
    if(lstat(directory,&fstats)==0) {
        //Si lng es true...
        if (lng) {
            //Obtenemos la información del usuario y grupo.
            struct passwd *userinfo=getpwuid(fstats.st_uid);
            struct group *usergroupinfo=getgrgid(fstats.st_gid);
            //Así mismo, también la fecha de modificación del fichero o directorio.
            struct tm *creationTime=localtime(&fstats.st_mtime);
            strftime(crtnaccTime,sizeof(crtnaccTime),"%Y/%m/%d-%H:%M",creationTime);
            //Si acc es true...
            if (acc) {
                //Sobreescribimos crtnaccTime con la última fecha de acceso.
                struct tm *accesTime=localtime(&fstats.st_atime);
                strftime(crtnaccTime,sizeof(crtnaccTime),"%Y/%m/%d-%H:%M",accesTime);
            }
            //Creamos el string con toda la información necesaria al usar -long.
            sprintf(lngstr,"%s %ld (%lu) %s %s %s %ld %s",crtnaccTime,fstats.st_nlink,fstats.st_ino,usergroupinfo->gr_name,userinfo->pw_name,ConvierteModo(fstats.st_mode),fstats.st_size,directory);
            //Si link está a true...
            if (link) {
                char buffer[sizeof (directory)];
                ssize_t len;
                //Obtenemos el fichero al que apunta el enlace.
                memset(buffer,0,sizeof(buffer));
                len=readlink(directory,buffer,sizeof (directory));
                //Si se puede leer ( len != -1) almacenamos en linkstr el enlace.
                if(len!=-1) sprintf(linkstr,"-> %s",buffer);
                //Si no, almacenamos \0.
                else strcpy(linkstr,"\0");
                //Imprimimos lngstr junto con linkstr.
                printf("%s %s\n",lngstr,linkstr);
            //Si link es false, imprimimos solo lngstr.
            } else printf("%s\n",lngstr);
        //Si ninguna de las flags está activa, imprimimos solo el tamaño del fichero/directorio y su nombre.
        } else printf("%ld %s\n",fstats.st_size,directory);
    //Si no se puede acceder al fichero o directorio imprimimos un error.
    } else printf("Can not acces file/directory: %s\n",directory);
}

void Cmd_stat(char *tr[]) {
    int i=1;
    //Inicializamos las flags a false.
    bool lng=false,link=false,acc=false;
    //Si el primer elemendo del array es NULL mostramos el directorio actual.
    if (tr[1]==NULL){
        Chdir(NULL);
        return;
    }
    //Comprobamos el array buscando las flags que hay que activar.
    while (tr[i]!=NULL && strcmp(tr[i],"\0")!=0) {
        if (strcmp("-long",tr[i])==0) lng=true;
        else if (strcmp("-link",tr[i])==0) link=true;
        else if (strcmp("-acc",tr[i])==0) acc=true;
        else break;
        i++;
    }
    //Para cada elemento de tr después de las flags...
    while (tr[i]!=NULL && strcmp(tr[i],"\0")!=0) {
        //Llamamos a la función stat para que haga su funcionalidad.
        CMD_stat_aux(tr[i],lng,link,acc);
        i++;
    }
}

//Función auxiliar con la funcionalidad de la función list para facilitar el uso de esta más adelante.
void list_aux(char *directory,bool hid,bool reca,bool recb,bool lng,bool link,bool acc) {
    DIR *dir;
    struct dirent *entry;
    char current_dir[MAX_LENGTH],aux_dir[MAX_LENGTH+2];
    struct stat fstat;
    //Obtengo la información del directorio actual y del fichero al que quiero hacer el comando list.
    getcwd(current_dir,sizeof(current_dir));
    if(lstat(directory,&fstat)==0){
        //Si recb está activado...
        if (recb) {
            //Si lo que se pasa de entrada es fichero, retornamos.
            if (LetraTF(fstat.st_mode)!='d') return;
            //Si es un directorio vacio.
            if (isEmptyDir(directory)) {
                //Imprimimos la cabecera de este.
                printf("\n********** %s **********\n",directory);
                //Si hid está activo, llamamos a stat con el directorio actual y retornamos.
                if (hid) CMD_stat_aux(directory,lng,link,acc);
                return;
            }
            //Abrimos el directorio si no está vacío ni es fichero.
            if((dir=opendir(directory))==NULL) return;
            //Se van leyendo todas las entradas del directorio.
            while ((entry = readdir(dir)) != NULL){
                //Si son las entradas . o .. pasamos a la siguiente iteración del bucle.
                if ((strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0 )) continue;
                //Si hid es false y la entrada es un directorio oculto, saltamos a la siguiente iteración del bucle.
                if (entry->d_name[0]=='.' && !hid) continue;
                //Almacenamos la ruta en un string auxiliar.
                sprintf(aux_dir,"%s/%s",directory,entry->d_name);
                //Llamamos de forma recursiva a pasandole la ruta como argumento.
                list_aux(aux_dir,hid,reca,recb,lng,link,acc);
            }
            //Cerramos el directorio.
            closedir(dir);
        }
        //Si lo que se pasa de entrada es un directorio...
        if (LetraTF(fstat.st_mode) == 'd') {
            //Abrimos el directorio.
            if((dir=opendir(directory))==NULL) return;
            //Imprimimos la cabecera del directorio.
            printf("\n********** %s **********\n",directory);
            //Cambiamos al directorio.
            if(chdir(directory)!=0) return;
            //Leemos todas las entradas.
            while ((entry=readdir(dir))!=NULL) {
                //Si es una entrada oculta y hid es false, saltamos a la siguiente iteración del bucle.
                if (entry->d_name[0]=='.' && !hid) continue;
                //Hacemos la stat de la entrada actual.
                CMD_stat_aux(entry->d_name,lng,link,acc);
            }
            //Volvemos al directorio en el que estábamos y cerramos el directorio.
            chdir(current_dir);
            closedir(dir);
        }
        //Si reca está activado...
        if (reca) {
            //Si la entrada es un directorio retornamos.
            if (LetraTF(fstat.st_mode)!='d') return;
            //Abrimos el directorio.
            if((dir=opendir(directory))==NULL) return;
            //Recorremos las entradas del directorio.
            while ((entry=readdir(dir))!=NULL) {
                //Si son las entradas . o .. pasamos a la siguiente iteración del bucle.
                if ((strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)) continue;
                //Si hid es false y la entrada es un directorio oculto, saltamos a la siguiente iteración del bucle.
                if (entry->d_name[0]=='.' && !hid) continue;
                //Almacenamos la ruta en un string auxiliar.
                sprintf(aux_dir,"%s/%s",directory,entry->d_name);
                //Llamamos de forma recursiva a pasandole la ruta como argumento.
                list_aux(aux_dir,hid,reca,recb,lng,link,acc);
            }
            //Cerramos el directorio.
            closedir(dir);
        }
    } else return;
}

void list(char *tr[]) {
    int i = 1;
    //Inicializamos las flags a false.
    bool hid = false, reca = false, recb = false, lng = false, link = false, acc = false;
    struct stat fstat;
    //Si el primer elemendo del array es NULL mostramos el directorio actual.
    if (tr[1]==NULL){
        Chdir(NULL);
        return;
    }
    //Comprobamos el array buscando las flags que hay que activar.
    while (tr[i]!=NULL && strcmp(tr[i],"\0")!=0) {
        if (strcmp("-long",tr[i])==0) lng=true;
        else if (strcmp("-link",tr[i])==0) link=true;
        else if (strcmp("-acc",tr[i])==0) acc=true;
        else if (strcmp("-reca",tr[i])==0){
            reca=true;
            recb=false;
        }
        else if (strcmp("-recb",tr[i])==0) {
            recb=true;
            reca=false;
        }
        else if (strcmp("-hid",tr[i])==0) hid=true;
        else break;
        i++;
    }
    //Para cada elemento de tr después de las flags...
    while (tr[i]!=NULL && strcmp(tr[i],"\0")!=0) {
        stat(tr[i],&fstat);
        //Si la entrada es un directorio, hacemos la stat.
        if (LetraTF(fstat.st_mode)!='d') CMD_stat_aux(tr[i],lng,link,acc);
        //Si no, llamamos a la auxiliar.
        else list_aux(tr[i],hid,reca,recb,lng,link,acc);
        i++;
    }
}

void delete_aux(char *directory) {
    struct stat fstat;
    stat(directory,&fstat);
    //Si la entrada es un directorio...
    if (LetraTF(fstat.st_mode)=='d') {
        //Verifica si está vacío.
        if (isEmptyDir(directory)) {
            //Si lo está lo cierra.
            rmdir(directory);
            printf("Removed directory: %s\n",directory);
        }
        //Si no imprime un error.
        else perror("Not an empty directory.");
    }
    //Si se puede eliminar el fichero, se elimina, si se imprime un error.
    else if(remove(directory) == 0) {
        printf("Removed file: %s\n", directory);
    } else perror("Specify a valid file or directory.");
}

void deltree_aux(char *directory) {
    struct stat fstat;
    struct dirent *entry;
    DIR *dir;
    char aux_dir[MAX_LENGTH+3];
    stat(directory,&fstat);
    //Si la entrada es un directorio...
    if (LetraTF(fstat.st_mode)=='d') {
        //Comprobamos si está vacío.
        if (isEmptyDir(directory)){
            //Si está vacío lo eliminamos.
            rmdir(directory);
            printf("Removed directory: %s\n", directory);
        }
        else {
            //Si no está vacío lo abrimos.
            dir = opendir(directory);
            while ((entry = readdir(dir))!=NULL) {
                //Se omiten las entradas . y ..
                if ((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )) continue;
                //Se almacena la ruta en un string.
                sprintf(aux_dir,"%s/%s",directory,entry->d_name);
                //Se llama recursivamente a la función.
                deltree_aux(aux_dir);
            }
            //Una vez vaciado el directorio, se llama a deltree recursivamente y se elimina.
            deltree_aux(directory);
        }
    }
    //Si es un fichero se elimina, si no se imprime un error.
    else if(remove(directory)==0){
        printf("Removed file: %s\n", directory);
        return;
    } else perror("Specify a valid file or directory.");
}

void delete(char *tr[]) {
    //Si la primera posición del array es NULL o \0
    if (tr[1]==NULL || tr[1][0]=='\0') {
        //Imprimimos un error.
        perror("Specify a valid file or directory.");
        return;
    }
    //Si especificamos que queremos usar la función deltree, la ejecutamos.
    if(strcmp(tr[0],"deltree")==0) for (int i=1;tr[i]!=NULL && strcmp(tr[i],"\0")!=0;i++) deltree_aux(tr[i]);
    //Si no llamamos a delete.
    else for (int i=1;tr[i]!=NULL && strcmp(tr[i],"\0")!=0;i++) delete_aux(tr[i]);
}

void malloc_aux(char* size,List *MemoryList) {
    void *mem_assigned;
    mem_assigned = malloc(atoi(size));
    if (mem_assigned == NULL){
        perror("Error allocating memory");
        return;
    }
    printf("Assigned %s bytes on %p\n",size,mem_assigned);
    insertMallocToMemoryList(mem_assigned,atoi(size),MemoryList);
}

void freeAux(char* type, int num, List *MemoryList){
    if (strcmp(type,"Malloc") == 0){
        Position pos = findItemStr(type,num,2,*MemoryList,true);
        if(pos==NULL){
            perror("Not found");
            return;
        }
        void *mem=cadtop(getItem(pos).data[0]);
        printf("Deallocated %d bytes from %p\n",num,mem);
        deleteAtPosition(pos,MemoryList);
        free(mem);
        return;
    }
    if (strcmp(type,"Shared")==0){
        char numb[50];
        sprintf(numb,"%d",num);
        Position pos = findItemStr(numb,num,4,*MemoryList,false);
        if(pos==NULL){
            perror("Not found");
            return;
        }
        void *mem=cadtop(getItem(pos).data[0]);
        if(mem==NULL) perror("Can not deallocate");
        else{
            printf("Deallocated %d bytes from %p\n",num,mem);
            deleteAtPosition(pos,MemoryList);
            if (shmdt(mem)==-1) {
                perror("Can not free memory.");
                return;
            }
        }
        return;
    }
    if (strcmp(type,"Mmap")==0){
        char numb[50];
        sprintf(numb,"%d",num);
        Position pos = findItemStr(numb,num,3, *MemoryList,false);
        if(pos==NULL){
            perror("Not found.");
            return;
        }
        //Obtener información sobre el archivo (tamaño, permisos, etc.)
        struct stat file_stat;
        if (fstat(num, &file_stat)==-1) {
            perror("Error obtaining file info.");
            return;
        }
        void *mem=cadtop(getItem(pos).data[0]);
        close(atoi(getItem(pos).data[3]));
        if (munmap(mem, file_stat.st_size) == -1) {
            perror("Error unmapping memory.");
            return;
        }
        printf("Deallocated memory mapped at %p\n", mem);
        deleteAtPosition(pos, MemoryList);
    }
}

void Cmd_malloc(char *tr[],List *MemoryList) {
    bool free=false;
    if (tr[1]==NULL || tr[1][0]=='\0'){
        printListWithFilter(*MemoryList,"Malloc");
        return;
    }
    if(strcmp(tr[1],"-free")==0 && tr[2]!=NULL && tr[2][0]!='\0') free=true;
    else if (!isNumber(tr[1])){
        perror("Not a valid option");
        return;
    }
    if(!free) malloc_aux(tr[1],MemoryList);
    else freeAux("Malloc",atoi(tr[2]),MemoryList);
}

void SharedCreate (char *tr[],List *MemoryList)
{
    key_t cl;
    size_t tam;
    void *p;

    if (tr[2]==NULL || tr[3]==NULL) {
        printListWithFilter(*MemoryList,"Shared");
        return;
    }

    cl=(key_t)  strtoul(tr[2],NULL,10);
    tam=(size_t) strtoul(tr[3],NULL,10);
    if (tam==0) {
        printf ("Can not assign blocks of 0 bytes.\n");
        return;
    }
    if ((p=ObtenerMemoriaShmget(cl,tam,MemoryList,true))!=NULL)
        printf ("Assigned %lu bytes on %p\n",(unsigned long) tam, p);
    else
        printf ("Can not assign shared memory with key %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void SharedDelkey (char *args[])
{
    key_t clave;
    int id;
    char *key=args[0];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        perror("Invalid key.\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: can not obtain shared memory");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: can not delete id of shared memory\n");
}

// Función para asignar/desasignar bloques de memoria compartida
void shared(char *tr[],List *MemoryList) {
    // Verificar si se proporcionan suficientes argumentos
    if (tr[1] == NULL) {
        printListWithFilter(*MemoryList,"Shared");
        return;
    }
    // Obtener la operación solicitada (free, create o delkey)
    char *operacion = tr[1];
    if(isNumber(operacion)){
        Position pos = findItemStr(operacion,0,4,*MemoryList,false);
        if (pos != NULL){
            key_t key=(key_t)  strtoul(operacion,NULL,10);
            size_t size = (size_t) getItem(pos).posNum;
            void *mem = ObtenerMemoriaShmget(key,size,MemoryList,false);
            if (mem!=NULL){
                printf ("Shared memory with key %d assigned on %p\n",key, mem);
                return;
            }
            else
                printf ("Can not assign shared memory with %lu:%s\n",(unsigned long) key,strerror(errno));
            return;
        } else{
            perror("Key not found");
            return;
        }
    }
    // Operación: create(mirar codigo ayuda sharedcreate)
    if (strcmp(operacion, "-create") == 0) {
        SharedCreate(tr,MemoryList);
        return;
    }
        // Operación: free
    if (strcmp(operacion, "-free") == 0) {
        // Verificar que se proporcionen suficientes argumentos
        if (tr[2] == NULL) {
            perror("Not a valid option\n");
            return;
        }
        freeAux("Shared", atoi(tr[2]),MemoryList);
    }
        // Operación: delkey(Mirar codigo ayuda shareddelkey)
    else if (strcmp(operacion, "-delkey") == 0) {
        SharedDelkey(tr+2);
    }
        // Operación desconocida
    else {
        printListWithFilter(*MemoryList,"Shared");
        return;
    }
}

void * MapearFichero (char * fichero, int protection, List *MemoryList)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
    insertMmapToMemoryList(p,fichero,df,s.st_size,MemoryList);
    return p;
}

void my_mmap(char *tr[],List *MemoryList) {
    char *mapped_data;
    // Obtener los parámetros de la línea de comandos
    if(tr[1]==NULL){
        printListWithFilter(*MemoryList,"descriptor:");
        return;
    }
    if(strcmp(tr[1],"-free")==0){
        if(tr[2]==NULL){
            perror("Can not free.");
            return;
        }
        int df;
        Position pos = findItemStr(tr[2],0,5,*MemoryList,false);
        df=atoi(getItem(pos).data[3]);
        freeAux("Mmap",df, MemoryList);
        return;
    }
    char *permissions=tr[2];
    // Establecer la protección en función de los permisos
    int protection=0;
    if (permissions!=NULL){
        for(int i=0;permissions[i]!='\0';i++){
            if(permissions[i]=='r'){
                protection|=PROT_READ;
            }
            if(permissions[i]=='w'){
                protection|=PROT_WRITE;
            }
            if(permissions[i]=='x'){
                protection|=PROT_EXEC;
            }
        }
        mapped_data=MapearFichero(tr[1],protection,MemoryList);
    } else {
        mapped_data=MapearFichero(tr[1],PROT_NONE,MemoryList);
    }
    // Mapear el archivo en memoria
    if(mapped_data==NULL){
        perror("Can not map file.");
        return;
    }
    else{
        // Abrir el archivo en modo lectura/escritura
        int fd=open(tr[1],O_RDWR);
        if (fd==-1) {
            perror("Error opening file.");
            return;
        }
        // Obtener información sobre el archivo (tamaño, permisos, etc.)
        struct stat file_stat;
        if (fstat(fd, &file_stat)==-1) {
            perror("Error obtaining file info.");
            return;
        }
        printf("File: %s succesfully mapped on %p\n",tr[1],mapped_data);
        close(fd);
    }
}

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

void Cmd_read(char *tr[]) {
    void *p;
    size_t cont=-1;  /* -1 indica leer todo el fichero*/
    ssize_t n;

    //Se verifica que haya al menos dos argumentos
    if (tr[1]==NULL || tr[2]==NULL){
        perror("Missing arguments\n");
        return;
    }

    // Convertimos una cadena a un puntero
    p=cadtop(tr[2]);

    // Si hay un tercer argumento, lo convertimos a size_t y lo asignamos a cont
    if (tr[3]!=NULL)
        cont=(size_t) atoll(tr[3]);

    // Llamada a la función LeerFichero para leer el archivo
    if ((n=LeerFichero(tr[1],p,cont))==-1)
        perror ("Can not read file");
    else
        printf ("Read %lld bytes of %s on %p\n",(long long) n,tr[1],p);
}

ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite)
{
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;

    if ((df=open(f,flags,0777))==-1)
        return -1;

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

void Cmd_write(char *tr[]) {
    int over = -1;
    size_t size;
    void *mem;
    char f[100];
    unsigned long direccion_num;
    if(tr[1]==NULL || tr[1][0]=='\0'){
        perror("Missing arguments");
        return;
    }
    if(strcmp(tr[1],"-o")==0) over = 1;
    if(over==1){
        if(tr[4]==NULL || tr[4][0]=='\0' || tr[3]==NULL || tr[3][0]=='\0' || tr[2]==NULL || tr[2][0]=='\0'){
            perror("Missing arguments.");
            return;
        }
        sprintf(f,"%s",tr[2]);
        direccion_num = strtoul(tr[3], NULL, 16);
        size = (size_t)strtoul(tr[4], NULL, 10);
    } else {
        if(tr[3]==NULL || tr[3][0]=='\0' || tr[2]==NULL || tr[2][0]=='\0'){
            perror("Missing arguments.");
            return;
        }
        sprintf(f,"%s",tr[1]);
        direccion_num = strtoul(tr[2], NULL, 16);
        size = (size_t)strtoul(tr[3], NULL, 10);
    }
    mem = (void *)direccion_num;
    printf("Trying to write %zu bytes from direction %p on file %s...\n",size,mem,f);
    if(EscribirFichero(f,mem,size,over)==-1){
        perror("Error writing file.");
        return;
    }
    printf("Successfully wrote.\n");
}

void memdump(char *tr[]) {
    int n;
    if (tr[1] == NULL) {
        perror("Missing arguments.");
        return;
    }
    unsigned long direccion_num = strtoul(tr[1], NULL, 16);
    void *p = (void *)direccion_num;
    unsigned char *mem = (unsigned char *)p;
    if(tr[2] == NULL || tr[2][0] == '\0') n = 25;
    else n = atoi(tr[2]);
    printf("Dumping %d bytes from %p:\n",n,p);
    for (size_t i = 0; i < n; i++) {
        if(mem[i] == '\n') printf("%-1s", "\n");
        else if(isprint(mem[i])) printf("%-3c", mem[i]);
        else printf("%-3c",' ');
    }
    printf("\n");
    for (size_t i = 0; i < n; i++) {
        printf("%02X ", mem[i]);
    }
    printf("\n");
}

void LlenarMemoria (void *p, size_t cont, unsigned char byte)
{
    unsigned char *arr=(unsigned char *) p;
    size_t i;
    for (i=0; i<cont;i++)
        arr[i]=byte;
}

void memfill(char *tr[]) {
    if (tr[3] == NULL || tr[3][0] == '\0'||tr[2] == NULL || tr[2][0] == '\0'||tr[1] == NULL || tr[1][0] == '\0'){
        perror("Invalid input");
        return;
    } else {
        unsigned long direccion_num = strtoul(tr[1], NULL, 16);
        void *mem = (void *)direccion_num;
        size_t size;
        size = (size_t)strtoul(tr[2], NULL, 10);
        if(isNumber(tr[3])){
            unsigned long byte = strtoul(tr[3], NULL, 10);
            LlenarMemoria(mem, size, (unsigned char) byte);
            printf("Assingned %zu bytes with byte %c %02lX on direction %p\n",size,(unsigned char) atoi(tr[3]),byte,mem);
        }else {
            int c1=-1,c2=-1;
            for(int i=0; tr[3][i]!='\0'; i++){
                if(tr[3][i] == '\'' && c1 == -1){
                    c1 = i;
                } else if (tr[3][i] == '\'' && c2 == -1){
                    c2 = i;
                }
            }
            if (c1 == -1 || c2 == -1){
                LlenarMemoria(mem, size, 00);
                printf("Assingned %zu bytes with byte %s %d on direction %p\n",size,"",00,mem);
                return;
            }
            if (c2 - c1 != 2){
                LlenarMemoria(mem, size, 00);
                printf("Assingned %zu bytes with byte %s %d on direction %p\n",size,"",00,mem);
                return;
            }
            unsigned char bytec = tr[3][c1+1];
            LlenarMemoria(mem, size, bytec);
            printf("Assingned %zu bytes with byte %c %d on direction %p\n",size,bytec,bytec,mem);
        }
    }
}

void Do_MemPmap (void) /*sin argumentos*/
{ pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};

    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Can not create process.");
        return;
    }
    if (pid==0){ /*proceso hijo*/
        if (execvp(argv[0],argv)==-1)
            perror("Can not execute pmap (linux, solaris).");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("Can not execute vmmap (Mac-OS).");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("Can not execute procstat (FreeBSD).");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("Can not execute procmap (OpenBSD).");

        exit(1);
    }
    waitpid (pid,NULL,0);
}

void FunctionsPrint(){
    printf("Program functions:     %p %p %p\n", dateANDtime, pid, infosys);
    printf("Library functions:     %p %p %p\n", perror, printf, strcpy);
}

int global1=0,global2=0,global3=0;
int globalni1,globalni2,globalni3;

void VarPrint(){
    static int s1=0,s2=0,s3=0;
    static int sni1,sni2,sni3;
    int l1=0,l2=0,l3=0;
    printf("Local  variables:      %p %p %p\n",&l1,&l2,&l3);
    printf("Global variables:      %p %p %p\n", &global1, &global2, &global3);
    printf("Global variables(N.I): %p %p %p\n", &globalni1, &globalni2, &globalni3);
    printf("Static variables:      %p %p %p\n", &s1, &s2, &s3);
    printf("Static variables(N.I): %p %p %p\n", &sni1, &sni2, &sni3);
}

void mem(char *tr[],List MemoryList) {
    if (tr[1] == NULL || tr[1][0]=='\0'){
        VarPrint();
        FunctionsPrint();
        printList(LNULL,MemoryList,2);
        return;
    }
    if(strcmp(tr[1],"-pmap")==0){
        Do_MemPmap();
        return;
    }
    if(strcmp(tr[1],"-vars")==0){
        VarPrint();
        return;
    }
    if(strcmp(tr[1],"-funcs")==0){
        FunctionsPrint();
        return;
    }
    if(strcmp(tr[1],"-blocks")==0){
        printList(LNULL,MemoryList,2);
        return;
    }
    if(strcmp(tr[1],"-all")==0){
        VarPrint();
        FunctionsPrint();
        printList(LNULL,MemoryList,2);
        return;
    }
    perror("Invalid Argument.");
}

void Recursiva (int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];
    printf ("Parameter:%3d(%p) Array %p, Static arr %p\n",n,&n,automatico, estatico);
    if (n>0)
        Recursiva(n-1);
}

void recurse(char *tr[]) {
    if(tr[1] == NULL || tr[1][0] == '\0'){
        perror("Missing argument.");
        return;
    }
    if(!isNumber(tr[1])){
        perror("Invalid argument.");
        return;
    }
    Recursiva(atoi(tr[1]));
}

void printUID(uid_t uid){
    struct passwd *usr;
    //Si la información de los usuarios se puede obtener
    if((usr=getpwuid(uid))!=NULL){
        //Imprimimos las credenciales.
        printf("CREDENTIAL: %u (%s)\n",uid,usr->pw_name);
    } else perror("Error obtaining user credentials.");
}

void setUidLogin(char *usr){
    struct passwd *usrinfo;
    if((usrinfo=getpwnam(usr))!=0){
        if(seteuid(usrinfo->pw_uid)!=0) perror("Can not change credential.");
        else printf("Successfully changed credentials.\n");
    } else perror("Can not change credentials");
}

void uid(char *tr[]){
    //Comprobamos la flag
    if(tr[1]==NULL || tr[1][0]=='\0' || strcmp(tr[1],"-get")==0){
        //Obtenemos las uid y las pasamos a la función que imprime las uid
        printf("REAL ");
        printUID(getuid());
        printf("EFFECTIVE ");
        printUID(geteuid());
        return;
    }
    if(strcmp(tr[1],"-set")==0){
        if(strcmp(tr[2],"-l")==0){
            if (tr[3]!=NULL && tr[3][0]!='\0'){
                setUidLogin(tr[3]);
            }
            return;
        }
        if (tr[2]!=NULL && tr[2][0]!='\0' && isNumber(tr[2])){
            if(seteuid(atoi(tr[2]))==0){
                printf("Successfully changed credentials.\n");
            } else perror("Can not change credentials");
        } else perror("Not a valid id.");
        return;
    }
}

void printArg3(char *tr[],char *arg3[]){
    int pos = BuscarVariable(tr[1],arg3);
    if(pos!=-1){
        printf("With main arg3 %s(%p) @%p\n",arg3[pos],(void *)arg3[pos],(void *)&arg3[pos]);
    }else perror("Can not obtain using main arg3.");
}

void printEnviron(char *tr[]){
    int pos = BuscarVariable(tr[1],environ);
    if(pos!=-1){
        printf("With environ %s(%p) @%p\n",environ[pos],(void *)environ[pos],(void *)&environ[pos]);
    }else perror("Can not obtain using environ.");
}

void printGetEnv(char *tr[]){
    char *env = getenv(tr[1]);
    if(env!=NULL){
        printf("With getenv %s(%p)\n",env,(void *)env);
    }else perror("Can not obtain using getenv.");
}

void printEnv(char *e[],int id){
    char envTipe[15];
    switch (id) {
        case 1:
            strcpy(envTipe,"main arg3");
            break;
        case 2:
            strcpy(envTipe,"environ");
            break;
        default:
            perror("Not a valid id");
            return;
    }
    for(int i=0;e[i]!=NULL;i++){
        printf("%p -> %s[%d] = %p %s\n",(void *) e[i],envTipe,i,(void *) &e[i],e[i]);
    }
}

void showvar(char *tr[],char *arg3[]){
    if (tr[1]!=NULL && tr[1][0]!='\0'){
        printArg3(tr,arg3);
        printEnviron(tr);
        printGetEnv(tr);
    }else printEnv(arg3,1);
}

void changevar(char *tr[],char *arg3[],List *EnvList){
    if (tr[1]!=NULL && tr[1][0]!='\0' && tr[2]!=NULL && tr[2][0]!='\0' && tr[3]!=NULL && tr[3][0]!='\0'){
        if(strcmp(tr[1],"-a")==0){
            int val = CambiarVariable(tr[2],tr[3],arg3);
            if(val==-1) perror("Error changing variable.");
            return;
        }
        if(strcmp(tr[1],"-e")==0){
            if(CambiarVariable(tr[2],tr[3],environ)==-1) perror("Error changing variable.");
            return;
        }
        if(strcmp(tr[1],"-p")==0){
            char *enviVar=malloc(strlen(tr[2])+strlen(tr[3])+2);
            insertEnvVar((void *)enviVar,EnvList);
            sprintf(enviVar, "%s=%s",tr[2],tr[3]);
            if(putenv(enviVar)!=0) perror("Error changing variable.");
            return;
        }
        perror("Not a valid argument.");
    }else perror("Input an argument.");
}

void subsvar(char *tr[],char *arg3[]){
    if (tr[1]!=NULL && tr[1][0]!='\0' && tr[2]!=NULL && tr[2][0]!='\0' && tr[3]!=NULL && tr[3][0]!='\0' && tr[4]!=NULL && tr[4][0]!='\0'){
        if(strcmp(tr[1],"-a")==0){
            if(SustituirVariable(tr[2],tr[3],tr[4],arg3)==-1) perror("Error changing variable.");
            return;
        }
        if(strcmp(tr[1],"-e")==0){
            if(SustituirVariable(tr[2],tr[3],tr[4],arg3)==-1) perror("Error changing variable.");
            return;
        }
        return;
    }else perror("Input an argument.");
}

void showenv(char *tr[],char *arg3[]){
    if (tr[1]!=NULL && tr[1][0]!='\0'){
        if(strcmp(tr[1],"-environ")==0){
            printEnv(environ,2);
            return;
        }
        if(strcmp(tr[1],"-addr")==0){
            printf("environ  : %p (stored in %p)\n",(void *)environ,(void *)&environ);
            printf("main arg3: %p (stored in %p)\n",(void *)arg3,(void *)&arg3);
            return;
        }
        printEnv(arg3,1);
    }else printEnv(arg3,1);
}

//Sacada del código de ayuda
void Cmd_fork(List *p){
        pid_t pid;

        if ((pid=fork())==0){
            deleteList(p);
            printf ("ejecutando proceso %d\n", getpid());
        }
        else if (pid!=-1)
            waitpid (pid,NULL,0);
}

void Cmd_exec(char *tr[]){
    if(execvp(tr[1],tr+1)!=0) perror("Can not execute.");
}

void jobs(List *ProcessList){
    Position Pos1=first(*ProcessList);
    while(Pos1!=NULL){
        updateProcessList(&Pos1->item);
        Pos1=Pos1->next;
    }
    printList(NULL,*ProcessList,3);
}

void deljobs(char *tr[],List *ProcessList){
    bool deleteTerm=false,deleteSig=false;
    int i=0;
    Position Pos1=first(*ProcessList);
    if(tr[1]==NULL || tr[1][0]=='\0'){
        jobs(ProcessList);
        return;
    }
    else{
        while(tr[i]!=NULL && tr[i][0]!='\0'){
            if(strcmp(tr[i],"-term")==0) deleteTerm=true;
            if(strcmp(tr[i],"-sig")==0) deleteSig=true;
            i++;
        }
        if(!deleteSig && !deleteTerm){
            jobs(ProcessList);
            return;
        }
    }
    while(Pos1!=NULL){
        if(deleteTerm){
            if(strcmp(Pos1->item.data[3],"TERMINATED")==0) deleteAtPosition(Pos1,ProcessList);
        }
        if(deleteSig){
            if(strcmp(Pos1->item.data[4],"000")!=0) deleteAtPosition(Pos1,ProcessList);
        }

        Pos1=Pos1->next;
    }
}

void job(char *tr[],List *ProcessList){
    int i=1;
    bool fg=false;
    Position Pos1=first(*ProcessList);
    if(tr[1]==NULL || tr[1][0]=='\0'){
        jobs(ProcessList);
        return;
    }
    if(strcmp(tr[1],"-fg")==0){
        fg=true;
        i=2;
    }
    while(tr[i]!=NULL && tr[i][0]!='\0'){
            while(Pos1!=NULL){
                if(atoi(tr[i])==Pos1->item.posNum){
                    if(!fg){
                        updateProcessList(&Pos1->item);
                        printItem(Pos1->item);
                    }else{
                        waitpid(Pos1->item.posNum,NULL,0);
                        deleteAtPosition(Pos1,ProcessList);
                    }
                }
                Pos1=Pos1->next;
            }
            i++;
    }
}

void execForeground(char *tr[],bool background,cmd *cmds,List *h,List *f,List *m,List *p){
    pid_t pid=fork();
    if(pid<0) perror("Input a valid command.");
    else if (pid==0) {
        if(execvp(tr[0],tr)==-1){
            perror("Input a valid command.");
            free(cmds);
            deleteList(h);
            deleteFileList(f);
            deleteList(p);
            deleteMemList(m);
            exit(0);
        }
    } else {
        if(!background) waitpid(pid,NULL,0);
        else insertProcessToList(tr,p,pid);
    }

}

void execExternal(char *tr[],cmd *cmds,List *h,List *f,List *m,List *p){
    bool background=false;
    for(int i=0;tr[i]!=NULL;i++){
        if(tr[i][0]=='\0'){
            perror("Can not execute with command.");
            return;
        }
        if(tr[i+1]==NULL){
            if(tr[i][0]=='&' && tr[i][1]=='\0'){
             background=true;
             tr[i]=NULL;
            }
        }
    }
    execForeground(tr,background,cmds,h,f,m,p);
}