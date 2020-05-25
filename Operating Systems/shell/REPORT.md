# **PROJECT 1 REPORT**
###### *Written by Bill Pansacola & Justin Buhay*
------
- Justin and I remained in constant communication via Facebook Messenger when
working our code. Occasionally we would meet to engage in Pair programming;
Justin would be coding (the driver), while I reviewed our lines of code (the
  observer).
- The  ph

#### ***Phase 2 Development***

 - In order to read commands from input, we defined a constant called
 ```MAX_LINE_CHARS```, which holds the value 512, the maximum length of the
 command line. We then placed a ```printf("sshell$ ")``` and
 ```fgets(char[MAX_LINE_CHARS], MAX_LINE_CHARS, stdin)``` in order to
 constantly prompt the user after each command. ```fgets``` stops reading if it
 encounters a newline character, but includes that character, alongside the null
 terminating char, so it saved us some lines of code.
 - We store the input into two separate types of input variables,
 ```origInput``` and ```input```, in order to save the original input for our
 exit status, while using ```input``` for parsing arguments in the later phases.

#### ***Phase 3 Development***

- To keep track of all the types of errors, we implemented an enum to hold all
the types of errors provided by the assignment. We then used the values from the
enum within another function called, ```errorMessages(int errorType)```, that
prints out the correct error message based on the errorType. It utilizes a
switch statement, so it prevented us from constantly calling
- Created a struct to hold our parsing of the command line input. We wanted to
take advantage of a struct acting like an object, so we created variables that
hold the command, arguments, whether there exists an I/O redirection, and a file
name. This also made it easier to pipe, as we can create an array of structs to
represent which struct as what.

#### ***Phase 4 Development***

- We first do a check with whether any of these commands exist within the input.
For cd, we always know that the second argument will be the directory we want to
relocate to. We read through the documentation in GNU C and saw we had to use
chdir() command passing in that second argument.
- For pwd, we did a similar check as GNU documentation explains that
```getcwd()``` command will obtain the current working directory for us.
- For exit, it was pretty simple. We just print out to stderr() and
exit the current process.

#### ***Phase 5 Development***

- Beginning with parsing, we started off with adding spaces before/after every
"<" symbol so that it would be easier to handle in the following process.
- Now we created 2 variables into our struct, inputRedirection and file.
In which there exists an input redirection and its file name.
- We then created a function called IO_RedirectionOpenFile().
Which takes in the current struct CommandArgs, and a non-existent file
descriptor. Now we use the inputRedirection variable and check if it exists.
- If it does exist, we have to open the file we've obtained. Then dup2() it
into our STDIN_FILENO into the file instead of of our terminal.
And close the file descriptor we opened.

#### ***Phase 6 Development***

- Similar to the previous phase. But instead of passing STDIN_FILENO into dup2,
we pass in STDOUT_FILENO which gives us access to writing into a file.
Most importantly, we had to open() using ```O_CREAT | O_TRUNC | O_WRONLY```
because it provides the ability to truncate to 0.

#### ***Phase 7 Development***

- This section took us a long time to implement, as it consisted of changing a
lot in our parsing method and also drawing out how it works.
  - We need to first create an array of structs that will help us implmenet the
  number of commands to use.
  - Now we counted it up in our main function and placed that number in the
  variable ```numCommands```.
- Next we had to make a function called ```pipelineCommand``` taking in our
command struct and numCommands.
  - Now we have to create a ```for loop``` that excludes the last command as it
  will receive input/output differently.
  - So, for each command we ```pipe``` and ```fork()``` then look at the child
  process. We need to keep track of the write of our file descriptor pipe and
  an extra variable called input, which changes with fd[0].
- Now, when those change, we want to duplicate them again into STDIN/STDOUT.
  - We also have to consider when our functions contain input/output redirection
   so we included that into our function.
- At the end, we run ```execvp```.

#### ***Phase 8 Development***

- We ran into difficulties implementing background processes. We were able to
identify the last token of our input to identify if the string contains a
```&```. Much like during the I/O Redirection phases, we used parseInput() to
remove the ```&``` from the argument list.
- Though we were able to identify if a process should be executed in the
background, we weren't able to process the background process' exit status
correctly.
- We also ran into some problems where the time interval argument recognized
the ```&``` as a unit of time rather than the background process sign.
- We believed this problem is due some part of our implementations for parsing
input, and recognizing a background child and/or parent process.

### **References used to help understand the code**

1. https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/dup2.html
2. [video about execvp](https://www.youtube.com/watch?v=1Tn0E7aul9A&fbclid=IwAR2xsHH7kpufp5lfp89kLLTSV6WQxvznslnDkkBWVpoJL0R2IXLcRGhWsL4)
3. http://man7.org/linux/man-pages/man3/system.3.html
4. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Running-a-Command
5. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Line-Input
6. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Executing-a-File
7. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Working-Directory
8. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Pipes-and-FIFOs
9. https://www.gnu.org/software/libc/manual/html_mono/libc.html#Process-Completion
