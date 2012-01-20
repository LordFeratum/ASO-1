#!/bin/bash

cp ../mi*.c ./

gcc -o mi_mkfs mi_mkfs.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_cat mi_cat.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_chmod mi_chmod.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_ln mi_ln.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_ls mi_ls.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_mkdir mi_mkdir.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_rm mi_rm.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_stat mi_stat.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_touch mi_touch.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_write mi_write.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c
gcc -o mi_read mi_read.c bloques.c directorios.c ficheros_basico.c ficheros.c semaforo.c

rm mi*.c
mv mi* ../
