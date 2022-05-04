#pragma GCC optimize(3)
#include "csp.hpp"

int main(){
    init();

    auto start = clock();
    auto assigns_first = new Assignment[constraints[0].num_workers][DAY_PER_WEEK];
    constraints[0].back_track(assigns_first);
    auto end = clock();
    cout << "Case 1 Time: " << (double)(end - start)/CLOCKS_PER_SEC << endl;
    print_assignments(assigns_first, constraints[0].num_workers);

    start = clock();
    auto assigns_second = new Assignment[constraints[1].num_workers][DAY_PER_WEEK];
    constraints[1].back_track(assigns_second);
    end = clock();
    cout << "Case 2 Time: " << (double)(end - start)/CLOCKS_PER_SEC << endl;
    print_assignments(assigns_second, constraints[1].num_workers);

    return 0;
}


bool Constraint::back_track(Assignment assigns[][DAY_PER_WEEK]){
    if(is_complete(assigns) && validate_with_forward_checking(assigns)){
        return true;
    } else {
        int var_worker, var_day;
        select_unassigned_variable(assigns, var_worker, var_day);
        auto dup_assigns = new Assignment[num_workers][DAY_PER_WEEK];
        duplicate_assignments(assigns, dup_assigns);
        /* assign not to work */
        assigns[var_worker][var_day].is_assigned = true;
        assigns[var_worker][var_day].value = false;
        if(validate_with_forward_checking(assigns) && back_track(assigns)){
            return true;
        } else {
            duplicate_assignments(dup_assigns, assigns);
        }
        /* assign to work */
        assigns[var_worker][var_day].is_assigned = true;
        assigns[var_worker][var_day].value = true;
        if(validate_with_forward_checking(assigns) && back_track(assigns)){
            return true;
        } else {
            return false;
        }
    }
}

bool Constraint::validate_with_forward_checking(Assignment assigns[][DAY_PER_WEEK]) {
    for (int j=0; j<DAY_PER_WEEK; j++){
        /* least daily num workers */
        int work_num_workers = num_workers;
        for (int i=0; i<num_workers; i++)
            if(assigns[i][j].is_assigned && !assigns[i][j].value)
                work_num_workers --;
        if (work_num_workers < least_daily_num_workers){
            return false;
        }

        /* at least one senior worker every day */
        int num_rest_seniors = 0;
        for (int i=0; i<num_workers; i++){
            if(worker_levels[i] == senior){
                if(assigns[i][j].is_assigned && !assigns[i][j].value)
                    num_rest_seniors ++;
            }
        }
        if (num_rest_seniors == num_seniors){
            return false;
        }

        /* handle worker conflicts */
        for (auto &conf:conflicts){
            int worker_first = conf.first;
            int worker_second = conf.second;
            if (assigns[worker_first][j].is_assigned && assigns[worker_first][j].value && assigns[worker_second][j].is_assigned && assigns[worker_second][j].value){
                // they both work today
                return false;
            } else if (!assigns[worker_first][j].is_assigned && assigns[worker_second][j].is_assigned && assigns[worker_second][j].value) {
                assigns[worker_first][j].is_assigned = true;
                assigns[worker_first][j].value = false;
                return validate_with_forward_checking(assigns);
            } else if (!assigns[worker_second][j].is_assigned && assigns[worker_first][j].is_assigned && assigns[worker_first][j].value) {
                assigns[worker_second][j].is_assigned = true;
                assigns[worker_second][j].value = false;
                return validate_with_forward_checking(assigns);
            }
        }
    }
    for (int i=0; i<num_workers; i++){
        /* least rest num days */
        int work_num_days = 0;
        for (int j=0; j<DAY_PER_WEEK; j++)
            if (assigns[i][j].is_assigned && assigns[i][j].value) 
                work_num_days ++;
        if ((DAY_PER_WEEK - work_num_days) < least_rest_num_days){
            return false;
        }

        /* most continuous rest days */
        int rest_num_days = 0;
        for (int j=0; j<DAY_PER_WEEK; j++){
            if (assigns[i][j].is_assigned && assigns[i][j].value){ 
                rest_num_days = 0;
            } else if (assigns[i][j].is_assigned && !assigns[i][j].value) { 
                rest_num_days ++;
                if (rest_num_days > most_continuous_rest_days)
                    return false;
            } else if (!assigns[i][j].is_assigned) {
                if (rest_num_days == most_continuous_rest_days ){ 
                    assigns[i][j].is_assigned = true;
                    assigns[i][j].value = false;
                    return validate_with_forward_checking(assigns);
                }
                rest_num_days = 0;
            }
        }
    }
    return true;
}

bool Constraint::is_complete(const Assignment assigns[][DAY_PER_WEEK]){
    /* every assign is available */
    for (int i=0; i<num_workers; i++){
        for (int j=0; j<DAY_PER_WEEK; j++){
            if (!assigns[i][j].is_assigned){
                return false;
            }
        }
    }
    return true;
}

void Constraint::select_unassigned_variable(const Assignment assigns[][DAY_PER_WEEK], int &worker, int&day){
    for (int i=0; i<num_workers; i++){
        for (int j=0; j<DAY_PER_WEEK; j++){
            if (!assigns[i][j].is_assigned){
                worker = i;
                day = j;
                return;
            }
        }
    }
}

void Constraint::duplicate_assignments(Assignment source_assigns[][DAY_PER_WEEK], Assignment dest_assigns[][DAY_PER_WEEK]){
    for (int i=0; i<num_workers; i++){
        for (int j=0; j<DAY_PER_WEEK; j++){
            dest_assigns[i][j].is_assigned = source_assigns[i][j].is_assigned;
            dest_assigns[i][j].value = source_assigns[i][j].value;
        }
    }
}

void print_assignments(Assignment assigns[][DAY_PER_WEEK], int num_workers){
    for (int j=0; j<DAY_PER_WEEK; j++){
        for (int i=0; i<num_workers; i++){
            if (assigns[i][j].value)
                cout << i+1 << " ";
        }
        cout << endl;
    }
}

void init(){
    /* first test case */
    auto constr = &constraints[0];
    constr->num_workers = 7;
    constr->num_seniors = 2;
    constr->least_daily_num_workers = 4;
    constr->least_rest_num_days = 2;
    constr->most_continuous_rest_days = 2;
    constr->worker_levels[0] = senior;
    constr->worker_levels[1] = senior;
    constr->worker_levels[2] = junior;
    constr->worker_levels[3] = junior;
    constr->worker_levels[4] = junior;
    constr->worker_levels[5] = junior;
    constr->worker_levels[6] = junior;
    constr->conflicts.push_back(std::make_pair(1-1, 4-1));
    constr->conflicts.push_back(std::make_pair(2-1, 3-1));
    constr->conflicts.push_back(std::make_pair(3-1, 6-1));


    /* second test case */ 
    constr = &constraints[1];
    constr->num_workers = 10;
    constr->num_seniors = 4;
    constr->least_daily_num_workers = 5;
    constr->least_rest_num_days = 2;
    constr->most_continuous_rest_days = 2;
    constr->worker_levels[0] = senior;
    constr->worker_levels[1] = senior;
    constr->worker_levels[2] = junior;
    constr->worker_levels[3] = junior;
    constr->worker_levels[4] = junior;
    constr->worker_levels[5] = junior;
    constr->worker_levels[6] = junior;
    constr->worker_levels[7] = senior;
    constr->worker_levels[8] = junior;
    constr->worker_levels[9] = senior;
    constr->conflicts.push_back(std::make_pair(1-1, 5-1));
    constr->conflicts.push_back(std::make_pair(2-1, 6-1));
    constr->conflicts.push_back(std::make_pair(8-1, 10-1));
}

