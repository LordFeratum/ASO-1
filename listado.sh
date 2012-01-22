#!/bin/bash
for i in {1..16}
do
	./mi_touch Disco /dir16/pru$i 7
	./mi_ls Disco /
done



