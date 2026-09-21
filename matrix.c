#include "matrix.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


// Initialize global matrix configuration with default settings: bounds checking enabled, row-major order.
MatrixConfig matrix_config = {true, ROW_MAJOR};


/**
 * Initializes a matrix and allocates memory based on the specified type and dimensions.
 * The memory is zero-initialized to ensure clean setup.
 */
Matrix* matrix_init(size_t rows, size_t cols, MatrixType type){
    Matrix* mat = malloc(sizeof(Matrix));
    if (!mat) {
        fprintf(stderr, "Failed to allocate memory for matrix structure.\n");
        return NULL;
    }
    
    mat->rows = rows;
    mat->cols = cols;
    mat->type = type;

    // Determine the size of the elements based on the datatype
    size_t elem_size = (type == INT ? sizeof(int):
                        type == FLOAT ? sizeof(float):
                        sizeof(double));

    // Calculate total memory required for matrix data
    size_t total_size = rows * cols * elem_size;

    // Allocate memory and check for success
    mat->data = malloc(total_size);
    if(!mat->data) {
        fprintf(stderr, "Memory allocation failed womp womp\n");
        free(mat);  // Free the matrix structure if data allocation fails
        return NULL;
    } 

    //Initialize matrix data to zero so that all elements start with known values
    memset(mat->data, 0, total_size);
    return mat;
}


/**
 * Frees the resources associated with a matrix.
 * This function releases the memory allocated for the matrix data and 
 * resets the matrix structure fields to prevent misuse.
 */
void matrix_free(Matrix* mat) {
    if (mat == NULL) {
        // Guard against freeing a NULL pointer
        return;
    }

    // Free the data associated with the matrix if it is not NULL
    if (mat->data != NULL) {
        free(mat->data);
        mat->data = NULL; // Set data pointer to NULL to avoid dangling pointers
    }

    // Reset other fields to indicate that the matrix is no longer valid
    mat->rows = 0;
    mat->cols = 0;

    //Free the matrix structure itself
    free(mat);
}


/**
 * Check if the given indices are within the bounds of the matrix dimensions.
 */
bool bounds_check(const Matrix* mat, size_t i, size_t j) {
    if (i >= mat->rows || j >= mat->cols) {
        fprintf(stderr, "Error: Index out of bounds (i: %zu, j: %zu, rows: %zu, cols: %zu)\n", i, j, mat->rows, mat->cols);
        return false;
    }
    return true;
}


/**
 * Adjusts global matrix configuration. Useful for changing behavior across all matrix operations.
 */
void set_matrix_config(bool bounds_check, StorageOrder order) {
    matrix_config.bounds_check = bounds_check;
    matrix_config.storage_order = order;
}


/**
 * Retrieve the number of rows in the matrix.
 */
size_t get_row_dim(const Matrix* mat) {
    if (mat == NULL) {
        return 0; //
    }
    return mat->rows;
}

/**
 * Retrieve the number of columns in the matrix.
 */
size_t get_col_dim(const Matrix* mat) {
    if (mat == NULL) {
        return 0; 
    }
    return mat->cols;
}


/**
 * Set an individual element in the matrix at the specified row and column index.
 * This function handles different data types and incorporates optional bounds checking.
 */
void set_elem(Matrix* mat, size_t i, size_t j, void* elem) {
    // Check if bounds checking is enabled and verify the indices are within bounds
    if (matrix_config.bounds_check && !bounds_check(mat, i, j)) {
        return;
    }

    // Calculate the index based on the storage order
    size_t index;
    if (matrix_config.storage_order == ROW_MAJOR) {
        index = i * mat->cols + j;
    } else {  // COL_MAJOR
        index = j * mat->rows + i;
    }

    // Set the element based on the type of the matrix
    switch (mat->type) {
        case INT:
            //printf("Before setting: Matrix[%zu][%zu] = %d\n", i, j, ((int*)mat->data)[index]);
            ((int*)mat->data)[index] = *(int*)elem;
            //printf("After setting: Matrix[%zu][%zu] = %d\n", i, j, ((int*)mat->data)[index]);
            break;
        case FLOAT:
            //printf("Before setting: Matrix[%zu][%zu] = %f\n", i, j, ((float*)mat->data)[index]);
            ((float*)mat->data)[index] = *(float*)elem;
            //printf("After setting: Matrix[%zu][%zu] = %f\n", i, j, ((float*)mat->data)[index]);
            break;
        case DOUBLE:
            //printf("Before setting: Matrix[%zu][%zu] = %f\n", i, j, ((double*)mat->data)[index]);
            ((double*)mat->data)[index] = *(double*)elem;
            //printf("After setting: Matrix[%zu][%zu] = %f\n", i, j, ((double*)mat->data)[index]);
            break;
        default:
            fprintf(stderr, "Error: Unsupported data type for setting element\n");
            return;
    }
}


/**
 * Retrieve an individual element from the matrix at the specified row and column index.
 * This function uses global configuration settings to check for bounds and
 * handle matrix storage order.
 */
void* get_elem(const Matrix* mat, size_t i, size_t j) {
    // Check if bounds checking is enabled and verify the indices are within bounds
    if (matrix_config.bounds_check) {
        if (!bounds_check(mat, i, j)) {
            return NULL;  // Return NULL if indices are out of bounds
        }
    }

    // Calculate the index based on the storage order
    size_t index;
    if (matrix_config.storage_order == ROW_MAJOR) {
        index = i * mat->cols + j;
    } else {  // COL_MAJOR
        index = j * mat->rows + i;
    }

    // Return a pointer to the element based on the type of the matrix
    switch (mat->type) {
        case INT:
            return &((int*)mat->data)[index];
        case FLOAT:
            return &((float*)mat->data)[index];
        case DOUBLE:
            return &((double*)mat->data)[index];
        default:
            fprintf(stderr, "Error: Unsupported data type for retrieval\n");
            return NULL;
    }
}


/**
 * Set an entire row in the matrix with the provided data.
 * This function copies data from the provided array into the specified row
 * of the matrix, respecting the matrix's storage order and data type.
 */
void set_row(Matrix* mat, size_t row, void* data) {
    // Perform bounds check if enabled
    if (matrix_config.bounds_check && (row >= mat->rows)) {
        fprintf(stderr, "Error: Row index out of bounds (row: %zu, max rows: %zu)\n", row, mat->rows);
        return;
    }

    // Calculate the size of one row based on matrix type
    size_t row_size = mat->cols * (mat->type == INT ? sizeof(int) :
                                   mat->type == FLOAT ? sizeof(float) :
                                   sizeof(double));

    // Copy data to the specified row based on the storage order
    if (matrix_config.storage_order == ROW_MAJOR) {
        // Directly copy into the correct position for row-major order
        memcpy((char*)mat->data + row * row_size, data, row_size);
    } else {
        // For column-major, we need to copy element by element
        size_t col = 0;
        for (; col < mat->cols; col++) {
            size_t index = col * mat->rows + row;
            switch (mat->type) {
                case INT:
                    ((int*)mat->data)[index] = ((int*)data)[col];
                    break;
                case FLOAT:
                    ((float*)mat->data)[index] = ((float*)data)[col];
                    break;
                case DOUBLE:
                    ((double*)mat->data)[index] = ((double*)data)[col];
                    break;
                default:
                    fprintf(stderr, "Error: Unsupported data type for setting row\n");
                    return;
            }
        }
    }
}


/**
 * Set an entire column in the matrix with the provided data.
 * This function copies data from the provided array into the specified column
 * of the matrix, respecting the matrix's storage order and data type.
 */
void set_col(Matrix* mat, size_t col, void* data) {
    // Perform bounds check if enabled
    if (matrix_config.bounds_check && (col >= mat->cols)) {
        fprintf(stderr, "Error: Column index out of bounds (col: %zu, max cols: %zu)\n", col, mat->cols);
        return;
    }

    // Calculate the size of one column based on matrix type
    size_t col_size = mat->rows * (mat->type == INT ? sizeof(int) :
                                   mat->type == FLOAT ? sizeof(float) :
                                   sizeof(double));

    // Copy data to the specified column based on the storage order
    if (matrix_config.storage_order == COL_MAJOR) {
        // Directly copy into the correct position for column-major order
        memcpy((char*)mat->data + col * col_size, data, col_size);
    } else {
        // For row-major, we need to copy element by element
        size_t row = 0;
        for (; row < mat->rows; row++) {
            size_t index = row * mat->cols + col;
            switch (mat->type) {
                case INT:
                    ((int*)mat->data)[index] = ((int*)data)[row];
                    break;
                case FLOAT:
                    ((float*)mat->data)[index] = ((float*)data)[row];
                    break;
                case DOUBLE:
                    ((double*)mat->data)[index] = ((double*)data)[row];
                    break;
                default:
                    fprintf(stderr, "Error: Unsupported data type for setting column\n");
                    return;
            }
        }
    }
}


/**
 * Retrieve an entire row from the matrix. This function handles both row-major and column-major
 * storage orders. The function allocates memory to ensure the row data
 * is contiguous. The caller is responsible for freeing this memory.
 */
void* get_row(const Matrix* mat, size_t row) {
    // First, check if the requested row index is within the bounds of the matrix.
    if (matrix_config.bounds_check && row >= mat->rows) {
        fprintf(stderr, "Error: Row index out of bounds (row: %zu, max rows: %zu)\n", row, mat->rows);
        return NULL;
    }

    // Determine the size of each element based on the matrix type.
    size_t elem_size = (mat->type == INT ? sizeof(int) :
                        mat->type == FLOAT ? sizeof(float) :
                        sizeof(double));
    // Calculate the total size of the row in bytes.
    size_t row_size = mat->cols * elem_size;

    // Allocate memory for the row data to be returned.
    void* row_data = malloc(row_size);
    if (!row_data) {
        fprintf(stderr, "Memory allocation failed for row retrieval\n");
        return NULL;
    }

    // Loop through each column in the specified row.
    for (size_t col = 0; col < mat->cols; col++) {
        // Retrieve each element using the get_elem() function.
        void* elem = get_elem(mat, row, col);
        // Copy the element into the row_data array at the correct position.
        memcpy((char*)row_data + col * elem_size, elem, elem_size);
    }

    // Return the pointer to the newly created row data.
    return row_data;
}


/**
 * Retrieve an entire column from the matrix. This function handles both column-major and row-major
 * storage orders. The function allocates memory to ensure the column data
 * is contiguous. The caller is responsible for freeing this memory.
 */
void* get_col(const Matrix* mat, size_t col) {
    // Check if the requested column index is within the bounds of the matrix.
    if (matrix_config.bounds_check && col >= mat->cols) {
        fprintf(stderr, "Error: Column index out of bounds (col: %zu, max cols: %zu)\n", col, mat->cols);
        return NULL;
    }

    // Determine the size of each element based on the matrix type.
    size_t elem_size = (mat->type == INT ? sizeof(int) :
                        mat->type == FLOAT ? sizeof(float) :
                        sizeof(double));
    // Calculate the total size of the column in bytes.
    size_t col_size = mat->rows * elem_size;

    // Allocate memory for the column data to be returned.
    void* col_data = malloc(col_size);
    if (!col_data) {
        fprintf(stderr, "Memory allocation failed for column retrieval\n");
        return NULL;
    }

    // Loop through each row in the specified column.
    for (size_t row = 0; row < mat->rows; row++) {
        // Retrieve each element using the get_elem() function.
        void* elem = get_elem(mat, row, col);
        // Copy the element into the col_data array at the correct position.
        memcpy((char*)col_data + row * elem_size, elem, elem_size);
    }

    // Return the pointer to the newly created column data.
    return col_data;
}


/**
 * Create a new matrix that contains a specified subset of the original matrix.
 * Validates boundaries, initializes a new matrix, and copies the subset data.
 */
Matrix* create_subset_matrix(const Matrix* src, size_t start_row, size_t start_col, size_t num_rows, size_t num_cols) {
    // Validate the requested subset is within the source matrix dimensions
    if (matrix_config.bounds_check && ((start_row + num_rows > src->rows) || (start_col + num_cols > src->cols))) {
        fprintf(stderr, "Error: Subset indices are out of bounds.\n");
        return NULL;
    }

    // Initialize the new matrix for the subset
    Matrix* subset = matrix_init(num_rows, num_cols, src->type);
    if (!subset) {
        fprintf(stderr, "Error: Memory allocation for subset matrix failed.\n");
        return NULL;
    }

    // Copy data from source matrix to the new subset matrix
    for (size_t i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            size_t src_row = start_row + i;
            size_t src_col = start_col + j;
            void* elem = get_elem(src, src_row, src_col);
            set_elem(subset, i, j, elem);
        }
    }

    return subset;
}


/**
 * Sets a subset of a matrix into another matrix at specified starting indices.
 * This involves copying data from each cell of the source matrix to the corresponding
 * position in the destination matrix.
 */
void set_subset_matrix(Matrix* dest, size_t start_row, size_t start_col, const Matrix* src) {
    // Validation if bound checking is on
    if (matrix_config.bounds_check) {
        if (start_row + src->rows > dest->rows || start_col + src->cols > dest->cols) {
            fprintf(stderr, "Error: Source matrix exceeds the bounds of the destination matrix.\n");
            return;
        }
    }

    // Dimension Compatibility
    if (src->type != dest->type) {
        fprintf(stderr, "Error: Source and destination matrix types do not match.\n");
        return;
    }

    // Data Copying
    for (size_t i = 0; i < src->rows; i++) {
        for (size_t j = 0; j < src->cols; j++) {
            void* elem = get_elem(src, i, j);
            if (elem) { // Only copy if get_elem did not return NULL (can happen if out of bounds)
                set_elem(dest, start_row + i, start_col + j, elem);
            }
        }
    }
}


/**
 * Adds two matrices of the same dimensions and data type.
 * Validates that both matrices have the same dimensions and data type before proceeding.
 */
Matrix* add_matrices(const Matrix* A, const Matrix* B) {
    // Check dimensions
    if (A->rows != B->rows || A->cols != B->cols) {
        fprintf(stderr, "Error: Matrices dimensions do not match.\n");
        return NULL;
    }

    // Check data type compatibility
    if (A->type != B->type) {
        fprintf(stderr, "Error: Matrices data types do not match.\n");
        return NULL;
    }

    // Create result matrix
    Matrix* result = matrix_init(A->rows, A->cols, A->type);
    if (result == NULL) {
        return NULL; // matrix_init already handles the error message
    }

    // Add matrices
    size_t total_elements = A->rows * A->cols;
    for (size_t i = 0; i < total_elements; i++) {
        switch (A->type) {
            case INT:
                ((int*)result->data)[i] = ((int*)A->data)[i] + ((int*)B->data)[i];
                break;
            case FLOAT:
                ((float*)result->data)[i] = ((float*)A->data)[i] + ((float*)B->data)[i];
                break;
            case DOUBLE:
                ((double*)result->data)[i] = ((double*)A->data)[i] + ((double*)B->data)[i];
                break;
            default:
                fprintf(stderr, "Error: Unsupported data type for addition.\n");
                matrix_free(result);
                return NULL;
        }
    }

    return result;
}


/**
 * Subtracts two matrices of the same dimensions and data type.
 * Validates that both matrices have the same dimensions and data type before proceeding.
 */
Matrix* subtract_matrices(const Matrix* A, const Matrix* B) {
    // Check dimensions
    if (A->rows != B->rows || A->cols != B->cols) {
        fprintf(stderr, "Error: Matrices dimensions do not match.\n");
        return NULL;
    }

    // Check data type compatibility
    if (A->type != B->type) {
        fprintf(stderr, "Error: Matrices data types do not match.\n");
        return NULL;
    }

    // Create result matrix
    Matrix* result = matrix_init(A->rows, A->cols, A->type);
    if (result == NULL) {
        return NULL; // matrix_init already handles the error message
    }

    // Subtract matrices
    size_t total_elements = A->rows * A->cols;
    for (size_t i = 0; i < total_elements; i++) {
        switch (A->type) {
            case INT:
                ((int*)result->data)[i] = ((int*)A->data)[i] - ((int*)B->data)[i];
                break;
            case FLOAT:
                ((float*)result->data)[i] = ((float*)A->data)[i] - ((float*)B->data)[i];
                break;
            case DOUBLE:
                ((double*)result->data)[i] = ((double*)A->data)[i] - ((double*)B->data)[i];
                break;
            default:
                fprintf(stderr, "Error: Unsupported data type for subtraction.\n");
                matrix_free(result);
                return NULL;
        }
    }

    return result;
}


/**
 * Multiplies two matrices if their dimensions are compatible.
 * Validates that the number of columns in A matches the number of rows in B
 * and that both matrices have the same data type before proceeding.
 */
Matrix* multiply_matrices(const Matrix* A, const Matrix* B) {
    // Check dimension compatibility for matrix multiplication
    if (A->cols != B->rows) {
        fprintf(stderr, "Error: Matrices dimensions are not compatible for multiplication.\n");
        return NULL;
    }

    // Check data type compatibility
    if (A->type != B->type) {
        fprintf(stderr, "Error: Matrices data types do not match.\n");
        return NULL;
    }

    // Create result matrix with dimensions A->rows x B->cols
    Matrix* result = matrix_init(A->rows, B->cols, A->type);
    if (result == NULL) {
        return NULL; // matrix_init already handles the error message
    }

    // Perform matrix multiplication
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < B->cols; j++) {
            // Initialize the element at result[i][j] to zero
            switch (A->type) {
                case INT:
                    ((int*)result->data)[i * B->cols + j] = 0;
                    for (size_t k = 0; k < A->cols; k++) {
                        ((int*)result->data)[i * B->cols + j] += ((int*)A->data)[i * A->cols + k] * ((int*)B->data)[k * B->cols + j];
                    }
                    break;
                case FLOAT:
                    ((float*)result->data)[i * B->cols + j] = 0.0f;
                    for (size_t k = 0; k < A->cols; k++) {
                        ((float*)result->data)[i * B->cols + j] += ((float*)A->data)[i * A->cols + k] * ((float*)B->data)[k * B->cols + j];
                    }
                    break;
                case DOUBLE:
                    ((double*)result->data)[i * B->cols + j] = 0.0;
                    for (size_t k = 0; k < A->cols; k++) {
                        ((double*)result->data)[i * B->cols + j] += ((double*)A->data)[i * A->cols + k] * ((double*)B->data)[k * B->cols + j];
                    }
                    break;
                default:
                    fprintf(stderr, "Error: Unsupported data type for multiplication.\n");
                    matrix_free(result);
                    return NULL;
            }
        }
    }

    return result;
}


/**
 * Creates a deep copy of a matrix.
 * This involves initializing a new matrix with the same dimensions and data type,
 * and then copying each element from the original matrix to the new one.
 */
Matrix* clone_matrix(const Matrix* A) {
    // Initialize the new matrix
    Matrix* copy = matrix_init(A->rows, A->cols, A->type);
    if (copy == NULL) {
        return NULL; // matrix_init already handles the error message
    }

    // Copy data from the original matrix to the new matrix
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < A->cols; j++) {
            void* elem = get_elem(A, i, j);
            set_elem(copy, i, j, elem);
        }
    }

    return copy;
}


/**
 * Compares two matrices for instance-wise equivalence.
 * This function checks if the two matrices are the same instance (i.e., occupy the same memory space).
 */
bool instance_wise_comparison(const Matrix* A, const Matrix* B) {
    // Compare the pointers directly
    return A == B;
}


/**
 * Compares two matrices for element-wise equivalence.
 * This involves checking if the dimensions, data types, and each element
 * in the matrices are identical.
 */
bool element_wise_comparison(const Matrix* A, const Matrix* B) {
    // Check if dimensions match
    if (A->rows != B->rows || A->cols != B->cols) {
        return false;
    }

    // Check if data types match
    if (A->type != B->type) {
        return false;
    }

    // Compare each element
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < A->cols; j++) {
            void* elemA = get_elem(A, i, j);
            void* elemB = get_elem(B, i, j);

            switch (A->type) {
                case INT:
                    if (*(int*)elemA != *(int*)elemB) return false;
                    break;
                case FLOAT:
                    if (*(float*)elemA != *(float*)elemB) return false;
                    break;
                case DOUBLE:
                    if (*(double*)elemA != *(double*)elemB) return false;
                    break;
                default:
                    fprintf(stderr, "Error: Unsupported data type for comparison.\n");
                    return false;
            }
        }
    }

    // If all checks passed, matrices are element-wise equivalent
    return true;
}


/**
 * Rotates a matrix 90 degrees clockwise.
 * This involves creating a new matrix with dimensions swapped,
 * copying the contents to their new locations, and updating the original
 * matrix pointer to point to the new rotated matrix.
 */
void rotate_matrix(Matrix* mat) {
    // Create a new matrix with dimensions swapped
    Matrix* rotated = matrix_init(mat->cols, mat->rows, mat->type);
    if (rotated == NULL) {
        fprintf(stderr, "Error: Memory allocation for rotated matrix failed.\n");
        return;
    }

    // Copy contents to the new matrix in rotated positions
    for (size_t i = 0; i < mat->rows; i++) {
        for (size_t j = 0; j < mat->cols; j++) {
            void* elem = get_elem(mat, i, j);
            set_elem(rotated, j, mat->rows - 1 - i, elem);
        }
    }

    // Free the original matrix data but keep the structure
    free(mat->data);

    // Update the original matrix to point to the new rotated matrix
    mat->data = rotated->data;
    mat->rows = rotated->rows;
    mat->cols = rotated->cols;
    mat->type = rotated->type;

    // Free the rotated matrix structure but not the data (since it's now pointed to by mat)
    free(rotated);
}


/**
 * Resizes a matrix to the specified new dimensions.
 * Copies existing elements to the new matrix within the limits of the old and new dimensions.
 *
 * @param mat Pointer to the matrix to be resized.
 * @param new_rows The new number of rows for the matrix.
 * @param new_cols The new number of columns for the matrix.
 */
void resize_matrix(Matrix* mat, size_t new_rows, size_t new_cols) {
    // Initialize a new matrix with the specified new dimensions
    Matrix* resized = matrix_init(new_rows, new_cols, mat->type);
    if (resized == NULL) {
        fprintf(stderr, "Error: Memory allocation for resized matrix failed.\n");
        return;
    }

    // Copy existing elements to the new matrix within the limits of the old and new dimensions
    size_t min_rows = (new_rows < mat->rows) ? new_rows : mat->rows;
    size_t min_cols = (new_cols < mat->cols) ? new_cols : mat->cols;

    for (size_t i = 0; i < min_rows; i++) {
        for (size_t j = 0; j < min_cols; j++) {
            void* elem = get_elem(mat, i, j);
            set_elem(resized, i, j, elem);
        }
    }

    // Free the original matrix data but keep the structure
    free(mat->data);

    // Update the original matrix to point to the new resized matrix
    mat->data = resized->data;
    mat->rows = resized->rows;
    mat->cols = resized->cols;
    mat->type = resized->type;

    // Free the resized matrix structure but not the data (since it's now pointed to by mat)
    free(resized);
}
