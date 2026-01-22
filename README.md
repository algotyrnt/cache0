# cache0

![Build Status](https://github.com/algotyrnt/cache0/actions/workflows/run_tests.yml/badge.svg)
![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**cache0** is a lightweight, persistent Key-Value store built from scratch in C++. It is designed to demonstrate core systems programming concepts including memory management, file persistence (RAII), and the Standard Template Library (STL).

## Features

- **In-Memory Speed:** Uses `std::unordered_map` for $O(1)$ average time complexity on lookups.
- **Persistence:** Automatically saves data to disk on exit and loads on startup (RAII pattern).
- **Safety:** Utilizes `std::optional` to handle missing keys gracefully.
- **Modern C++:** Written in C++17.

## Getting Started

### Prerequisites
- A C++ compiler supporting C++17 (GCC, Clang, or MSVC).

### Directory Structure
```text
Cache0/
├── include/
│   └── cache0.hpp      # The Product (Header-Only Library)
├── src/
│   └── main.cpp        # The Example/Demo file
├── tests/
│   └──test_cache0.cpp  # Unit Tests
├── .gitignore          # The ignore file
├── .github/            # The CI workflow
└── README.md           # Readme File
```
## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
