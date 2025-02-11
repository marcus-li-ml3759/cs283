1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: 'fgets()' is a good choice for this application because it neatly reads one line at a time and handles EOF
	 >gracefully making it ideal for commnand-based input. This makes it simpler to process one complete command line
	 >at a time without worrying about partial input or buffer overflow.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: I needed to use 'malloc()' because it allows the shell to accomodate varying command lengths
	 > without wasting memory or risking overflow. It allows us to dynamically allocate memory based on the
	 > requirements of the application o ruser configuration. If we used a fixed-size array, we might run out of space
	 > for longer commands or waste memory if commands are short. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  It is necessary because leading and trailing spaces can cause empty or malformed tokens. If we
	 > don't trim spaces, we risk having empty or misleading tokens. Trimming ensures commands and arguments are parsed correctly. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 3 Redirection examples include 'command > file', 'command < file', 'command 2> file'. The first
	 > example redirections STDOUT of command into the file. the second example redirects the contents of the file to
	 > STDIN of command. The third example redirects STDERR into the file. The challenges we might have implementing
	 > them includes parsing the redirection symbols in addition to normal arguments and having to handle cases like
	 > managing error-handling if files can't be opened.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The difference between them is that redirection sends the output or input of a single command
	 > to/from a file or device. Piping connects the STDOUT of one command directly to the STDIN of another command,
	 > which is a way to chain commands together. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep these separate in a shell because it allows scripts and users to
	 > distinguish between normal program output and error messages. If everything were together, it would be
	 > difficult to differentiate between a valid result and an error. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from commands that fail by checking the command exit status
	 > and handle the errors accordingly. If a command outputs both STDOUT and STDERR, we could provide a way to
	 > merge them if needed. We could do this by manipulating file descriptors properly before executing the command. 
