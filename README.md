# t256
T-256: A dead simple virtual Turing machine with 256kb memory

## Try It Out

To quickly test the machine with the included sample instructions, run:

```sh
cd capitalizer
gcc capitalizer.c -o capitalizer && ./capitalizer
cd ..
gcc t256.c -o t256 && echo "This is a test." | ./t256 capitalizer/capitalizer.t256
```

Expected output:

```txt
ACCEPT. Tape:
THIS IS A TEST.
```

## Memory Format

See the comment in `t256.c` for more information.

## Behavior

This runs like a regular Turing machine, with a few exceptions:

- The machine only has 254 states, plus the special states `0` (accept) and `1` (reject).

- The tape is limited to 192kb (192 * 1024 characters). Attempting to move right past the end will leave the head at its current position (like attempting to move left past the beginning).

- Each character on the tape must be 7 bits; since the tape stores 8-bit bytes, the highest-order bit will be ignored. This is not an issue if using standard ASCII characters.

- Transition to state `0` to accept or state `1` to reject, in either case terminating. If the program transitions to a terminal state while moving the tape head left, the machine outputs the tape as a null-terminated string starting with the beginning of the tape; if the tape head is moved right at this transition, the machine outputs the tape starting at the new location of the head.

## Compilation

Compile the files using any C compiler. For example,

```sh
gcc t256.c -o t256
```

## Usage

```sh
./t256 [-d] file.t256
```

*Inputs:*

- `file.t256`: must be exactly 256kb with instructions for the machine, see Memory Format

*Flags:*

- `-d`: debug (additional output, useful for debugging)

*Output:*

Status code `0` for accept, `1` for reject, anything else is an error.

By default, the program also prints the final tape as a null-terminated string.