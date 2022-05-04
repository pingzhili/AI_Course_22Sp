#pragma GCC optimize(3)
#include "a.hpp"


int main(int argc, char *argv[]){
    test_case = argv[1];
    string data_path = "../EXP1_files/data/";
    ifstream infile_input(data_path + "input" + test_case + ".txt");
    ifstream infile_target(data_path + "target" + test_case + ".txt");
    if (infile_input.is_open() && infile_target.is_open()) {
        for (int i=0; i<N; i++){
            for (int j=0; j<N; j++){
                infile_input >> input_pos[i][j];
                infile_target >> target_pos[i][j];
                position position_pair = std::make_pair(i, j);
                planet_to_target_pos[target_pos[i][j]] = position_pair;
            }
        }
    } else{
        cout << "Error opening file" << endl;
        exit(1);
    }
    infile_input.close();
    infile_target.close();
    int final_step = -1;
    auto start = clock();
    if (stoi(argv[2]) == 1){ // A_h1
        final_step = A_h1(input_pos, target_pos);
    } else if (stoi(argv[2]) == 2){ // A_h2
        final_step = A_h2(input_pos, target_pos);
    } else if (stoi(argv[2]) == 3){ // IDA_h1
        final_step = IDA_h1(input_pos, target_pos);
    } else if (stoi(argv[2]) == 4){ // IDA_h2
        final_step = IDA_h2(input_pos, target_pos);
    } else {
        cout << "Wrong input algorithm" << endl;
        return 0;
    }
    auto end = clock();
    print_path(final_step);
    cout << ", " << (double)(end-start)/CLOCKS_PER_SEC << "s" << endl;
    return 0;
}

void print_path(int step){
    if (step == 1){
        return;
    } else {
        print_path(prev_step[step]);
        cout << directions[prev_direct[step]];
    }
}

/* consider edge tunnel */
bool is_reachable(int &x, int &y){ 
    if (x > -1 && x < N && y > -1 && y < N){
        return true;
    } else if (x == -1 && y == N/2){
        x = 4;
        return true;
    } else if (x == N && y == N/2){
        x = 0;
        return true;
    } else if (x == N/2 && y == -1){
        y = 4;
        return true;
    } else if (x == N/2 && y == N){
        y = 0;
        return true;
    } else {
        return false;
    }
}

bool is_target(MatrixState state, const vector< vector<int> > target){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (state.m.data[i][j] != target_pos[i][j]){
                return false;
            }
        }
    }
    return true;
}

int h1(MatrixState state){
    int h = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            if (state.m.data[i][j] != target_pos[i][j] && state.m.data[i][j] != 0){
                h++;
            }
        }
    }
    return h;
}

/* consider edge tunnel */
int get_min_dist(int p1_x, int p1_y, int p2_x, int p2_y){
    int min_dist = abs(p1_x - p2_x) + abs(p1_y - p2_y);
    int up_down_tunnel_min_dist = abs(p1_x - 0) + abs(p1_y - N/2) + abs(p2_x - (N-1)) + abs(p2_y - N/2) + 1;
    int left_right_tunnel_min_dist = abs(p1_x - N/2) + abs(p1_y - 0) + abs(p2_x - N/2) + abs(p2_y - (N-1)) + 1;
    min_dist = min(min_dist, up_down_tunnel_min_dist);
    min_dist = min(min_dist, left_right_tunnel_min_dist);
    return min_dist;
}

/* the sum of min distances between current position and target postion of each planet */
int h2(MatrixState state){
    int h = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            if (state.m.data[i][j] != target_pos[i][j] && state.m.data[i][j] != 0){
                auto pos_pair = planet_to_target_pos[state.m.data[i][j]];
                int pos_x = pos_pair.first;
                int pos_y = pos_pair.second;
                h += get_min_dist(i, j, pos_x, pos_y);
            }
        }
    }
    return h;
}

int A_h1(const vector< vector<int> > &start, const vector<vector< int> > &target){
    std::priority_queue<MatrixState> open_list;
    std::set<Matrix> closed_list;
    int global_step = 1;
    int finish_step = 0;
    M.step = 1;
    M.g = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            M.m.data[i][j] = start[i][j];
            if (start[i][j] == 0){
                M.x = i;
                M.y = j;
            }
        }
    }
    M.h = h1(M);
    open_list.push(M);
    closed_list.insert(M.m);
    while(!open_list.empty()){
        auto C = open_list.top();
        open_list.pop();
        if(is_target(C, target_pos)){
            return C.step;
        }else {
            for (int i=0; i<4; i++) {
                int x = C.x + dx[i];
                int y = C.y + dy[i];
                if (is_reachable(x, y) && C.m.data[x][y] > 0){
                    swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                    if(closed_list.find(C.m) == closed_list.end()){
                        global_step ++;
                        closed_list.insert(C.m);
                        MatrixState new_state(C.m, global_step, x, y, C.g+1, h1(C));
                        open_list.push(new_state);
                        prev_step[global_step] = C.step;
                        prev_direct[global_step] = i;
                    }
                    swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                }
            }
        }
    }
    return finish_step;
}

int A_h2(const vector< vector<int> > &start, const vector<vector< int> > &target){
    std::priority_queue<MatrixState> open_list;
    std::set<Matrix> closed_list;
    int global_step = 1;
    int finish_step = 0;
    M.step = 1;
    M.g = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            M.m.data[i][j] = start[i][j];
            if (start[i][j] == 0){
                M.x = i;
                M.y = j;
            }
        }
    }
    M.h = h2(M);
    open_list.push(M);
    closed_list.insert(M.m);
    while(!open_list.empty()){
        auto C = open_list.top();
        open_list.pop();
        if(is_target(C, target_pos)){
            return C.step;
        }else {
            for (int i=0; i<4; i++) {
                int x = C.x + dx[i];
                int y = C.y + dy[i];
                if (is_reachable(x, y) && C.m.data[x][y] > 0){
                    swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                    if(closed_list.find(C.m) == closed_list.end()){
                        global_step ++;
                        closed_list.insert(C.m);
                        MatrixState new_state(C.m, global_step, x, y, C.g+1, h2(C));
                        open_list.push(new_state);
                        prev_step[global_step] = C.step;
                        prev_direct[global_step] = i;
                    }
                    swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                }
            }
        }
    }
    return finish_step;
}

int IDA_h1(const vector< vector<int> > &start, const vector<vector< int> > &target){
    int global_step = 1;
    int finish_step = 0;
    int depth_limit = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            M.m.data[i][j] = start[i][j];
            if (start[i][j] == 0){
                M.x = i;
                M.y = j;
            }
        }
    }
    depth_limit = h1(M);
    while(depth_limit != INT_MAX){
        std::priority_queue<MatrixState> open_list;
        std::set<Matrix> closed_list;
        int new_depth_limit = INT_MAX;
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                M.m.data[i][j] = start[i][j];
                if (start[i][j] == 0){
                    M.x = i;
                    M.y = j;
                }
            }
        }
        M.step = 1;
        M.g = 0;
        M.h = h1(M);
        open_list.push(M);
        closed_list.insert(M.m);
        while(!open_list.empty()){
            auto C = open_list.top();
            open_list.pop();
            if (is_target(C, target_pos)){
                return C.step;
            } else if (C.g + C.h > depth_limit){
                new_depth_limit = min(new_depth_limit, C.g + C.h);
            } else {
                for (int i=0; i<4; i++){
                    int x = C.x + dx[i];
                    int y = C.y + dy[i];
                    if (is_reachable(x, y) && C.m.data[x][y] > 0){
                        swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                        if(closed_list.find(C.m) == closed_list.end()){
                            global_step ++;
                            closed_list.insert(C.m);
                            MatrixState new_state(C.m, global_step, x, y, C.g+1, h1(C));
                            open_list.push(new_state);
                            prev_step[global_step] = C.step;
                            prev_direct[global_step] = i;
                        }
                        swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                    }
                }
            }
        }
        depth_limit = new_depth_limit;
    }
    return finish_step;
}

int IDA_h2(const vector< vector<int> > &start, const vector<vector< int> > &target){
    int global_step = 1;
    int finish_step = 0;
    int depth_limit = 0;
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            M.m.data[i][j] = start[i][j];
            if (start[i][j] == 0){
                M.x = i;
                M.y = j;
            }
        }
    }
    depth_limit = h2(M);
    while(depth_limit != INT_MAX){
        std::priority_queue<MatrixState> open_list;
        std::set<Matrix> closed_list;
        int new_depth_limit = INT_MAX;
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                M.m.data[i][j] = start[i][j];
                if (start[i][j] == 0){
                    M.x = i;
                    M.y = j;
                }
            }
        }
        M.step = 1;
        M.g = 0;
        M.h = h2(M);
        open_list.push(M);
        closed_list.insert(M.m);
        while(!open_list.empty()){
            auto C = open_list.top();
            open_list.pop();
            if (is_target(C, target_pos)){
                return C.step;
            } else if (C.g + C.h > depth_limit){
                new_depth_limit = min(new_depth_limit, C.g + C.h);
            } else {
                for (int i=0; i<4; i++){
                    int x = C.x + dx[i];
                    int y = C.y + dy[i];
                    if (is_reachable(x, y) && C.m.data[x][y] > 0){
                        swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                        if(closed_list.find(C.m) == closed_list.end()){
                            global_step ++;
                            closed_list.insert(C.m);
                            MatrixState new_state(C.m, global_step, x, y, C.g+1, h2(C));
                            open_list.push(new_state);
                            prev_step[global_step] = C.step;
                            prev_direct[global_step] = i;
                        }
                        swap(C.m.data[x][y], C.m.data[C.x][C.y]);
                    }
                }
            }
        }
        depth_limit = new_depth_limit;
    }
    return finish_step;
}