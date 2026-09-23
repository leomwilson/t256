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

- The machine only has 254 states, plus the special states `0` (reject) and `1` (accept).

- The tape is limited to 192kb (192 * 1024 characters). Attempting to move right past the end will leave the head at its current position (like attempting to move left past the beginning).

- Each character on the tape must be 7 bits; since the tape stores 8-bit bytes, the highest-order bit will be ignored. This is not an issue if using standard ASCII characters.

- Transition to state `0` to reject or state `1` to accept, in either case terminating. If the program transitions to a terminal state while moving the tape head left, the machine outputs the tape as a null-terminated string starting with the beginning of the tape; if the tape head is moved right at this transition, the machine outputs the tape starting at the new location of the head.

- Note that because state `0` rejects, if the instruction memory is initialized to zeroes, then an undefined transition automatically rejects.

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

# t256asm
An assembler for T-256 the virtual machine.

## Compilation

```sh
cd assembler
gcc t256asm.c -o t256asm
```

## Usage

```sh
cd assembler
./t256asm input.t256asm output.t256
```

## Assembly Syntax

A T-256 assembly file is comprised of statements denoting
transitions for the Turing machine, in the form
`from_state, tape_char -> to_state, new_char, direction;`
where the states and characters are given as two-digit hex numbers
and the direction is the character `R` (right) or `L` (left).
All of these are case-insensitive, and whitespace is ignored.
Comments begin with `#` and end with a newline.

For example, `3F, 41 -> 09, 42, R; # comment` is a transition
from state 63 (0x3F) when the character 'A' (ASCII 65, 0x41) is read
to state 9 (0x09), writing the character 'B' (ASCII 66, 0x42)
and moving the head right, followed by a comment.

To accept or reject, transition to the respective states,
as explained in the behavior section above. Note that undefined
transitions automatically reject, since they default to zero and
are thus equivalent to `XX, XX -> 00, 00, L;`

See `assembler/0n1n.t256asm` for a complete example.
Feel free to assemble that file and try it out!

## AI Statement

No AI tools were used in the development of this project.
No code in this repository was generated using AI.

In many of my other projects, I use AI assistance
(with careful testing and human review, of course)
to accelerate my development process. On this project,
my goal was primarily to refamiliarize myself with
low-level development in C, as well as to do
something I had envisioned but never had time to
actually build. Since this is largely an educational
and personal-interest exercise, not software with
a strong real-world use case, using AI to get quick
results would be antithetical to my purpose.

In `$currentYear`, a competent developer should
have the skills to use AI responsibly when beneficial,
but must never rely so heavily on it as to lose the
ability to write high-quality code without it.