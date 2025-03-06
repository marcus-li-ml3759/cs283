1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation calls waitpid() for each forked child, ensuring all children finish before the shell accepts new
input. Without these calls, children could become zombies, and the shell might proceed while the pipeline is still
running.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After dup2(), the original pipe descriptors remain open. Closing them is necessary to signal EOF correctly and free
resources. Leaving them open can prevent EOF detection, cause deadlocks, and eventually exhause file descriptors.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in rather than an external command because it needs to change the current directory of the
shell process itself. If implemented exteranlly, the change would occur in a child process and not affect the parent
shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

I could modify my implementation by switching from fixed-size arrays to dynamicaly allocating structures for command
buffers and pipes. This approach increases flexibility but also adds complexity in memory management and may incur
performance overhead while still being subject to system resource limits. 
