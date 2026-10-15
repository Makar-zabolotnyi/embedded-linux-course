# Lab 02 report

- Student name: Makar Zabolotnyi
- GitHub username: Makar-zabolotnyi

Replace each placeholder with your own command output. Run HOST build commands
from this lab directory and BBB module commands from `/home/debian/labs/lab02`.
Keep the complete `vermagic`, not only its release prefix.

## Prerequisites and module metadata

### BBB: `uname -r`

```text
6.12.96-bone64
```

### HOST: `cat ~/bbb-workspace/kernel/bb-kernel/KERNEL/include/config/kernel.release`

```text
6.12.96-bone64
```

### HOST: `file lab02_hello.ko`

```text
lab02_hello.ko: ELF 32-bit LSB relocatable, ARM, EABI5 version 1 (SYSV), BuildID[sha1]=8458aaac023a58f229a0d62323f3c206f88d6a79, not stripped
```

### HOST: `modinfo lab02_hello.ko | grep vermagic`

```text
vermagic:       6.12.96-bone64 preempt mod_unload ARMv7 thumb2 p2v8
```

Explain why the full kernel releases must match and whether yours match:

The running kernel compares a loaded module's `vermagic` string (exact kernel
release plus build configuration flags such as preempt, SMP support, and ABI)
against its own. A module built against a different release or configuration
is rejected, or can destabilize the kernel if force-loaded, so the full
release string must match exactly, not just a shared prefix. Here the HOST
build-tree release (`6.12.96-bone64`), the module's `vermagic` release
(`6.12.96-bone64`), and the BBB `uname -r` (`6.12.96-bone64`) **all match**.
Note: `make` printed a warning that the kernel was built with GCC 14.3.0 while
GCC 14.4.0 was used to build this module; this did not affect the vermagic
release string or loading, but is recorded here for completeness. So was rebuild 
module with matching GCC 14.3.0 toolchain

## Load, inspect, and unload on BBB

### `sudo insmod ./lab02_hello.ko name=Student` and `sudo dmesg | tail -20`

```text
$ sudo insmod ./lab02_hello.ko name=Student
$ echo $?
0
$ sudo dmesg | tail -10
[   38.689100] configfs-gadget.g_multi gadget.0: HOST MAC 78:04:73:41:0b:e3
[   38.729153] configfs-gadget.g_multi gadget.0: MAC 78:04:73:41:0b:e4
[ 2341.568799] lab02_hello: loading out-of-tree module taints kernel.
[ 2341.575830] hello_module: Hello, Student, from kernel space on BBB!
```

### `lsmod | grep lab02_hello`

```text
lab02_hello            12288  0
```

`cat /sys/module/lab02_hello/parameters/name` → `Student`

### `sudo rmmod lab02_hello` and `sudo dmesg | tail -20`

```text
$ sudo rmmod lab02_hello
$ echo $?
0
$ sudo dmesg | tail -10
[ 2341.568799] lab02_hello: loading out-of-tree module taints kernel.
[ 2341.575830] hello_module: Hello, Student, from kernel space on BBB!
[ 2440.485997] hello_module: Goodbye from kernel space on BBB!
$ lsmod | grep lab02_hello || echo 'module unloaded'
module unloaded
```

## Parameter tests on BBB

### Valid name: `name=YourName`

```text
$ sudo insmod ./lab02_hello.ko name=Makar
$ echo $?
0
$ sudo dmesg | tail -5
[ 2498.674473] hello_module: Hello, Makar, from kernel space on BBB!
$ sudo rmmod lab02_hello
```

### Invalid empty name: `name=` (must return `-EINVAL`)

```text
$ sudo insmod ./lab02_hello.ko name=
insmod: ERROR: could not insert module ./lab02_hello.ko: Invalid parameters
$ echo $?
1
$ sudo dmesg | tail -5
[ 2522.595838] hello_module: Goodbye from kernel space on BBB!
```

No new greeting appears in dmesg — the module was rejected before `pr_info()`
ran, confirming `hello_init()` returns `-EINVAL` for an empty name.

### Valid count: test `count=1` and `count=10`

```text
$ sudo insmod ./lab02_hello.ko count=1
$ echo $?
0
$ sudo dmesg | tail -5
[ 2600.383060] hello_module: Hello, Student, from kernel space on BBB!
$ sudo rmmod lab02_hello

$ sudo insmod ./lab02_hello.ko count=10
$ echo $?
0
$ sudo dmesg | tail -15
[ 2647.527190] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.533699] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.540946] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.548309] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.555073] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.561791] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.568479] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.575171] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.581889] hello_module: Hello, Student, from kernel space on BBB!
[ 2647.588588] hello_module: Hello, Student, from kernel space on BBB!
$ sudo rmmod lab02_hello
```

`count=1` printed the greeting exactly once; `count=10` printed it exactly ten
times.

### Invalid count: test `count=0` and `count=11` (must return `-EINVAL`)

```text
$ sudo insmod ./lab02_hello.ko count=0
insmod: ERROR: could not insert module ./lab02_hello.ko: Invalid parameters
$ echo $?
1
$ sudo dmesg | tail -5
[ 2670.815453] hello_module: Goodbye from kernel space on BBB!

$ sudo insmod ./lab02_hello.ko count=11
insmod: ERROR: could not insert module ./lab02_hello.ko: Invalid parameters
$ echo $?
1
$ sudo dmesg | tail -5
[ 2670.815453] hello_module: Goodbye from kernel space on BBB!
```

Both out-of-range values are rejected with a non-zero exit status, and no new
greeting appears in dmesg, confirming `hello_init()` returns `-EINVAL` for
`count` outside the 1–10 range.

## Questions

1. Why does a kernel module not have `main()`?

   A kernel module is not a standalone process with its own entry point
   invoked by the C runtime via `exec()`. Instead it registers callback
   functions (`module_init`/`module_exit`) that the kernel calls when the
   module is loaded and unloaded; in between, the module only runs in
   response to calls from the kernel, not as an independent sequential
   program.

2. What does `module_init()` do?

   It registers the function that the kernel runs once when the module is
   loaded (`insmod`/`modprobe`), or at boot time if the module is built in.
   In this lab it registers `hello_init()`.

3. What does `vermagic` show?

   The exact kernel release and key build configuration flags (preempt/SMP
   support, ABI, compiler-related options) that the module was compiled
   against. The running kernel compares this string to its own before
   allowing `insmod` to load the module.

4. Why can `insmod` fail with `invalid module format`?

   Usually because the module's `vermagic` does not match the running
   kernel's exact release or build configuration, or because the module was
   built for a different architecture, or the `.ko` file is corrupted or
   incomplete.

5. What does the `name` parameter do, and what do its permissions `0444` mean?

   `name` lets the user pass a string at `insmod` time
   (`insmod lab02_hello.ko name=value`), which the module prints in its
   greeting message. Permission `0444` means the corresponding file under
   `/sys/module/lab02_hello/parameters/name` is readable by everyone (owner,
   group, others) but not writable, so the current value can be inspected via
   sysfs but not changed at runtime by writing to that file.

Submit `src/hello.c`, `Makefile`, and this completed `report.md`. Do not commit
`.ko`, `.o`, `.mod`, `.mod.c`, `.cmd`, `Module.symvers`, or `modules.order`.