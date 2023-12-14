#include "List.h"
#include "stdio.h"
#include "unistd.h"
#include "cmnds.h"

/////////////FUNCIONES BÁSICAS PARA OPERAR CON EL TAD/////////////

bool createNode(Position *PNodo){
    //Función que crea nodos, necesaria para operar con la lista ya que es una sucesión de nodos.
    *PNodo=malloc(sizeof(struct Node));
    return *PNodo!=LNULL;
}

void createEmptyList(List *list){
    //Creación de una lista vacía asignándole un valor NULL al puntero lista.
    *list=LNULL;
}

void deleteList(List *list) {
    Position actual = *list;
    Position next;
    //Bucle que va liberando de la memoria cada posición de la lista.
    while (actual!=NULL) {
        next=actual->next;
        free(actual);
        actual=next;
    }
    //Se le asigna NULL al puntero de la lista.
    *list=NULL;
}

void deleteListVars(List *list){
    Position actual = *list;
    Position next;
    //Bucle que va liberando de la memoria cada posición de la lista.
    while (actual!=NULL) {
        next=actual->next;
        free(cadtop(actual->item.data[0]));
        free(actual);
        actual=next;
    }
    //Se le asigna NULL al puntero de la lista.
    *list=NULL;
}

void deleteFileList(List *list) {
    Position actual = *list;
    Position next;
    //Bucle que va liberando de la memoria cada posición de la lista.
    while (actual!=NULL) {
        if(actual->item.posNum!=0 && actual->item.posNum!=1 && actual->item.posNum!=2){
            close(actual->item.posNum);
            printf("Closing file %s with descriptor %d and perms ",actual->item.data[0],actual->item.posNum);
            for (int i=1;actual->item.data[i][0]!='\0';++i) {
                printf("%s ",actual->item.data[i]);
            }
            printf("\n");
        }
        next=actual->next;
        free(actual);
        actual=next;
    }
    //Se le asigna NULL al puntero de la lista.
    *list=NULL;
}

void deleteMemList(List *list) {
    Position actual = *list;
    Position next;
    //Bucle que va liberando de la memoria cada posición de la lista.
    while (actual!=NULL) {
        if(actual->next==NULL){
            if(strcmp(actual->item.data[2],"Malloc")==0){
                freeAux("Malloc", actual->item.posNum,list);
                return;
            }
            if(strcmp(actual->item.data[2],"Shared")==0){
                freeAux("Shared", atoi(actual->item.data[4]),list);
                return;
            }
            if(strcmp(actual->item.data[2],"descriptor:")==0){
                freeAux("Mmap", atoi(actual->item.data[3]),list);
                return;
            }
            return;
        }else {
            if(strcmp(actual->item.data[2],"Malloc")==0){
                freeAux("Malloc", actual->item.posNum,list);
            }
            if(strcmp(actual->item.data[2],"Shared")==0){
                freeAux("Shared", atoi(actual->item.data[4]),list);
            }
            if(strcmp(actual->item.data[2],"descriptor:")==0){
                freeAux("Mmap", atoi(actual->item.data[3]),list);
            }
        }
        next=actual->next;
        actual=next;
    }
    //Se le asigna NULL al puntero de la lista.
    *list=NULL;
}

void deleteAtPosition(Position Pos,List *list) {
    Position Pos2;
    //Si la posicion es la primera de la lista, el puntero list, apunta al siguiente nodo.
    if (*list==Pos) *list=(*list)->next;
        //Si no es la primera, se comprueba si es la segunda y el puntero list pasa a apuntar al siguiente de la posicion introducida
    else if ((*list)->next==Pos) (*list)->next=Pos->next;
    else{
        //Si no se cumple ninguna de estas condiciones especiales:
        //Se crea un bucle que recorre la lista desde el principio hasta la posición introducida
        for(Pos2=*list;Pos2!=LNULL && Pos2->next!=Pos;Pos2=Pos2->next);
        //Si se encuentra la posición introducida, Pos2->Next apunta a Pos->Next
        if (Pos2!=LNULL) Pos2->next=Pos->next;
    }
    //Se libera el espacio en memoria de la función Pos y pasa a ser NULL
    free(Pos);
    Pos=LNULL;
}

bool isEmptyList(List list) {
    //Comprobación si el primer valor de la lista contiene un NULL, por lo tento está vacía.
    return list==LNULL;
}

Position first(List list) {
    //Si la lista no está vacía, devuelve la primera posición.
    return list;
}

Position last(List list) {
    Position Pos;
    //Bucle que recorre toda la lista hasta que la posición Pos2 sea igual a la última de la lista.
    for(Pos=list;(Pos->next!=LNULL);Pos=Pos->next);
    return Pos;
}

bool insertToList(Item command,Position Pos,List *list) {
    Position Pos1,Pos2;
    //Si no se puede crear un nodo, la función retorna falso y no inserta nada.
    if(!createNode(&Pos1)) return false;
    else{
        //Si no, la variable Pos1 va a almacenar el comando a insertar y su siguiente posición pasa a ser NULL.
        Pos1->item=command;
        Pos1->next=LNULL;
        //Si la lista está vacía, el primer elemento será el contenido de Pos1.
        if (*list==LNULL) *list=Pos1;
        //Si la posición que pasas es NULL (la última)...
        else if (Pos==LNULL) {
            //Se crea un bucle que recorre toda la lista hasta el último elemento y lo inserta.
            for (Pos2=*list;Pos2->next!=LNULL;Pos2=Pos2->next);
            Pos2->next=Pos1;
        }
        //Si la posición de la lista es la primera...
        else if (Pos==*list) {
            //Se inserta el elemento como primer nodo de la lista
            Pos1->next=*list;
            *list=Pos1;
        }
        else{
            //Si ninguna de estas condiciones especiales se cumple, quiere decir que la posición pertenece a la lista.
            //Por lo que se inserta el item y se desplazan el resto de nodos de la lista posteriores a la posicion indicada.
            Pos1->item=Pos->item;
            Pos->item=command;
            Pos1->next=Pos->next;
            Pos->next=Pos1;
        }
        return true;
    }
}

Item getItem(Position Pos) {
    //Se retorna el comando de la posición indicada.
    return Pos->item;
}

Position findItem(PositionNumber command,List list) {
    Position Pos;
    //Se declara un bucle que va a recorrer toda la lista hasta que encuentre un comando igual al introducido.
    for (Pos=list;(Pos!=LNULL) && (command!=Pos->item.posNum );Pos=Pos->next);
    //Si lo encuentra, se devuelve la posición del comando.
    if (Pos!=LNULL && command==Pos->item.posNum) return Pos;
    //Si no lo encuentra, devuelve nulo.
    else return LNULL;
}

Position findItemStr(char *data_str, int num, int index, List list, bool useNum) {
    Position Pos;
    if(useNum){
        for (Pos=list;(Pos!=LNULL);Pos=Pos->next){
            if (strcmp(data_str,Pos->item.data[index])==0){
                if( num==Pos->item.posNum) return Pos;
            }
        }
        return LNULL;
    }
    //Se declara un bucle que va a recorrer toda la lista hasta que encuentre un comando igual al introducido.
    for (Pos=list;(Pos!=LNULL) && (strcmp(data_str,Pos->item.data[index])!=0 );Pos=Pos->next);
    //Si lo encuentra, se devuelve la posición del comando.
    if (Pos!=LNULL && strcmp(data_str,Pos->item.data[index])==0) return Pos;
        //Si no lo encuentra, devuelve nulo.
    else return LNULL;
}

/////////////FUNCIONES AUXILIARES QUE USAN FUNCIONES BÁSICAS/////////////

void printItem(Item item){
    printf("%d",item.posNum);
    for(int i=0;item.data[i][0]!='\0';i++){
        printf(" %2s",item.data[i]);
    }
    printf("\n");
}

void printList(Position Pos, List list, int code) {
    Position Pos1=first(list);
    char header[50],tipe[15];
    switch (code) {
        case 0:
            sprintf(header,"%s","---COMMAND HISTORY:---");
            sprintf(tipe,"%s","command");
            break;
        case 1:
            sprintf(header,"%s","---FILE LIST:---");
            sprintf(tipe,"%s","descriptor:");
            break;
        case 2:
            sprintf(header,"---BLOQUES ASIGNADOS PARA EL PROCESO %d---",getpid());
            sprintf(tipe,"%s","");
            break;
        case 3:
            sprintf(header,"---PROCESS LIST---");
            sprintf(tipe,"%s","");
            break;
        default:
            break;
    }
    printf("%s\n",header);
    //Se establece un bucle que recorre la lista desde el principio hasta el final.
    while (Pos1!=LNULL) {
        int j = 0;
        if(code == 2){
            printf("- %s ",Pos1->item.data[0]);
            j = 1;
        }
        //Se imprime el comando de la posición de la lista.
        printf("%s %d",tipe,Pos1->item.posNum);
        for (int i = j;Pos1->item.data[i][0]!='\0';i++) printf(" %2s",Pos1->item.data[i]);
        printf("\n");
        //Si el bucle llega a la posición especificada, se detiene.
        if (Pos1==Pos) break;
        Pos1=Pos1->next;
    }
}

void printListWithFilter(List list,char* filter){
    Position Pos1=first(list);
    if(strcmp(filter,"descriptor:")==0) printf("---LIST OF MEMORY (%s) FOR PROCESS %d---\n","Mmap",getpid());
    else printf("---LIST OF MEMORY (%s) FOR PROCESS %d---\n",filter,getpid());
    //Se establece un bucle que recorre la lista desde el principio hasta el final.
    while (Pos1!=LNULL) {
        int i;
        if (strcmp(Pos1->item.data[2], filter) == 0) {
            printf("- %s ", Pos1->item.data[0]);
            //Se imprime el comando de la posición de la lista.
            printf("%10d", Pos1->item.posNum);
            for (i=1; Pos1->item.data[i][0] != '\0'; i++) printf(" %10s", Pos1->item.data[i]);
            printf("\n");
            //Si el bucle llega a la posición especificada, se detiene.
            Pos1 = Pos1->next;
        } else Pos1 = Pos1->next;
    }
}

void initializeFileList(List *list) {
    Item newFile;
    //Creamos un nuevo fichero y le reservamos los valores de descriptor 0 1 y 2, le asignamos las salidas como nombre y el modo de apertura, y lo insertamos a la lista de ficheros.
    newFile.posNum=0;
    strcpy(newFile.data[0],"entrada estandar");
    strcpy(newFile.data[1],"O_RDWR");
    insertToList(newFile,LNULL,list);

    newFile.posNum=1;
    strcpy(newFile.data[0],"salida estandar");
    strcpy(newFile.data[1],"O_RDWR");
    insertToList(newFile,LNULL,list);

    newFile.posNum=2;
    strcpy(newFile.data[0],"error estandar");
    strcpy(newFile.data[1],"O_RDWR");
    insertToList(newFile,LNULL,list);
}