![image](image/prof-saulo-santos-pss-c.png)

# PSS Library for C - Refactored (pss2)

This is a professional-grade C library developed to safely read characters, strings, integers, and floating-point numbers from standard input. This refactored version (`pss2`) focuses on production readiness, addressing common pitfalls found in academic code such as blocking loops, internal static buffers, and silent failures.

## 🚀 Key Features of v2 (Production-Ready)

1. **Error Status Codes (`PSS_STATUS`) & Predictability:**
   Instead of failing silently or guessing the result, all functions return a `PSS_STATUS` enum (`PSS_OK`, `PSS_ERR_EOF`, `PSS_ERR_EMPTY`, `PSS_ERR_PARSE`, `PSS_ERR_RANGE`, `PSS_ERR_INVALID_BUFFER`). The actual parsed values are returned via pointers (`*output`), cleanly separating the operation outcome from the data payload.

2. **Inversion of Control & Memory Management:**
   Removed "magic numbers" (like `char aux[5120]`) inside the library. The caller is now required to inject the pre-allocated buffer and its size. This mitigates stealthy Stack Overflow risks and adheres to standard modern C library practices.

3. **Non-Blocking (Fail-Fast) Architecture:**
   The interactive infinite `do...while` loops have been completely removed. If an error occurs (such as invalid parsing or EOF), the function returns the error immediately rather than trapping the terminal. This makes the library perfectly suited for CI/CD pipelines, automated testing, server environments, and batch data ingestion via pipes (`< input.txt`).

4. **Single Responsibility Principle (SRP):**
   The library now exclusively handles the capture and conversion of strings to their respective types. The responsibility of deciding what to do upon failure (e.g., retry, abort, or log) is returned to the caller application.

## 🛠️ Data Types Supported

Data reading via standard input is available for the following types:
`int`, `long int`, `float`, `double`, `long double`, `char`, and `char[]`.

## 💻 Usage Examples

### 1) Reading a Double with Error Handling

```c
#include <stdio.h>
#include "pss2.h"

int main() {
    char buffer[256];
    double salary;
    PSS_STATUS status;

    status = input_lf("Enter a salary: ", buffer, sizeof(buffer), &salary);

    if (status == PSS_OK) {
        printf("Valid salary entered: %lf\n", salary);
    } else if (status == PSS_ERR_RANGE) {
        printf("Error: Number out of range.\n");
    } else if (status == PSS_ERR_PARSE) {
        printf("Error: Invalid numeric format.\n");
    } else {
        printf("Error: Input failed (Status code: %d).\n", status);
    }

    return 0;
}
```

### 2) Reading a String

```c
#include <stdio.h>
#include "pss2.h"

int main() {
    char name[100];
    PSS_STATUS status;

    status = input_s("Enter your name: ", name, sizeof(name));

    if (status == PSS_OK) {
        printf("Hello, %s!\n", name);
    } else {
        printf("Failed to read the name.\n");
    }

    return 0;
}
```

## ⚙️ How to Include in Your Project

Copy and paste the `pss2.c` and `pss2.h` files into the root folder of your project.
Insert `#include "pss2.h"` at the beginning of the file that will use the functions.

### Terminal (GCC)
- Compile with: `gcc main.c pss2.c -o app.exe`
- Execute with: `./app.exe`

### IDEs (Code::Blocks / Dev-C++)
- Go to "Project -> Add to Project" (or "Add Files")
- Select `pss2.c` and `pss2.h`
- Compile and Run normally.

## 🤝 Contributions
Contributions are welcome! Feel free to contribute with improvements, bug fixes, or new features for this library. Just fork the repository, make your changes, and submit a pull request.

## 📄 License
This project is distributed under the BSD 3-Clause License. See the `LICENSE` file for more information.
