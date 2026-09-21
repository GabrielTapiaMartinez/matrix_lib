#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Function prototypes
void parse_and_fill(const char* filename, Matrix* matrices[5], int* intArray1, int* intArray2, float* floatArray, double* doubleArray1, double* doubleArray2);
int compare_matrices(Matrix* matrices[5], double* arrays[5]);
void test_get_col_int_matrix(Matrix* mat, int* known_values);
void test_get_row_float_matrix(Matrix* mat, float* known_values);
void test_get_elem_double_matrix(Matrix* mat, double* known_values);
void test_get_elem_int_matrix(Matrix* mat, int* known_values);
void test_get_elem_float_matrix(Matrix* mat, float* known_values);
void test_create_subset_matrix(Matrix* src);
void test_set_subset_matrix(Matrix* dest, const Matrix* src, size_t start_row, size_t start_col);
void test_add_matrices(const Matrix* A, const Matrix* B);
void test_multiply_matrices(const Matrix* A, const Matrix* B);
void test_subtract_matrices(const Matrix* A, const Matrix* B);
void test_clone_and_compare(const Matrix* original);
void test_matrix_comparisons(const Matrix* matA, const Matrix* matB, const Matrix* matC);
void test_and_rotate_float_matrix(Matrix* mat);


int main() {
    // Create arrays to store the expected data
    int arr1[100]; // 10x10 INT matrix
    int arr2[9];   // 3x3 INT matrix
    float arr3[12];  // 3x4 FLOAT matrix
    double arr4[16];  // 4x4 DOUBLE matrix
    double arr5[16];  // 4x4 DOUBLE matrix

    // Array of pointers to these arrays for easier handling
    //int* arrays[5] = {arr1, arr2, arr3, arr4, arr5};

    // Create matrices
    Matrix* matrices[5];
    for (int i = 0; i < 5; i++) {
        matrices[i] = malloc(sizeof(Matrix));
    }

    // Parse the CSV file and fill the matrices and arrays
    parse_and_fill("matrix_test_data.csv", matrices, arr1, arr2, arr3, arr4, arr5);

    // Test the 3rd column of the first INT 10x10 matrix
    if (matrices[0]) {
        test_get_col_int_matrix(matrices[0], arr1);
        //test_get_elem_int_matrix(matrices[0], arr1);
    } else {
        fprintf(stderr, "Error: Matrix 0 is not initialized.\n");
    }

    // Test the 2nd row of the 3rd FLOAT 3x4 matrix
    if (matrices[2]) {
        test_get_row_float_matrix(matrices[2], arr3);
        //test_get_elem_float_matrix(matrices[2], arr3);
    } else {
        fprintf(stderr, "Error: Matrix 2 is not initialized.\n");
    }

    // Test specific elements (1,2), (2,2), (3,1) of the 4th DOUBLE 4x4 matrix
    if (matrices[3]) {
        test_get_elem_double_matrix(matrices[3], arr4);
    } else {
        fprintf(stderr, "Error: Matrix 3 is not initialized.\n");
    }

    // Run the subset creation test
    test_create_subset_matrix(matrices[0]);

    //Run the set subset matric test
    test_set_subset_matrix(matrices[0], matrices[1], 4, 5);

    //Run the add matrices test
    test_add_matrices(matrices[3], matrices[4]);

    //Run the multiply  matrices test
    test_multiply_matrices(matrices[3], matrices[4]);

    //Run the subtract matrices test
    test_subtract_matrices(matrices[3], matrices[4]);

    //Run test matrix comparisons
    Matrix* willy = matrices[3];

    test_matrix_comparisons(matrices[3], matrices[4], willy);

    //Run clone and compare test
    test_clone_and_compare(matrices[0]);

    //Run rotate test
    test_and_rotate_float_matrix(matrices[2]);


    // Free the matrices
    for (int i = 0; i < 5; i++) {
        matrix_free(matrices[i]);
    }
    printf("All good");
    return 0;
}


//Parses and fill Matrices and arrays
void parse_and_fill(const char* filename, Matrix* matrices[5], int* intArray1, int* intArray2, float* floatArray, double* doubleArray1, double* doubleArray2) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    char line[1024];
    int matIndex = 0;
    while (fgets(line, sizeof(line), file) && matIndex < 5) {
        if (line[0] == '#') continue; // skip comments

        char* token = strtok(line, ",");
        MatrixType type = (strcmp(token, "INT") == 0 ? INT : (strcmp(token, "FLOAT") == 0 ? FLOAT : DOUBLE));
        size_t rows = atoi(strtok(NULL, ","));
        size_t cols = atoi(strtok(NULL, ","));

        matrices[matIndex] = matrix_init(rows, cols, type);

        void* currentArray = NULL;
        switch (matIndex) {
            case 0:
            case 1:
                currentArray = matIndex == 0 ? (void*)intArray1 : (void*)intArray2;
                break;
            case 2:
                currentArray = (void*)floatArray;
                break;
            case 3:
            case 4:
                currentArray = matIndex == 3 ? (void*)doubleArray1 : (void*)doubleArray2;
                break;
        }

        size_t idx = 0;
        while ((token = strtok(NULL, ",")) != NULL && idx < rows * cols) {
            switch (type) {
                case INT:
                    ((int*)currentArray)[idx] = atoi(token);
                    break;
                case FLOAT:
                    ((float*)currentArray)[idx] = atof(token);
                    break;
                case DOUBLE:
                    ((double*)currentArray)[idx] = atof(token);
                    break;
            }
            idx++;
        }

        // Debug output to verify matrix contents right after parsing
        printf("Matrix[%d] after parsing:\n", matIndex);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                size_t index = i * cols + j;
                switch (type) {
                    case INT:
                        printf("%d ", ((int*)currentArray)[index]);
                        break;
                    case FLOAT:
                        printf("%f ", ((float*)currentArray)[index]);
                        break;
                    case DOUBLE:
                        printf("%f ", ((double*)currentArray)[index]);
                        break;
                }
            }
            printf("\n");
        }

        // Fill matrices based on type using appropriate set function
        if (type == INT) {
            for (size_t row = 0; row < rows; row++) {
                for (size_t col = 0; col < cols; col++) {
                    size_t index = row * cols + col;
                    set_elem(matrices[matIndex], row, col, &((int*)currentArray)[index]);
                }
            }
        } else if (type == FLOAT) {
            for (size_t col = 0; col < cols; col++) {
                float tempColData[rows];
                for (size_t row = 0; row < rows; row++) {
                    tempColData[row] = ((float*)currentArray)[row * cols + col];
                }
                set_col(matrices[matIndex], col, tempColData);
            }
        } else if (type == DOUBLE) {
            for (size_t row = 0; row < rows; row++) {
                double tempRowData[cols];
                for (size_t col = 0; col < cols; col++) {
                    tempRowData[col] = ((double*)currentArray)[row * cols + col];
                }
                set_row(matrices[matIndex], row, tempRowData);
            }
        }

        matIndex++;
    }
    fclose(file);
}


void test_get_col_int_matrix(Matrix* mat, int* known_values) {
    int* col_data = get_col(mat, 2);
    if (!col_data) {
        printf("Failed to retrieve column data.\n");
        return;
    }
    int errors = 0;
    for (size_t i = 0; i < mat->rows; i++) {
        if (col_data[i] != known_values[i * mat->cols + 2]) {
            printf("Mismatch at row %zu: expected %d, got %d\n", i, known_values[i * mat->cols + 2], col_data[i]);
            errors++;
        }
    }
    if (errors == 0) {
        printf("Column test passed.\n");
    }
    free(col_data);
}

// Tests the second row retrieval from a 3x4 float matrix.

void test_get_row_float_matrix(Matrix* mat, float* known_values) {
    float* row_data = get_row(mat, 1);
    if (!row_data) {
        printf("Failed to retrieve row data.\n");
        return;
    }
    int errors = 0;
    for (size_t j = 0; j < mat->cols; j++) {
        if (row_data[j] != known_values[1 * mat->cols + j]) {
            printf("Mismatch at column %zu: expected %f, got %f\n", j, known_values[1 * mat->cols + j], row_data[j]);
            errors++;
        }
    }
    if (errors == 0) {
        printf("Row test passed.\n");
    }
    free(row_data);
}

// Tests specific element retrieval from the first 4x4 double matrix.
void test_get_elem_double_matrix(Matrix* mat, double* known_values) {
    // Define the indices of the elements to be tested.
    size_t indices[3][2] = {{1, 2}, {2, 2}, {3, 1}};
    // Define the expected values for the elements at the given indices.
    double expected[3] = {known_values[1 * mat->cols + 2], known_values[2 * mat->cols + 2], known_values[3 * mat->cols + 1]};

    // Initialize error counter.
    int errors = 0;

    // Loop through the specified indices to retrieve and check elements.
    for (int k = 0; k < 3; k++) {
        double* elem = get_elem(mat, indices[k][0], indices[k][1]);
        // Compare the retrieved element with the expected value.
        if (*elem != expected[k]) {
            printf("Mismatch at (%zu,%zu): expected %f, got %f\n", indices[k][0], indices[k][1], expected[k], *elem);
            errors++;
        }
    }
    // Report the result of the test.
    if (errors == 0) {
        printf("Element test passed.\n");
    }
}

// Tests specific element retrieval from an INT matrix.
void test_get_elem_int_matrix(Matrix* mat, int* known_values) {
    // Define the indices of the elements to be tested.
    size_t indices[3][2] = {{1, 2}, {2, 2}, {3, 1}};
    // Initialize error counter.
    int errors = 0;

    // Loop through the specified indices to retrieve and check elements.
    for (int k = 0; k < 3; k++) {
        int* elem = (int*)get_elem(mat, indices[k][0], indices[k][1]);
        // Compare the retrieved element with the expected value.
        if (*elem != known_values[indices[k][0] * mat->cols + indices[k][1]]) {
            printf("Mismatch at (%zu,%zu): expected %d, got %d\n", indices[k][0], indices[k][1], known_values[indices[k][0] * mat->cols + indices[k][1]], *elem);
            errors++;
        }
    }
    // Report the result of the test.
    if (errors == 0) {
        printf("Element test passed.\n");
    }
}

// Tests specific element retrieval from a FLOAT matrix.
void test_get_elem_float_matrix(Matrix* mat, float* known_values) {
    // Define the indices of the elements to be tested.
    size_t indices[3][2] = {{1, 2}, {2, 2}, {3, 1}};
    // Initialize error counter.
    int errors = 0;

    // Loop through the specified indices to retrieve and check elements.
    for (int k = 0; k < 3; k++) {
        float* elem = (float*)get_elem(mat, indices[k][0], indices[k][1]);
        // Compare the retrieved element with the expected value.
        if (*elem != known_values[indices[k][0] * mat->cols + indices[k][1]]) {
            printf("Mismatch at (%zu,%zu): expected %f, got %f\n", indices[k][0], indices[k][1], known_values[indices[k][0] * mat->cols + indices[k][1]], *elem);
            errors++;
        }
    }
    // Report the result of the test.
    if (errors == 0) {
        printf("Element test passed.\n");
    }
}


// Function to test the creation of a subset matrix from a larger matrix
void test_create_subset_matrix(Matrix* src) {

    // Define the start row, start column, and the dimensions of the subset matrix
    size_t start_row = 2;
    size_t start_col = 2;
    size_t num_rows = 3;
    size_t num_cols = 3;

    // Create the subset matrix from the original matrix
    Matrix* subset = create_subset_matrix(src, start_row, start_col, num_rows, num_cols);

    // Check if the subset matrix was created successfully
    if (subset == NULL) {
        fprintf(stderr, "Failed to create subset matrix.\n");
        return;
    }

    // Verify the values in the subset matrix match the corresponding values in the original matrix
    int errors = 0;
    for (size_t i = 0; i < num_rows; i++) {
        for (size_t j = 0; j < num_cols; j++) {
            int* expected_elem = (int*)get_elem(src, start_row + i, start_col + j);
            int* actual_elem = (int*)get_elem(subset, i, j);
            if (*actual_elem != *expected_elem) {
                printf("Mismatch at (%zu,%zu): expected %d, got %d\n", i, j, *expected_elem, *actual_elem);
                errors++;
            }
        }
    }

    // Report the result of the test
    if (errors == 0) {
        printf("Subset matrix test passed.\n");
    } else {
        printf("Subset matrix test failed with %d errors.\n", errors);
    }

    // Free the allocated subset matrix
    matrix_free(subset);
}


//Funtion to test setting a subset matrix to a destination matrix
void test_set_subset_matrix(Matrix* dest, const Matrix* src, size_t start_row, size_t start_col) {
    // Applying the subset matrix to the destination matrix
    set_subset_matrix(dest, start_row, start_col, src);

    // Output the resulting destination matrix for visual verification
    printf("Destination Matrix after setting subset:\n");
    for (size_t i = 0; i < dest->rows; i++) {
        for (size_t j = 0; j < dest->cols; j++) {
            int* elem = (int*)get_elem(dest, i, j);
            printf("%d ", *elem);
        }
        printf("\n");
    }

    // Verification: Check if the values from the source have been correctly copied
    bool mismatch = false;
    for (size_t i = 0; i < src->rows; i++) {
        for (size_t j = 0; j < src->cols; j++) {
            int* src_elem = (int*)get_elem(src, i, j);
            int* dest_elem = (int*)get_elem(dest, start_row + i, start_col + j);
            if (*src_elem != *dest_elem) {
                printf("Mismatch found at dest(%zu,%zu): expected %d, got %d\n", start_row + i, start_col + j, *src_elem, *dest_elem);
                mismatch = true;
            }
        }
    }

    if (!mismatch) {
        printf("Test passed: All elements correctly copied.\n");
    }
}



//Funtion to test adding two matrices
void test_add_matrices(const Matrix* A, const Matrix* B) {
    // Call the function to add the matrices
    Matrix* result = add_matrices(A, B);

    // Check if the result matrix was successfully created
    if (result == NULL) {
        fprintf(stderr, "Failed to add matrices.\n");
        return;
    }

    // Output the resulting matrix to verify its contents
    printf("Resulting Matrix from Addition:\n");
    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->cols; j++) {
            double* elem = (double*)get_elem(result, i, j);
            printf("%.2f ", *elem);
        }
        printf("\n");
    }

    // Free the result matrix after use
    matrix_free(result);
}


//Funtion to test multiplication between two matrices
void test_multiply_matrices(const Matrix* A, const Matrix* B) {
    // Call the function to multiply the matrices
    Matrix* result = multiply_matrices(A, B);

    // Check if the result matrix was successfully created
    if (result == NULL) {
        fprintf(stderr, "Failed to multiply matrices.\n");
        return;
    }

    // Output the resulting matrix to verify its contents
    printf("Resulting Matrix from Multiplication:\n");
    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->cols; j++) {
            double* elem = (double*)get_elem(result, i, j);
            printf("%.2f ", *elem);
        }
        printf("\n");
    }

    // Free the result matrix after use
    matrix_free(result);
}


//Funtion to test substracting two matrices
void test_subtract_matrices(const Matrix* A, const Matrix* B) {
    // Call the function to subtract the matrices
    Matrix* result = subtract_matrices(A, B);

    // Check if the result matrix was successfully created
    if (result == NULL) {
        fprintf(stderr, "Failed to subtract matrices.\n");
        return;
    }

    // Output the resulting matrix to verify its contents
    printf("Resulting Matrix from Subtraction:\n");
    for (size_t i = 0; i < result->rows; i++) {
        for (size_t j = 0; j < result->cols; j++) {
            double* elem = (double*)get_elem(result, i, j);
            printf("%.2f ", *elem);
        }
        printf("\n");
    }

    // Free the result matrix after use
    matrix_free(result);
}

//Function to test clone,instance wise comparison and element wise comparison
void test_clone_and_compare(const Matrix* original) {
    // Create a clone of the original matrix
    Matrix* cloned = clone_matrix(original);

    // Check if the clone was successfully created
    if (!cloned) {
        fprintf(stderr, "Cloning failed: original matrix could not be cloned.\n");
        return;
    }

    // Test if the cloned matrix is the same instance as the original
    if (instance_wise_comparison(original, cloned)) {
        fprintf(stderr, "Failure: Cloned matrix is the same instance as the original (should be a different instance).\n");
    } else {
        printf("Instance test passed: Cloned matrix is a different instance as expected.\n");
    }

    // Test if the cloned matrix is not element-wise equivalent to the original
    if (!element_wise_comparison(original, cloned)) {
        fprintf(stderr, "Failure: Cloned matrix is not element-wise equivalent to the original.\n");
    } else {
        printf("Element-wise test passed: Cloned matrix is equivalent to the original as expected.\n");
    }

    // Free the cloned matrix
    matrix_free(cloned);
}


//Function to test elemnt wise and instance wise comparisons
void test_matrix_comparisons(const Matrix* matA, const Matrix* matB, const Matrix* matC) {
    // Test element-wise comparison between matA and matB
    if (element_wise_comparison(matA, matB)) {
        printf("Element-wise comparison passed: MatA and MatB are equivalent.\n");
    } else {
        fprintf(stderr, "Element-wise comparison failed: MatA and MatB are not equivalent.\n");
    }

    // Assign matC to point to matA to simulate a same instance scenario
    const Matrix* instanceCopy = matA;

    // Test instance-wise comparison between instanceCopy and matC
    if (instance_wise_comparison(instanceCopy, matC)) {
        printf("Instance-wise comparison passed: instanceCopy and MatC are the same instance.\n");
    } else {
        fprintf(stderr, "Instance-wise comparison failed: instanceCopy and MatC are not the same instance.\n");
    }
}


//Function to test matrix rotation
void test_and_rotate_float_matrix(Matrix* mat) {
    // Print the original matrix
    printf("Original matrix [%zu x %zu]:\n", mat->rows, mat->cols);
    for (size_t i = 0; i < mat->rows; i++) {
        for (size_t j = 0; j < mat->cols; j++) {
            float* elem = (float*)get_elem(mat, i, j);
            printf("%.2f ", *elem);
        }
        printf("\n");
    }
    printf("\n");

    // Rotate the matrix
    rotate_matrix(mat);

    // Print the rotated matrix
    printf("Rotated matrix [%zu x %zu]:\n", mat->rows, mat->cols);
    for (size_t i = 0; i < mat->rows; i++) {
        for (size_t j = 0; j < mat->cols; j++) {
            float* elem = (float*)get_elem(mat, i, j);
            printf("%.2f ", *elem);
        }
        printf("\n");
    }
    printf("\n");
}
