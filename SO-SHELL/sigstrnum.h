#ifndef P2_SIGSTRNUM_H
#define P2_SIGSTRNUM_H
#endif //P2_SIGSTRNUM_H

#include "signal.h"
//Sacada del código de ayuda
/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del número y viceversa */
struct SEN {
    char *nombre;
    int senal;
};
char *NombreSenal(int sen);
