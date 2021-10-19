Please leave all of my files in the same folder.

To run PMan, all we need is to type 'make' in terminal, makefile will automatically 
compiles and runs PMan. No need to execute something like './Pman'.

Please type a whitespace between your PMan commands and your pid, otherwise your commands
would be considered invalid. Please type like 'bg ls -al'.

Sometimes the PMan output could comes up before or after bash output. This is a common
stuff. Please normally type your PMan commands, ignore the previous output, and PMan 
will give you the right output.

My zombie process checking works very efficiently. It is impossible that we
kill a child process or execute an invalid bash command using a child process but the zombie 
process still shows on the bglist. But if it happens (with 0.01% likelihood that the OS uses 
a very long time killing this process) , the next time you execute bglist you would not
 definitely see it again.
 
All coding and debugging is executed using ECS348 lab Linux environment. If there is anything
unexpected happens like bugs or compilation errors, please use ECS348 lab Linux environment
to do your testing and marking.

If you want to run inf.c using bg, compile inf.c using "gcc inf.c -o inf" before you execute
'make'.

If you just let PMan run simple application like inf.c, for the pstat information, utime and 
stime are very likely to be 0 because this application spends a lot time sleeping.
In order to view more non-zero pstat information including utime, stime, rss and ctext switches,
please let PMan run some application that involves hard drive or ROM I/O, then we could see
the correct non-zero information in pstat.

Thank you!
