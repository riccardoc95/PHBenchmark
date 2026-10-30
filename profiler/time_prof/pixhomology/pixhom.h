#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#  ifdef MODULE_API_EXPORTS
#    define MODULE_API __declspec(dllexport)
#  else
#    define MODULE_API __declspec(dllimport)
#  endif
#else
#  define MODULE_API
#endif


// Define a struct to store the result
typedef struct {
    double *data;
    int *posix;
    int length;
} Result;

// Define a struct to store information about u_points
typedef struct {
    int c_point;
    int u_point;
} UPoint;


// Function to find Argmin and Argmax of array
void findArgminArgmax();

// Function to compare UPoints for qsort
int compareUPoints(const void *a, const void *b);


// Function to find Upoints
void findUpoints();


// FreeMemory function
MODULE_API void freemem();


#ifdef __cplusplus
}
#endif
