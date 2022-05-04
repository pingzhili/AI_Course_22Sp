#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <set>
#include <ctime>
#include <climits>
using namespace std;

#define MAX_LENGTH 524288
#define N 5

typedef int _Matrix[N][N];
typedef std::pair<int, int> position;

string test_case = "";
int prev_step[MAX_LENGTH];
int prev_direct[MAX_LENGTH];
const int dx[4] = {1, -1, 0, 0};
const int dy[4] = {0, 0, 1, -1};
const string directions[4] = {"D", "U", "R", "L"};
vector< vector<int> > input_pos(N, vector<int>(N, 0));
vector< vector<int> > target_pos(N, vector<int>(N, 0));
unordered_map<int, position> planet_to_target_pos;

class Matrix{
    public:
    _Matrix data;
    bool operator<(const Matrix &other) const{
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (data[i][j] != other.data[i][j])
                    return data[i][j] < other.data[i][j];
        return false;
    }
};

class MatrixState{
    public:
    Matrix m;
    int step, x, y, g, h;
    MatrixState(){}
    MatrixState(Matrix m_init, int step_init, int x_init, int y_init, int g_init, int h_init){
        m = m_init;
        step = step_init;
        g = g_init;
        h = h_init;
        x = x_init;
        y = y_init;
    }
    bool operator<(MatrixState other) const{
        return g + h > other.g + other.h;
    }
} M;

int A_h1(const vector< vector<int> > &start, const vector<vector< int> > &target);
int A_h2(const vector< vector<int> > &start, const vector<vector< int> > &target);
int IDA_h1(const vector< vector<int> > &start, const vector<vector< int> > &target);
int IDA_h2(const vector< vector<int> > &start, const vector<vector<int> > &target);
bool is_reachable(int &nx, int &ny);
int h1(MatrixState state);
int h2(MatrixState state);
int get_min_dist(int p1_x, int p1_y, int p2_x, int p2_y);
void print_path(int step);
bool is_target(MatrixState state, const vector< vector<int> > target);