#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Enum for defining matrix storage order.
 */
typedef enum {
    ROW_MAJOR, /**< Store matrix elements row by row. */
    COL_MAJOR  /**< Store matrix elements column by column. */
} StorageOrder;


/**
 * @brief Configuration structure for global matrix settings.
 */
typedef struct {
    bool bounds_check;         /**< Flag to enable/disable bounds checking. */
    StorageOrder storage_order;/**< Global storage order for all matrices. */
} MatrixConfig;


/**
 * @brief Enum for supported matrix data types.
 */
typedef enum {
    INT,   /**< Matrix stores integers. */
    FLOAT, /**< Matrix stores floating-point numbers. */
    DOUBLE /**< Matrix stores double-precision floating-point numbers. */
} MatrixType;


/**
 * @brief Structure representing a matrix.
 */
typedef struct {
    size_t rows;     /**< Number of rows in the matrix. */
    size_t cols;     /**< Number of columns in the matrix. */
    void *data;      /**< Pointer to the matrix data, stored as a contiguous block. */
    MatrixType type; /**< Data type of the matrix elements. */
} Matrix;


/**
 * Initialize a new matrix with specified dimensions and data type.
 * 
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 * @param type Data type of the matrix elements (e.g., MATRIX_INT, MATRIX_FLOAT).
 * @return Matrix pointer to the newly created matrix with allocated memory initialized to zero.
 */
Matrix* matrix_init(size_t rows, size_t cols, MatrixType type);


/**
 * Free the resources associated with a matrix.
 *
 * @param mat Pointer to the matrix to be freed.
 */
void matrix_free(Matrix* mat);


/**
 * Retrieve the number of rows in the matrix.
 *
 * @param mat Pointer to the matrix.
 * @return The number of rows in the matrix.
 */
size_t get_row_dim(const Matrix* mat);

/**
 * Retrieve the number of columns in the matrix.
 *
 * @param mat Pointer to the matrix.
 * @return The number of columns in the matrix.
 */
size_t get_col_dim(const Matrix* mat);


/**
 * Set an individual element in the matrix.
 * 
 * @param mat Pointer to the matrix.
 * @param i Row index of the element to set.
 * @param j Column index of the element to set.
 * @param elem Pointer to the element value to be set.
 */
void set_elem(Matrix* mat, size_t i, size_t j, void* elem);


/**
 * Check if the given indices are within the bounds of the matrix dimensions.
 *
 * @param mat Pointer to the matrix.
 * @param i Row index to check.
 * @param j Column index to check.
 * @return true if both indices are within bounds, false otherwise.
 */
bool bounds_check(const Matrix* mat, size_t i, size_t j);


/**
 * Retrieve an individual element from the matrix.
 * 
 * @param mat Pointer to the matrix.
 * @param i Row index of the element to retrieve.
 * @param j Column index of the element to retrieve.
 * @return void* Pointer to the retrieved element.
 */
void* get_elem(const Matrix* mat, size_t i, size_t j);


/**
 * Set an entire row in the matrix.
 * 
 * @param mat Pointer to the matrix.
 * @param row Row index where data should be set.
 * @param data Pointer to the data to set in the row.
 */
void set_row(Matrix* mat, size_t row, void* data);


/**
 * Set an entire column in the matrix.
 * 
 * @param mat Pointer to the matrix.
 * @param col Column index where data should be set.
 * @param data Pointer to the data to set in the column.
 */
void set_col(Matrix* mat, size_t col, void* data);


/**
 * Retrieve an entire row from the matrix.This function handles both row-major and column-major
 * storage orders. The function allocates memory to ensure the row data
 * is contiguous. The caller is responsible for freeing this memory.
 * 
 * @param mat Pointer to the matrix.
 * @param row Row index to retrieve.
 * @return void* Pointer to the retrieved row,or NULL if out of bounds. The user must free this memory if column-major storage is used.
 */
void* get_row(const Matrix* mat, size_t row);


/**
 * Retrieve an entire column from the matrix. This function handles both column-major and row-major
 * storage orders. The function allocates memory to ensure the column data
 * is contiguous. The caller is responsible for freeing this memory.
 *
 * @param mat Pointer to the matrix.
 * @param col Column index to retrieve.
 * @return void* Pointer to the retrieved column, or NULL if out of bounds. The user must free this memory if row-major storage is used.
 */
void* get_col(const Matrix* mat, size_t col);


/**
 * Create a new matrix that contains a specified subset of the original matrix.
 * Validates boundaries, initializes a new matrix, and copies the subset data.
 *
 * @param src Pointer to the source matrix.
 * @param start_row Starting row index for the subset.
 * @param start_col Starting column index for the subset.
 * @param num_rows Number of rows in the subset.
 * @param num_cols Number of columns in the subset.
 * @return Matrix* Pointer to the new subset matrix, or NULL if operation fails.
 */
Matrix* create_subset_matrix(const Matrix* src, size_t start_row, size_t start_col, size_t num_rows, size_t num_cols);


/**
 * Sets a subset of a matrix into another matrix at specified starting indices.
 * Ensures that the subset defined by a source matrix fits within the bounds of the destination matrix.
 * Uses data copying from the source matrix to the destination matrix.
 *
 * @param dest Pointer to the destination matrix.
 * @param start_row Starting row index in the destination matrix.
 * @param start_col Starting column index in the destination matrix.
 * @param src Pointer to the source matrix which contains the subset data.
 */
void set_subset_matrix(Matrix* dest, size_t start_row, size_t start_col, const Matrix* src);


/**
 * Update the global configuration settings for all matrices.
 * 
 * @param bounds_check Enable or disable bounds checking.
 * @param order Define the storage order for matrices (ROW_MAJOR or COL_MAJOR).
 */
void set_matrix_config(bool bounds_check, StorageOrder order);


/**
 * Adds two matrices of the same dimensions and data type.
 * Validates that both matrices have the same dimensions and data type before proceeding.
 *
 * @param A Pointer to the first matrix operand.
 * @param B Pointer to the second matrix operand.
 * @return Matrix* Pointer to the new matrix with the results of the addition,
 *         or NULL if there is a dimension mismatch, incompatible types, or memory allocation failure.
 */
Matrix* add_matrices(const Matrix* A, const Matrix* B);


/**
 * Subtracts two matrices of the same dimensions and data type.
 * Validates that both matrices have the same dimensions and data type before proceeding.
 *
 * @param A Pointer to the first matrix operand.
 * @param B Pointer to the second matrix operand.
 * @return Matrix* Pointer to the new matrix with the results of the subtraction,
 *         or NULL if there is a dimension mismatch, incompatible types, or memory allocation failure.
 */
Matrix* subtract_matrices(const Matrix* A, const Matrix* B);


/**
 * Multiplies two matrices if their dimensions are compatible.
 * Validates that the number of columns in A matches the number of rows in B
 * and that both matrices have the same data type before proceeding.
 *
 * @param A Pointer to the first matrix operand.
 * @param B Pointer to the second matrix operand.
 * @return Matrix* Pointer to the new matrix with the results of the multiplication,
 *         or NULL if there is a dimension mismatch, incompatible types, or memory allocation failure.
 */
Matrix* multiply_matrices(const Matrix* A, const Matrix* B);


/**
 * Creates a deep copy of a matrix.
 *
 * @param A Pointer to the matrix to be cloned.
 * @return Matrix* Pointer to the new matrix that is a deep copy of A,
 *         or NULL if memory allocation fails.
 */
Matrix* clone_matrix(const Matrix* A);


/**
 * Compares two matrices for instance-wise equivalence.
 * This function checks if the two matrices are the same instance (i.e., occupy the same memory space).
 *
 * @param A Pointer to the first matrix.
 * @param B Pointer to the second matrix.
 * @return bool True if the matrices are the same instance, false otherwise.
 */
bool instance_wise_comparison(const Matrix* A, const Matrix* B);


/**
 * Compares two matrices for element-wise equivalence.
 * This involves checking if the dimensions, data types, and each element
 * in the matrices are identical.
 *
 * @param A Pointer to the first matrix.
 * @param B Pointer to the second matrix.
 * @return bool True if the matrices are element-wise equivalent, false otherwise.
 */
bool element_wise_comparison(const Matrix* A, const Matrix* B);


/**
 * Rotates a matrix 90 degrees clockwise.
 *
 * @param mat Pointer to the matrix to be rotated.
 */
void rotate_matrix(Matrix* mat);


/**
 * Resizes a matrix to the specified new dimensions.
 * Copies existing elements to the new matrix within the limits of the old and new dimensions.
 *
 * @param mat Pointer to the matrix to be resized.
 * @param new_rows The new number of rows for the matrix.
 * @param new_cols The new number of columns for the matrix.
 */
void resize_matrix(Matrix* mat, size_t new_rows, size_t new_cols);


/**
 * Declare a global configuration variable that can be accessed and modified
 * by all matrix functions to adjust behavior library-wide.
 */
extern MatrixConfig matrix_config;

#endif // MATRIX_H

