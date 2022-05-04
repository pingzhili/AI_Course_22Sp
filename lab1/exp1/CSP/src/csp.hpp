#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;
#define MAX_NUM_WORKERS 10
#define NUM_CASES 2
#define DAY_PER_WEEK 7

enum Level {junior, senior};
typedef struct Assignment{
    bool is_assigned = false; // block or not
    bool value = false; // today work or not
} Assignment;
class Constraint {
public:
    int num_workers;
    int num_seniors;
    int least_daily_num_workers;
    int least_rest_num_days;
    int most_continuous_rest_days;
    int worker_levels[MAX_NUM_WORKERS];
    std::vector< std::pair<int, int> > conflicts;
    
    bool back_track(Assignment assigns[][DAY_PER_WEEK]);
    bool is_complete(const Assignment assigns[][DAY_PER_WEEK]);
    bool validate_with_forward_checking(Assignment assigns[][DAY_PER_WEEK]);
    void select_unassigned_variable(const Assignment assigns[][DAY_PER_WEEK], int &worker, int&day);
    void duplicate_assignments(Assignment source_assigns[][DAY_PER_WEEK], Assignment dest_assigns[][DAY_PER_WEEK]);
};
Constraint constraints[NUM_CASES];

void init();
void print_assignments(Assignment assigns[][DAY_PER_WEEK], int num_workers);