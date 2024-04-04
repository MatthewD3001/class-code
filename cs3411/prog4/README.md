CS3411 Program 4: Prose
=======================
Step 1
------
This program works as expected with printing the remaining arguments provided into a file with the given name and appends it with ".stdout".
I tested this with `./step1 output ls -la` and the output file was created with the proper name and extension and it contained "ls-la" which is the expected output.

Step 2
------
This program works as expected with making a child process to exec the given progam with the expected args.
I tested this with `./step2 output ls -la` and the child process correctly exec'd and printed the output of `ls -la` to my console.

Step 3
------
This program works as expected with opening the stdout and stderr files and reassigning the child process' stdout and stderr to these file descriptors.
I tested this with `./step3 output ls -la` and the child process correctly filled the `output.stdout` file that was created in the current directory with the output of the command `ls -la`.
I then tested this with `./step3 output ls -la junk_argument` and the child process correctly filled the `output.stderr` file that was created in the current directory with the error output of `ls` not expecting `junk_argument`.

Step 4
------
This program works as expected with waiting a set amount of time for input on stdin before printing "tick" to the console.
I tested this by running the program with a timeout value of 3 seconds. When running the program I waited the 3 seconds initially and saw the first output of "tick" I then inputed to stdin multiple times for about 5 seconds to confirm that "tick" was not printed until I was inactive for 3 seconds and that it would correctly print out whatever I was typing on stdin after pressing enter, which it correcly printed "tick" then.

Step 5
------
This program works as expected with waiting for input from either stdin or the pipe created in the directory and printing "tick" when the timeout occurs.
I tested this program by first creating the pipe using `mkfifo transit` to create the pipe in the current directory. I then started the program and wiating the inital 3 seconds to confirm that "tick" was printed to stdout. Then I inputed on stdin for about 5 seconds to confirm it was printing what I typed after pressing enter and it was also not printing "tick" during this time. I then waitied 3 seconds to see "tick" appear again and after that I opened another terminal to write to the pipe using the command `echo "Hello world" > transit` periodically over the next 5 seconds and confirmed that in the first terminal was printing this statement and also not printing "tick" wich it was all doing correctly.

Step 6
------
This program works as expected with creating the output files and opening the pipes then assigning them to the child process and reading from them in the parent process. At the time this was still using the timeout method to exit the program.
I tested this program by running the command `./step6 output cat prose` followed by `cmp output.stdout prose` and verified that there was no output meaning the two files were exactly the same.
I then tested this program again by running the command `./step6 output ls -la junk_argument` to verify that I was given the proper error statement in the console and also in the `output.stderr` file.

Prose
-----
This program works as expected with making the two pipes needed. It assigns the two write ends of the pipes to the child's stdout and stderr fds then execs the given program. After that the parent process correcly interprets the read end of the pipes until both read EOF indicating the program has finished. The output to the console is converted to hex for the non-printable characters and only the necessary `.stdout` and/or `.stderr` files are created.
I tested this program by running the command `./prose output cat prose` followed by `cmp output.stdout prose` and verified that there was no output, this meant they were the same file as expected.
I then tested this program by running the command `./prose output ls -la junk_argument` and varified that the error statement was printed, only the `output.stderr` was printed and that it contained the correct error.
