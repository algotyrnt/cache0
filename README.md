# Cache0

![Build Status](https://github.com/algotyrnt/Cache0/actions/workflows/run_tests.yml/badge.svg)
![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Type](https://img.shields.io/badge/type-Header--Only-orange.svg)

Cache0 is a lightweight, header-only C++17 key-value store with automatic disk persistence. It provides a simple, template-based interface for storing and retrieving arbitrary data types with RAII file synchronization.

---

## Features

- **Header-Only Library**: Drop `include/algotyrnt/cache0.hpp` into your project without extra compilation steps.
- **Generic & Type-Agnostic**: Store standard types (`std::string`, `int`, `double`) or custom user classes using C++ templates.
- **Automatic RAII Persistence**: Automatically loads data from disk on construction and flushes modified state to disk on destruction.
- **Exception Safety**: Safe file I/O operations throwing `std::runtime_error` on storage failures.
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

    // Fetch value (returns std::optional<T>)
    auto user = cache.get("user_101");
    if (user.has_value()) {
        std::cout << "Found: " << *user << std::endl;
    }

    // Remove key
    cache.remove("user_102");

    // State is saved automatically when 'cache' goes out of scope
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

| Method                                               | Signature          | Description                                                                    |
| :--------------------------------------------------- | :----------------- | :----------------------------------------------------------------------------- |
| `Cache0(const std::string& filename)`                | Constructor        | Initializes cache and loads existing key-value pairs from `filename`.          |
| `void put(const std::string& key, const T& value)`   | `void`             | Inserts or updates the value associated with `key`.                            |
| `std::optional<T> get(const std::string& key) const` | `std::optional<T>` | Retrieves value for `key` if present; returns `std::nullopt` if absent.        |
| `bool remove(const std::string& key)`                | `bool`             | Deletes `key` from cache. Returns `true` if removed, `false` if key not found. |
| `~Cache0()`                                          | Destructor         | Flushes all key-value entries to disk.                                         |

---

## Integration Guidelines

### Option A: CMake (Recommended)

Add this repository to your project directory or git submodule, then reference it in your `CMakeLists.txt`:

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
├── .github/workflows/   # CI/CD workflows for automated CMake testing
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
