#!/bin/bash
for i in {1..10}
do
	./mi_mkdir Disco /dir$i/ 7
	./mi_ls Disco /
done



