#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

double global_sqrt_sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *method1(void *args);

void *method2(void *args);

void *method3(void *args);

void executeMethod1(long long int a, long long int b, int numberOfThreads, long long int rangePerThread);

void executeMethod2(long long int a, long long int b, int numberOfThreads, long long int rangePerThread);

void executeMethod3(long long int a, long long int b, int numberOfThreads, long long int rangePerThread);

typedef struct {
    long long int start;
    long long int end;
} ThreadParameters;




/**
 * @brief Calculates the square root of numbers in a given range and updates a global sum.
 *
 * This function is designed to be executed as a separate thread using pthread.
 * It takes a ThreadParameters struct as argument, which contains the range of numbers to process.
 * The function calculates the square root of each number in the range and adds it to the global_sqrt_sum variable.
 *
 * @param args A pointer to a ThreadParameters struct specifying the start and end of the range.
 * @return void* This function does not return a value.
 */
void *method1(void *args) {
    ThreadParameters *threadParameters = (ThreadParameters *) args;

    for (long long int i = threadParameters->start; i <= threadParameters->end; ++i) {
        global_sqrt_sum += sqrt((double) i);
    }

    pthread_exit(NULL);
}

/**
 * @brief Represents a method that calculates the sum of square roots in a specified range using multiple threads.
 *
 * This method is designed to be run in a separate thread to perform the calculation concurrently with other threads.
 * It takes a pointer to a `ThreadParameters` struct as a parameter, which specifies the start and end values of the range for the calculation.
 * The calculated sum is added to the `global_sqrt_sum` variable, using a mutex to ensure thread safety.
 *
 * @param args Pointer to a `ThreadParameters` struct containing the start and end values of the range.
 * @return Void.
 */
void *method2(void *args) {
    ThreadParameters *threadParameters = (ThreadParameters *) args;

    for (long long int i = threadParameters->start; i <= threadParameters->end; ++i) {
        pthread_mutex_lock(&mutex);
        global_sqrt_sum += sqrt((double) i);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}



/**
 * @brief Calculates the sum of square roots for a given range of numbers using multiple threads.
 *
 * This function is used to calculate the sum of square roots for a range of numbers using multiple threads.
 * It receives a struct of type `ThreadParameters` as an argument, which contains the start and end values for the range.
 * Each thread calculates the square root of each number in the range and adds it to a local_sum variable.
 * The local_sum is then added to the global_sqrt_sum using a mutex lock to ensure thread-safe access.
 *
 * @param args A void pointer to a struct of type `ThreadParameters` that defines the range of numbers to calculate the sum of square roots.
 * @return None.
 *
 * @see ThreadParameters
 * @see mutex
 * @see global_sqrt_sum
 */
void *method3(void *args) {
    ThreadParameters *threadParameters = (ThreadParameters *) args;
    double local_sqrt_sum = 0;

    for (long long int i = threadParameters->start; i <= threadParameters->end; ++i) {
        local_sqrt_sum += sqrt((double) i);
    }

    pthread_mutex_lock(&mutex);
    global_sqrt_sum += local_sqrt_sum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}


/**
 * @brief Executes method 3 with multiple threads.
 *
 * This function divides the range between 'a' and 'b' into 'numberOfThreads'
 * parts and creates a thread for each part to calculate the sum of square roots.
 * Each thread receives a 'start' and 'end' value specifying the range of values to
 * calculate the sum of square roots. The main thread waits for all threads to finish
 * before printing the final result.
 *
 * @param a The lower bound of the range.
 * @param b The upper bound of the range.
 * @param numberOfThreads The number of threads to create.
 * @param rangePerThread The size of the range to assign to each thread.
 */
void executeMethod3(long long int a, long long int b, int numberOfThreads, long long int rangePerThread) {
    pthread_t threads[numberOfThreads];
    ThreadParameters threadArgs[numberOfThreads];
    // Calculate the rangePerThread for each thread
    for (int i = 0; i < numberOfThreads; ++i) {
        threadArgs[i].start = a + i * rangePerThread;
        if (i == (numberOfThreads - 1)) {
            threadArgs[i].end = b;
        } else {
            threadArgs[i].end = a + (i + 1) * rangePerThread - 1;
        }
        pthread_create(&threads[i], NULL, method3, (void *) &threadArgs[i]);
    }
    // Wait for all threads to finish
    for (int i = 0; i < numberOfThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Method 3: \n");
    printf("The sum of square roots between %lld and %lld is: %.5e\n", a, b, global_sqrt_sum);
}

/**
  * @brief Executes method 2 with parallel threads.
  *
  * This function executes method 2 with parallel threads. It divides the range from 'a' to 'b' into
  * 'numberOfThreads' sub-ranges, and assigns each sub-range to a separate thread. Each thread
  * calculates the sum of square roots within its assigned sub-range. The final result is the sum of
  * square roots from all threads.
  *
  * @param a                 The starting value of the range.
  * @param b                 The ending value of the range.
  * @param numberOfThreads   The number of threads to be used for parallel execution.
  * @param rangePerThread    The range of values assigned to each thread.
  * @see method2
  */
void executeMethod2(long long int a, long long int b, int numberOfThreads, long long int rangePerThread) {
    pthread_t threads[numberOfThreads];
    ThreadParameters threadArgs[numberOfThreads];

    // Calculate the rangePerThread for each thread
    for (int i = 0; i < numberOfThreads; ++i) {
        threadArgs[i].start = a + i * rangePerThread;
        if (i == (numberOfThreads - 1)) {
            threadArgs[i].end = b;
        } else {
            threadArgs[i].end = a + (i + 1) * rangePerThread - 1;
        }
        pthread_create(&threads[i], NULL, method2, (void *) &threadArgs[i]);
    }
    // Wait for all threads to finish
    for (int i = 0; i < numberOfThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Method 2: \n");
    printf("The sum of square roots between %lld and %lld is: %.5e\n", a, b, global_sqrt_sum);
}


/**
 * @brief Executes method1 using multiple threads.
 *
 * This function splits the range [a, b] into smaller ranges and assigns each range to a separate thread. The number
 * of threads is determined by the parameter numberOfThreads. Each thread calculates the sum of square roots within
 * its assigned range and updates the global_sqrt_sum variable. After all threads have finished, the function prints
 * the final result.
 *
 * @param a             The starting value of the range.
 * @param b             The ending value of the range.
 * @param numberOfThreads   The number of threads to use for calculation.
 * @param rangePerThread    The number of values each thread should process.
 */
void executeMethod1(long long int a, long long int b, int numberOfThreads, long long int rangePerThread) {
    pthread_t threads[numberOfThreads];
    ThreadParameters threadArgs[numberOfThreads];
    // Calculate the range for each thread
    for (int i = 0; i < numberOfThreads; ++i) {
        threadArgs[i].start = a + i * rangePerThread;
        if (i == (numberOfThreads - 1)) {
            threadArgs[i].end = b;
        } else {
            threadArgs[i].end = a + (i + 1) * rangePerThread - 1;
        }
        pthread_create(&threads[i], NULL, method1, (void *) &threadArgs[i]);
    }
    // Wait for all threads to finish
    for (int i = 0; i < numberOfThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Method 1: \n");
    printf("The sum of square roots between %lld and %lld is: %.5e\n", a, b, global_sqrt_sum);
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <a> <b> <c> <d>\n", argv[0]);
        return 1;
    }
    long long int a = atoll(argv[1]);
    long long int b = atoll(argv[2]);
    int c = atoi(argv[3]);
    int d = atoi(argv[4]);
    int methodNumber = d;
    int numberOfThreads = c;
    long long int rangePerThread = (b - a) / numberOfThreads;

    switch (methodNumber) {
        case 1:
            executeMethod1(a, b, numberOfThreads, rangePerThread);
            break;
        case 2:
            executeMethod2(a, b, numberOfThreads, rangePerThread);
            break;
        case 3:
            executeMethod3(a, b, numberOfThreads, rangePerThread);
            break;
        default:
            printf("Invalid method number.\n");
            return 1;
    }
    return 0;
}
