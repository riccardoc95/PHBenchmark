#ifndef PIXHOM_HPP
#define PIXHOM_HPP

// Define a struct to store the result
struct Result {
    std::vector <double> data;
    std::vector <int> posix;
    int length;
};

// Define a struct to store information about u_points
struct UPoint {
    int c_point;
    int u_point;
};

// PixHomology core functions
Result computePH(double* inputArr, int numR, int numC);
Result computePH1(double* inputArr, int numR, int numC);
Result test(double *inputArr, int numR, int numC);
int find_root(int start_idx,
              const std::vector<int8_t>& mpatch_dir,
              int numRows,
              int numCols);
int find_parent(int x, std::vector<int>& parent);
void union_merge(int a, int b, std::vector<int>& parent);
void freemem();

#endif // PIXHOM_HPP
