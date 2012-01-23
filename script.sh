#!/bin/bash

echo "Script de primitivas"

Disco="Disco.imagen"
Dir="/simul_20121231278/"

#listamos la raiz
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco /

#cambiamos permisos al directorio de la simulacion
echo
echo MI_CHMOD -------------------------------------------
echo
./mi_chmod $Disco 4 $Dir

#volvemos a listar para ver los permisos del directorio de simulacion
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco /

#volvemos a cambiar los permisos del directorio de simulacion
echo
echo MI_CHMOD -------------------------------------------
echo
./mi_chmod $Disco 7 $Dir

#listamos de nuevo la raiz
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco /

#listamos el directorio de simulacion
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco $Dir

#hacemos un mi_cat de informe.txt que ha generado el verificador de la simulacion
echo
echo MI_CAT -------------------------------------------
echo
./mi_cat $Disco ${Dir}informe.txt

#creamos un enlace dentro del directorio raiz que apunta al informe.txt
echo
echo MI_LN -------------------------------------------
echo
./mi_ln $Disco ${Dir}informe.txt /enlace

#hacemos un mi_stat del informe.txt para ver como nlinks ahora vale 2
echo
echo MI_STAT -------------------------------------------
echo
./mi_stat $Disco ${Dir}informe.txt

#listamos la raiz para ver el enlace creado
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco /

#borramos el enlace
echo
echo MI_RM -------------------------------------------
echo
./mi_rm $Disco /enlace

#volvemos a hacer stat de informe.txt para ver como nlinks vale 1
echo
echo MI_STAT -------------------------------------------
echo
./mi_stat $Disco ${Dir}informe.txt

#listamos otra vez para ver que enlace ha sido borrado
echo
echo MI_LS -------------------------------------------
echo
./mi_ls $Disco /
