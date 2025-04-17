# Modern C++ Text Editor

A powerful command-line text manipulation tool inspired by classic Unix editors. This program processes text according to specified commands, including substitution, deletion, transliteration, and insertion operations.

## Features

- **Command-based text manipulation**: Apply various text operations through a simple command syntax
- **Line-based addressing**: Target specific lines or ranges of lines
- **Multiple operation types**:
  - Substitution: Replace patterns with new text (with optional global flag)
  - Deletion: Remove lines containing specific patterns
  - Transliteration: Character-by-character replacement
  - Insertion: Insert text at specific positions

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)
- Make (optional, for using the provided Makefile)

### Compilation

#### Using Make

The provided Makefile works on both Linux and Windows (with MinGW or MSVC):

```bash
# Standard build
make

# Debug build
make debug

# Optimized release build
make release

# Clean build artifacts
make clean
```

#### Manual Compilation

```bash
# Linux/MacOS/MinGW
g++ -std=c++17 -Wall -Wextra -o text_editor text_editor.cpp

# MSVC
cl /EHsc /std:c++17 /W4 text_editor.cpp /Fe:text_editor.exe
```

## Usage

The program reads input in the following format:

1. First line: M (number of command lines)
2. Next M lines: Editor commands
3. Next line: N (number of text lines to process)
4. Next N lines: Text content to process

### Command Syntax

Commands follow this general structure:
```
[address] operation/pattern/replacement/[flags]
```

Where:
- `address` (optional): Line number or range in the format `start,end`
- `operation`: One of `s` (substitution), `d` (deletion), `y` (transliteration), or `i` (insertion)
- `pattern`: The search pattern
- `replacement`: The replacement text (not used for deletion commands)
- `flags`: Optional flags like `g` for global substitution

### Examples

```
# Substitute "hello" with "world" on line 5
5 s/hello/world/

# Delete lines containing "error" in lines 10-20
10,20 d/error/

# Transliterate lowercase vowels to uppercase in all lines
y/aeiou/AEIOU/

# Insert "PREFIX: " before the first occurrence of "log" on all lines
i/log/PREFIX: /
```

## Command Types

### Substitution (`s`)

Replaces occurrences of `pattern` with `replacement`. Use the `g` flag for global replacement (all occurrences in a line).

### Deletion (`d`)

Removes lines containing the specified `pattern`.

### Transliteration (`y`)

Performs character-by-character replacement according to the mapping defined by `pattern` and `replacement`.

### Insertion (`i`)

Inserts the `replacement` text before the first occurrence of `pattern`.

## Error Handling

The program provides detailed error messages for various error conditions:
- Invalid command syntax
- Parameter errors
- Invalid line numbers

## License

This project is open source and available under the MIT License.