# buffer-overflows

Simple code showing smashing the stack on GNU-Linux x86-64. This code is based
on [Evan Jones Buffer Overflow
101](https://www.evanjones.ca/buffer-overflow-101.html) article.

## A vulnerable program

The program `vuln.c` contains an obvious buffer overflow bug: The function
`getstr(buffer)` doesn't check the buffer length. If input is larger than 80
characters, `getstr()` will overflow it.

## Testing the vulnerable program

1. Compile the program: `gcc -o vuln vuln.c`
2. Run the program: `./vuln` 
   1. Input less than 80 characters. It should work!
3. Run the program again. Input more than 80 characters. You should get
   `*** stack smashing detected ***: ./vuln terminated`

   Ok. It seems the compiler injected stack canaries (cookies) to detect stack
   buffer overflows. We can disable it by compiling with
   `gcc -o vuln -fno-stack-protector vuln.c`

   Run it again. The second line will show something as

   `&arg = 0x7fffffffdb1c; buffer = 0x7fffffffdb20 diff = -4`

   it shows the buffer address. We'll use it as input of the exploit program.

4. Run the program again. Input more than 80 characters. Now you should get
   `Segmentation fault`. On the return of function `getstr()`, it try to return
   to an invalid address, the the OS kernel trap it and kill the process.

5. You probably notice that in each run `vuln` showed different addresses of
   `buffer`. It is due the operating system is using _Address Space Layout
   Randomization (ASLR)_ when loading programs in memory. _ASLR_ makes loaded
   programs have different base addresses in each run.

   We need to disable ASLR to get a successful exploit. In GNU-Linux we can do
   it with the command 
   `sudo /bin/sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"`.

   You can re-enable ASLR later changing replacing the '0' by '1' in the before
   command.

## The exploit

The program `exploit.c` outputs a _payload_ to use as input to vulnerable
process. The _payload_ includes the following:

    [shellcode]NNNNNNNNNR

where:

1. The `shellcode` contains machine instructions opcodes equivalent to the
   C syscall `execve("/bin/sh", NULL);`

2. _N_: Zero bytes (could be NOP opcodes)

3. _R_: The return address = the buffer address extracted by analysis of
   `vuln.c`.

## Running the exploit

1. Compile the exploit: `gcc -o exploit exploit.c`
2. Run `./exploit 0x7fffffffdb20 -4 | ./vuln`
   
   (where `0x7fffffffdb20` is the buffer address showed by `vuln`).

   Note we give to exploit the address of buffer and the difference from the
   `getstr()` argument and the buffer: It is the frame size. The `exploit`
   program use it to calculate the position of return address.

   You should land in a new shell environment. Try some command like `ls`.
   Maybe you get a _Segmentation fault_ due to stack execution protection.

   To enable stack execution code recompile `vuln.c` with the `-z execstack`
   flag.

3. Run `./exploit 0x7fffffffdb20 -4 | ./vuln` again. Now it should work. You can
   run the `ls` command and stay in the shell. You can finish the shell pressing
   `Ctrl-D`.