# Cache0

![Build Status](https://github.com/algotyrnt/Cache0/actions/workflows/run_tests.yml/badge.svg)
![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Type](https://img.shields.io/badge/type-Header--Only-orange.svg)

Cache0 is a lightweight, persistent, header-only C++17 key-value store. It provides a template-based interface for storing and retrieving arbitrary data types backed by a Write-Ahead Log (WAL) persistence engine.

---

## Features

- **Header-Only Library**: Drop `include/algotyrnt/cache0.hpp` into your project without external compilation steps.
- **Generic & Type-Agnostic**: Store standard types (`std::string`, `int`, `double`) or custom user classes using C++ templates.
- **Write-Ahead Log (WAL) Engine**: Low-latency append-only log architecture (`#CACHE0_WAL_V1`) for fast writes, deletes, and crash recovery.
- **Move Semantics & Exclusive Ownership**: Move-constructible and move-assignable to safely transfer ownership between objects. Copy operations are explicitly deleted to prevent duplicate file handles.
- **Rich Container API**: Includes `contains()`, `size()`, `empty()`, `clear()`, `flush()`, and `compact()`.
- **Namespaced**: All classes and templates are cleanly encapsulated under `namespace algotyrnt`.

---

## Quick Start

### Basic Usage with `std::string`

```cpp
#include <iostream>
#include <algotyrnt/cache0.hpp>

int main() {
    // Initialize cache backed by "app_data.db"
    algotyrnt::Cache0<std::string> cache("app_data.db");

    // Put key-value pairs
    cache.put("user_101", "Alice");
    cache.put("user_102", "Bob");

    // Container observers
    std::cout << "Cache size: " << cache.size() << std::endl;
    if (cache.contains("user_101")) {
        auto user = cache.get("user_101");
        std::cout << "Found: " << *user << std::endl;
    }

    // Remove key
    cache.remove("user_102");

    // Explicitly flush log buffer to disk
    cache.flush();

    // Compact log file to reclaim disk space
    cache.compact();

    return 0;
}
```

### Usage with Custom Structs / Objects

To store custom types, implement standard stream operators (`operator<<` and `operator>>`):

```cpp
#include <iostream>
#include <algotyrnt/cache0.hpp>

struct UserProfile {
    int id;
    std::string role;

    friend std::ostream& operator<<(std::ostream& os, const UserProfile& u) {
        return os << u.id << " " << u.role;
    }

    friend std::istream& operator>>(std::istream& is, UserProfile& u) {
        return is >> u.id >> u.role;
    }
};

int main() {
    algotyrnt::Cache0<UserProfile> db("profiles.db");

    UserProfile admin{1, "Administrator"};
    db.put("admin_user", admin);

    auto result = db.get("admin_user");
    if (result) {
        std::cout << "ID: " << result->id << ", Role: " << result->role << std::endl;
    }

    return 0;
}
```

---

## API Reference

| Method                                               | Signature            | Description                                                                           |
| :--------------------------------------------------- | :------------------- | :------------------------------------------------------------------------------------ |
| `Cache0(const std::string& filename)`                | Explicit Constructor | Initializes cache and replays existing WAL log entries from `filename`.               |
| `void put(const std::string& key, const T& value)`   | `void`               | Inserts or updates key and appends SET record to WAL log.                             |
| `std::optional<T> get(const std::string& key) const` | `std::optional<T>`   | Retrieves value for `key` if present; returns `std::nullopt` if absent.               |
| `bool remove(const std::string& key)`                | `bool`               | Removes `key` from cache and appends DEL record to WAL log.                           |
| `bool contains(const std::string& key) const`        | `bool`               | Returns `true` if `key` exists in cache, `false` otherwise.                           |
| `size_t size() const noexcept`                       | `size_t`             | Returns number of active key-value pairs in memory.                                   |
| `bool empty() const noexcept`                        | `bool`               | Returns `true` if cache contains zero elements.                                       |
| `void clear()`                                       | `void`               | Clears all elements from cache and compacts log.                                      |
| `void flush() const`                                 | `void`               | Flushes active WAL stream buffer to disk; throws `std::runtime_error` on I/O failure. |
| `void compact() const`                               | `void`               | Rewrites active entries into a fresh compacted log file to reclaim disk space.        |
| `Cache0(Cache0&&) noexcept`                          | Move Constructor     | Transfers ownership of cache store and log file streams.                              |
| `~Cache0()`                                          | Destructor           | Compacts log file and closes active streams.                                          |

---

## Integration Guidelines

### Option A: CMake (Recommended)

Add this repository to your project directory, then reference it in your `CMakeLists.txt`:

```cmake
add_subdirectory(Cache0)
target_link_libraries(your_target PRIVATE cache0)
```

### Option B: Manual Include

Copy `include/algotyrnt/cache0.hpp` into your project's include directory and pass `-Iinclude` during compilation:

```bash
g++ -std=c++17 -Iinclude main.cpp -o main
```

---

## Building and Testing Locally

### Using CMake

```bash
# 1. Generate build files
cmake -S . -B build

# 2. Build test and demo targets
cmake --build build

# 3. Execute test suite
cd build && ctest --output-on-failure
```

### Manual Compilation

```bash
# Compile and run unit tests
g++ -std=c++17 -Iinclude tests/test_basic.cpp -o run_tests
./run_tests

# Compile and run demo application
g++ -std=c++17 -Iinclude examples/demo.cpp -o demo
./demo
```

---

## Repository Layout

```text
Cache0/
├── .github/workflows/   # CI/CD workflows for automated testing and releases
├── examples/            # Usage demonstration code
│   └── demo.cpp
├── include/             # Header-only library files
│   └── algotyrnt/
│       └── cache0.hpp
├── tests/               # Unit test suite
│   └── test_basic.cpp
├── CMakeLists.txt       # CMake build configuration
├── LICENSE              # MIT License
└── README.md            # Documentation
```

---

## License

Distributed under the MIT License. See `LICENSE` for details.
