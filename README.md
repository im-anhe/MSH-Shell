## Description
Mav shell (msh), similar to 
bourne shell (bash), c-shell (csh), or korn shell (ksh) will accept commands, fork a child 
process and execute those commands. The shell, like csh or bash, will run and accept 
commands until the user exits the shell

Mav shell supports up to 10 command line 
parameters in addition to the command. 

Mav shell is be implemented using fork(), wait() and one of the 
exec family of functions.

The shell also support the history command which will list the last 
15 commands entered by the user. Typing !n, where n is a number between 0 and 14 will 
result in the shell re-running the nth command. If the nth command does not exist then 
the shell will state “Command not in history.”. The output shall will be a list of 
numbers 1 through n and their commands.

The code will compile with
        gcc msh.c -o msh 
