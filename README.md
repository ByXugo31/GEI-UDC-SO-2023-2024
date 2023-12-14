Shell de la asignatura de SO del año 2023/2024 en UDC.
Más de 3919 lineas de código casi comentadas linea por linea.
Hay funciones sin ningún tipo de comentario (Comentaré todo más adelante).

FALLOS CONOCIDOS Y NOTAS A TENER EN CUENTA (Cosas que no voy a cambiar ni corregir, cualquier fallo extra hazmelo saber).

-La entrada, salida y error estandar se da por supuesto que se inicializan como O_RDWR, habría que inicializarlas con los permisos correspondientes.

-Memdump, falla en el formato.

-La implementación de la lista es bastante chapucera, recomiendo usar otra lista y cambiar todas las funciones que dependan de la lista.

-Recomiendo usar el struct cmdList, tan solo hay que modificar NUM_COMMANDS en el .h en función de los comandos a añadir o quitar, en el .c añadir o quitar elementos, en p3.c, en el switch, añadir o quitar elementos según los comandos que tengais
NOTA: Tiene mucho margen de mejora, tenía planeado añadir punteros a funciones dentro del struct pero por desgracia no he tenido tiempo para implementarlo.

-Las listas declaradlas globalmente, así no perdeis tiempo pasándoselas a todos los comandos.

-Lo que si recomiendo es mirar cmnds.c, ya que tiene la funcionalidad de todos los comandos.
