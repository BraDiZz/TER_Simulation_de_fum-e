#!/bin/sh
clear
make -j
bindir=$(pwd)
cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/TP1/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/TP1 
	else
		"/home/e20190004794/Bureau/master_imagine/s2/TER/code/build/TP1"  
	fi
else
	"/home/e20190004794/Bureau/master_imagine/s2/TER/code/build/TP1"  
fi
