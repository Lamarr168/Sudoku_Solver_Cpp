/* Get more familiar with C++ by writing a Sudoku puzzle solver.
 */

#include <iostream>
#include <fstream>
using namespace std;

const int  state_size = 9; // Diagonal length of puzzle. I bet that this value has to be the square of an integer for valid puzzles. 
const bool debug = false;
struct cellType {
    int row;
    int col;
};

void output_state(int state[state_size][state_size]);
bool try_new_number(int number, int row, int col, int state[state_size][state_size]);
cellType find_next_cell(int row, int col, int state[state_size][state_size]);
char search_for_solution(int row, int col, int state[state_size][state_size]);

cellType next_cell;
char result;
int search_nodes_visited = 0;

int main(int argc, char *argv[]) {
    bool temp_result;
    int i,j;
    int start_state [state_size][state_size];
    int start_state_testing[state_size][state_size] = {
                                0,9,4,8,6,0,5,0,2,
                                0,0,0,4,0,2,0,0,9,
                                0,2,6,0,0,0,0,0,4,
                                0,4,8,0,0,9,0,1,0,
                                0,0,3,0,1,0,0,2,0,
                                0,1,0,0,8,6,0,9,3,
                                0,0,5,2,9,0,3,0,0,
                                0,6,0,1,0,0,2,5,0,
                                2,0,1,0,0,8,0,4,7
                               };
    if (argc!=2){
        cout <<"Reading Sudoku puzzle from default.\nTo read puzzle from a file, supply file name as a command line arg.\n\n";
                copy(&start_state_testing[0][0], &start_state_testing[0][0]+state_size*state_size, &start_state[0][0]);
    }
    else{
        cout << "Reading Sudoku puzzle from file\n";
        ifstream in(argv[1], ios::in);
        if(!in){
            cout << "Cannot open file.\n";
            return 16;
        }
        for(i=0;i<state_size && !in.eof();i++){
            for(j=0;j<state_size && !in.eof();j++){
                in >> start_state[i][j];
            }
        }
        
        if((i<state_size)||(j<state_size)){
            cout << "Input file was too small. The numbers in the file have to be space-delimited with zero\n";
            cout << "representing open cells. The file is expected to contain " << state_size << " * " << state_size;
            cout << " = " << state_size*state_size << " number of numbers.\n";
            return 16;
        }
        in.close();
    }
    
    cout << "starting state: \n";
    output_state(start_state);
    cout << "------------------";
    
    result = search_for_solution(0, 0, start_state); 
    if (result == 'R'){
        cout << "\nSolution not found\n";
        return 4;
    }
    cout << "\nNumber of search nodes visited: " << search_nodes_visited << "\n";
    return 0;
}

/* Main process that recurses to find a solution
 * The return value 'P' signals that the solution has been found and we are popping the stack to return from recursion.
 * The return value 'R' (for "retry") means we have hit a dead end and need to backup and try other options for cell values,
 */
char search_for_solution(int row, int col, int state[state_size][state_size]){
    cellType next_cell;
    int new_val;
    char returned_value;
    
    search_nodes_visited++;
    next_cell = find_next_cell(row, col, state);
    if (next_cell.row == -1){
        cout << "\nSolution found:\n";
        output_state(state);
        return 'P';  // Solution found and output - return from recursion by popping the stack
    }
    for(new_val = state_size; new_val > 0; new_val--){
        if(try_new_number(new_val, next_cell.row, next_cell.col, state)){
            state[next_cell.row][next_cell.col] = new_val;
            if (debug) {output_state(state); cout << "------------------" <<next_cell.row <<","<<next_cell.col<<"^^^\n";}
            returned_value = search_for_solution(next_cell.row, next_cell.col, state);
            if (returned_value == 'P'){
                return 'P';
            }
        }
    }
    // If we are here then nothing has worked and we need to backup in the recursion tree.
    // We have to set the cell back to zero because C++ does call-by-value when passing around
    // arrays and so we only have one array as state in the program processing.
    state[next_cell.row][next_cell.col] = 0;
    return 'R';
}

//Output the state of the puzzle
void output_state(int state[state_size][state_size]){
    int row, col;

    for(row=0; row<state_size; row++){
        for(col=0; col<state_size; col++)
            cout << state[row][col] << " ";
        cout << "\n";
    }    
}
/*See if placing number in cell doesn't clash (=create duplicate) with existing state by checking...
 * 1) row;
 * 2) column;
 * 3) sector - 3x3 groups for 9x9 puzzle. 
 */
bool try_new_number(int number, int row, int col, int state[state_size][state_size]){
    int i,j,row_start,row_end,col_start,col_end;
    
    // Check column for duplication
    for(i=0; i<state_size; i++){
        if(number==state[i][col]){
            return false;
        }
    }
    // Check row for duplication
    for(j=0; j<state_size; j++){
        if(number==state[row][j]){
            //cout << "...row dup found \n";
            return false;
        }
    }
    //Check sector
    row_start = (row/3)*3;
    row_end   = row_start + 2;
    col_start = (col/3)*3;
    col_end   = col_start + 2;
    
    for(i=row_start;i<=row_end;i++)
        for(j=col_start;j<=col_end;j++)
            if(number==state[i][j]){
                return false;
            }
    return true;
}

// Find next open cell on the state.
cellType find_next_cell(int row, int col, int state[state_size][state_size]){
    cellType next_cell;
    int i, j, end;
    end = state_size - 1;
    
    i = row;
    j = col;
    while (true){
        if (state[i][j] == 0){
                next_cell.row = i;
                next_cell.col = j;
                return next_cell;
        }
        if (++j > end){
            ++i;
            j=0;
        }
        if (i > end){ // No more open cells found
            next_cell.row = -1;
            next_cell.col = -1;
            return next_cell;
        }
    }
}