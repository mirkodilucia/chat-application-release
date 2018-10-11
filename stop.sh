#!/bin/sh

for i in $(pidof client server);
do
	kill -s 9 $i
done

exit;
