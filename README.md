## **Shell de la asignatura SO - 2023/2024 Ingeniería Informática UDC.**


Más de 3919 lineas de código casi comentadas linea por linea.
Hay funciones sin ningún tipo de comentario (Comentaré todo más adelante).

**CUALQUIER DUDA, PONERSE EN CONTACTO CONMIGO VIA WHATSAPP (EN CASO DE TENER MI NÚMERO) O VIA INSTAGRAM (ESTÁ VINCULADO A MI PERFIL DE GITHUB)**

## Contenido del repositorio
En este repositorio están almacenados los enunciados de estas prácticas, el make y los .c y .h de mi shell, código de ayuda proporcionado por los profesores y una shell de referencia que se nos proporcionó para elaborar los comandos.






## **Fallos conocidos y notas a tener en cuenta**

**Todos estos fallos no serán arreglados**
**(Si hay algún otro fallo a parte de los notificados, hazmelo saber para añadirlo a esta lista)**

-La entrada, salida y error estandar se da por supuesto que se inicializan como O_RDWR, habría que inicializarlas con los permisos correspondientes.

-Memdump, falla en el formato.

-La implementación de la lista es bastante chapucera, recomiendo usar otra lista y cambiar todas las funciones que dependan de la lista.

-Recomiendo no usar cmdList ni el .c ni el .h, puede ser un poco engorroso de usar y de entender, aunque si se sabe usar, recomiendo modificarlo de manera que se adapte a lo que se pide y también un comentario dejando créditos, ya que si no se deja un comentario especificando eso, existe un alto riesgo de copia.

-Las listas se deberían declarar globalmente.

-Lo que si recomiendo es mirar cmnds.c, ya que tiene la funcionalidad de todos los comandos.

-Conditional jump en jobs al ejecutar con valgrind.

-Redirigir salida estandar y la entrada cuando ejecutas un comando en primer plano, ya que cuando lo ejecutas, mientras que la terminal no responde, al meter tu inputs, al finalizarse el proceso se van a leer esos inputs, cosa que no debería pasar.

## **NOTAS:**
P1:0.30  
P2:0.85  
P3:0.85  
Total:2/2.5

