# matrix_lib

A robust, lightweight, and configurable 2D matrix manipulation library written in C. Designed for scientific computing, graphics, and data processing tasks requiring multi-type support, flexible memory storage layouts, and runtime safety.

---

## Table of Contents

- [Features](#features)
- [Project Architecture](#project-architecture)
- [Data Structures](#data-structures)
- [API Reference](#api-reference)
  - [Initialization & Cleanup](#initialization--cleanup)
  - [Configuration & Dimensions](#configuration--dimensions)
  - [Element & Slice Access](#element--slice-access)
  - [Submatrix Operations](#submatrix-operations)
  - [Matrix Arithmetic](#matrix-arithmetic)
  - [Transformations & Copying](#transformations--copying)
  - [Comparison Utilities](#comparison-utilities)
- [Building & Testing](#building--testing)
- [Quickstart Example](#quickstart-example)
- [Memory Management Guidelines](#memory-management-guidelines)

---

## Features

- **Multi-Type Support**: Handles `int` (`INT`), `float` (`FLOAT`), and `double` (`DOUBLE`) matrices seamlessly.
- **Dual Memory Layouts**: Configurable row-major (`ROW_MAJOR`) and column-major (`COL_MAJOR`) memory layout order.
- **Safety First**: Optional runtime bounds checking toggleable via global configuration.
- **Submatrix Slicing & Blitting**: Extract sub-blocks or overlay smaller matrices onto larger ones.
- **Matrix Arithmetic**: Standard matrix addition, subtraction, and linear-algebraic matrix multiplication ($A \times B$).
- **In-Place Transformations**:
  - 90° clockwise matrix rotation (swapping dimensions dynamically).
  - Dynamic resizing (preserving overlapping regions, zeroing new cells).
- **Deep Cloning & Comparisons**: Distinct methods for element-wise equivalence and instance-level identity checking.
- **Automated Test Suite**: Includes CSV parsing and unit tests covering all library capabilities.

---

## Project Architecture

```
matrix_lib/
├── Makefile                # Build targets: matrix.o, test, clean
├── matrix.h                # Public API headers, enums, and data types
├── matrix.c                # Core matrix implementation
├── matrix_test.c           # Comprehensive test runner and CSV parser
└── matrix_test_data.csv    # Test fixtures (INT, FLOAT, DOUBLE test matrices)
```

---

## Data Structures

### `MatrixType`
Supported element data types:
```c
typedef enum {
    INT,    // 32-bit signed integer
    FLOAT,  // Single-precision floating point
    DOUBLE  // Double-precision floating point
} MatrixType;
```

### `StorageOrder`
Determines internal memory arrangement:
```c
typedef enum {
    ROW_MAJOR, // Elements stored row by row (index = i * cols + j)
    COL_MAJOR  // Elements stored column by column (index = j * rows + i)
} StorageOrder;
```

### `MatrixConfig`
Global configuration settings (default: `bounds_check = true`, `storage_order = ROW_MAJOR`):
```c
typedef struct {
    bool bounds_check;          // Toggle index bounds checking
    StorageOrder storage_order; // Global storage order for all matrices
} MatrixConfig;

extern MatrixConfig matrix_config;
```

### `Matrix`
Core matrix structure:
```c
typedef struct {
    size_t rows;      // Number of rows
    size_t cols;      // Number of columns
    void *data;       // Contiguous buffer holding matrix elements
    MatrixType type;  // Element data type
} Matrix;
```

---

## API Reference

### Initialization & Cleanup

| Function | Description |
| :--- | :--- |
| `Matrix* matrix_init(size_t rows, size_t cols, MatrixType type)` | Allocates and zero-initializes a matrix of dimension `rows x cols`. Returns `NULL` on allocation failure. |
| `void matrix_free(Matrix* mat)` | Releases matrix buffer and structure safely. Handles `NULL` pointers gracefully. |

### Configuration & Dimensions

| Function | Description |
| :--- | :--- |
| `void set_matrix_config(bool bounds_check, StorageOrder order)` | Updates global library settings for bounds checking and storage order. |
| `size_t get_row_dim(const Matrix* mat)` | Returns the row count. |
| `size_t get_col_dim(const Matrix* mat)` | Returns the column count. |
| `bool bounds_check(const Matrix* mat, size_t i, size_t j)` | Validates whether indices `(i, j)` fall within `[0, rows)` and `[0, cols)`. |

### Element & Slice Access

| Function | Description |
| :--- | :--- |
| `void set_elem(Matrix* mat, size_t i, size_t j, void* elem)` | Sets value at row `i`, col `j`. |
| `void* get_elem(const Matrix* mat, size_t i, size_t j)` | Returns a pointer to element at `(i, j)`. Returns `NULL` if out of bounds. |
| `void set_row(Matrix* mat, size_t row, void* data)` | Copies data array into the specified row. |
| `void set_col(Matrix* mat, size_t col, void* data)` | Copies data array into the specified column. |
| `void* get_row(const Matrix* mat, size_t row)` | Returns a newly allocated contiguous array containing the row data. **Caller must free**. |
| `void* get_col(const Matrix* mat, size_t col)` | Returns a newly allocated contiguous array containing the column data. **Caller must free**. |

### Submatrix Operations

| Function | Description |
| :--- | :--- |
| `Matrix* create_subset_matrix(const Matrix* src, size_t start_row, size_t start_col, size_t num_rows, size_t num_cols)` | Creates a new matrix representing a rectangular subset of `src`. |
| `void set_subset_matrix(Matrix* dest, size_t start_row, size_t start_col, const Matrix* src)` | Overwrites a rectangular region of `dest` starting at `(start_row, start_col)` with contents of `src`. |

### Matrix Arithmetic

| Function | Description |
| :--- | :--- |
| `Matrix* add_matrices(const Matrix* A, const Matrix* B)` | Element-wise addition ($A + B$). Requires identical dimensions and types. |
| `Matrix* subtract_matrices(const Matrix* A, const Matrix* B)` | Element-wise subtraction ($A - B$). Requires identical dimensions and types. |
| `Matrix* multiply_matrices(const Matrix* A, const Matrix* B)` | Standard matrix product ($A \times B$). Requires `A->cols == B->rows` and matching types. |

### Transformations & Copying

| Function | Description |
| :--- | :--- |
| `Matrix* clone_matrix(const Matrix* A)` | Allocates and creates an independent deep copy of matrix `A`. |
| `void rotate_matrix(Matrix* mat)` | Rotates matrix 90 degrees clockwise in-place, swapping rows and columns. |
| `void resize_matrix(Matrix* mat, size_t new_rows, size_t new_cols)` | Resizes matrix dimensions in-place, copying over overlapping data. |

### Comparison Utilities

| Function | Description |
| :--- | :--- |
| `bool instance_wise_comparison(const Matrix* A, const Matrix* B)` | Returns `true` if `A` and `B` point to the exact same memory instance. |
| `bool element_wise_comparison(const Matrix* A, const Matrix* B)` | Returns `true` if `A` and `B` have identical dimensions, types, and element values. |

---

## Building & Testing

### Prerequisites
- A C compiler supporting C99/C11 (e.g., `gcc` or `clang`).
- GNU `make`.

### Build Commands
```bash
# Build the test suite binary
make

# Build just the matrix object file
make matrix.o

# Run tests
./test

# Clean build artifacts
make clean
```

---

## Quickstart Example

```c
#include "matrix.h"
#include <stdio.h>

int main(void) {
    // 1. Initialize a 2x3 matrix of floats
    Matrix* A = matrix_init(2, 3, FLOAT);

    // 2. Populate elements
    float val1 = 1.5f, val2 = 2.5f;
    set_elem(A, 0, 0, &val1);
    set_elem(A, 0, 1, &val2);

    // 3. Inspect element
    float* elem = (float*)get_elem(A, 0, 1);
    printf("A[0][1] = %.2f\n", *elem);

    // 4. Clone matrix
    Matrix* B = clone_matrix(A);

    // 5. Compare
    if (element_wise_comparison(A, B)) {
        printf("A and B are element-wise equal.\n");
    }

    // 6. Free resources
    matrix_free(A);
    matrix_free(B);
    return 0;
}
```

---

## Memory Management Guidelines

1. **Matrix Instances**: Always pair `matrix_init()`, `clone_matrix()`, `create_subset_matrix()`, `add_matrices()`, `subtract_matrices()`, and `multiply_matrices()` with `matrix_free()`.
2. **Row & Column Extractions**: Calling `get_row()` or `get_col()` allocates a fresh array of the row or column elements. The caller is responsible for freeing this memory with `free()`.
3. **In-place Operations**: Functions like `rotate_matrix()` and `resize_matrix()` free and reallocate internal data buffers automatically on the existing `Matrix*` handle.
