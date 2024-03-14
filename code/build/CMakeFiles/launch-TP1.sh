#!/bin/sh
bindir=$(pwd)
cd /mnt/c/Users/33652/Desktop/TER_BIB/code/TP1/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /mnt/c/Users/33652/Desktop/TER_BIB/code/build/TP1 
	else
		"/mnt/c/Users/33652/Desktop/TER_BIB/code/build/TP1"  
	fi
else
	"/mnt/c/Users/33652/Desktop/TER_BIB/code/build/TP1"  
fi
