
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <set>
#include <random>

using namespace std;

//global variable
char last_colored_val = '0';

int calc_max_degree_heuristic(int n, vector<vector<int> >& a, int& node_max, int degree_node[])
{

    int prev_sum = 0;
    int current_sum = 0;
    int degree = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            current_sum = current_sum + a[i][j];
        }
        degree_node[i] = current_sum;
        if (current_sum > prev_sum) {
            node_max = i;
            degree = current_sum;
        }
        prev_sum = current_sum;
        current_sum = 0;
    }

    return degree;
}

void color_node(int node, vector<vector<char> >& domain_mat, pair<int, char> colored_graph[], int c, set<int>& complete)
{
    for (int i = 0; i < c; i++) {
        if (domain_mat[node][i] != '0') {
            colored_graph[complete.size()].first = node;
            colored_graph[complete.size()].second = domain_mat[node][i];
            last_colored_val = domain_mat[node][i];
            complete.insert(node);
            break;
        }
    }
}

void forward_checking(int node, vector<vector<char> >& domain_mat, vector<vector<int> >& adj_mat, int n, int c)
{
    //remove color from neighbours
    for (int i = 0; i < n; i++) {

        if (adj_mat[node][i] == 1) {

            for (int j = 0; j < c; j++) {
                if (domain_mat[i][j] == last_colored_val) {
                    domain_mat[i][j] = '0';
                    break;
                }
            }
        }
    }

    //set the domain  set of already colored node to 'Z'(to keep domain matrix constant)
    for (int j = 0; j < c; j++) {
        domain_mat[node][j] = 'Z';
    }
}

int find_mrv_node(set<int>& complete, vector<vector<char> >& domain_mat, int n, int c, int degree_heu[])
{

    vector<int> mrv_nodes;
    pair<int, int> legal_val[n];
    int prev_min = 5;
    int current = 0;
    int count = 0;
    int index = 0;

    //calculate number of legal values for each node
    for (int i = 0; i < n; i++) {

        if (complete.find(i) == complete.end()) {

            for (int j = 0; j < c; j++) {

                if (domain_mat[i][j] != '0') {

                    count++;
                }
            }
            legal_val[index] = make_pair(i, count);
            index++;
            count = 0;
        }
    }

    //Find miminum legal value

    for (int k = 0; k < index; k++) {

        current = legal_val[k].second;
        if (prev_min > current) {
            prev_min = current;
        }
    }

    //find nodes with minimim value

    for (int z = 0; z < index; z++) {

        if (legal_val[z].second == prev_min) {
            mrv_nodes.push_back(legal_val[z].first);
        }
    }

    //find highest degree heuristic value nodes with this min value and select any one of that node to color

    int prev_local = -1;
    int current_local = -1;
    int min_node = -1;
    for (int i = 0; i < mrv_nodes.size(); i++) {

        current_local = degree_heu[mrv_nodes[i]];
        if (current_local > prev_local) {
            prev_local = current_local;
            min_node = mrv_nodes[i];
        }
    }

    return min_node;
}

int check_solution_correctness(int n, pair<int, char> colored_graph[], vector<vector<int> >& adj_mat, int flag_print)
{

    //check for solution accuracy
    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            if (adj_mat[colored_graph[i].first][colored_graph[j].first] == 1 && colored_graph[i].second != colored_graph[j].second) {
                if (flag_print == 1) {
                    cout << "Node : " << colored_graph[i].first << " adjacent to node " << colored_graph[j].first << "   are of colors "
                         << colored_graph[i].second << " and " << colored_graph[j].second << " respectively are different. " << endl;
                }
            }
            else if (adj_mat[colored_graph[i].first][colored_graph[j].first] == 1 && colored_graph[i].second == colored_graph[j].second) {
                if (flag_print == 1) {
                    //cout<<"Node : "<<colored_graph[i].first<<" adjacent to node " << colored_graph[j].first <<"   are of colors"\
					// <<colored_graph[i].second<<" and "<<colored_graph[j].second<<"  are same "<<endl;
                    //cout<<"COLORING FAILED"<<endl;
                }
                return 1;
            }
        }
    }

    return 0;
}

//Make intial random coloring for nodes
void init_random_color_assignment(int n, int c, pair<int, char> min_conflict_colored_graph[], char min_color_array[])
{

    //To generate random generation uninform as a substitute for rand()
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0, c);

    int rand_choice;

    for (int i = 0; i < n; i++) {
        rand_choice = dist(mt);
        min_conflict_colored_graph[i] = make_pair(i, min_color_array[rand_choice]);
    }
}

//Calculate conlfict of node
int calculate_conflict(int node, int n, pair<int, char> min_conflict_colored_graph[], vector<vector<int> >& adj_mat)
{

    int conflict = 0;

    for (int i = 0; i < n; i++) {

        if (adj_mat[node][i] == 1 && min_conflict_colored_graph[node].second == min_conflict_colored_graph[i].second) {
            conflict++;
        }
    }

    return conflict;
}

//Pick a random node to reduce conflicts

int pick_random_node(int n, pair<int, char> min_conflict_colored_graph[], vector<vector<int> >& adj_mat)
{

    int rand_node_chosen = -1;
    int chosen_conflict = n;

    for (;;) {
        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(0, n);
        rand_node_chosen = dist(mt);

        chosen_conflict = calculate_conflict(rand_node_chosen, n, min_conflict_colored_graph, adj_mat);
        if (chosen_conflict != 0) {
            break;
        }
    }

    return rand_node_chosen;
}

void minimise_confilct_node(int node, int n, int c, pair<int, char> min_conflict_colored_graph[], char min_color_array[], vector<vector<int> >& adj_mat)
{
    int current_con = 0;
    int prev_con = n + 1;
    pair<int, char> temp[n];
    //copy original pairs into temp
    for (int i = 0; i < n; i++) {
        temp[i].first = min_conflict_colored_graph[i].first;
        temp[i].second = min_conflict_colored_graph[i].second;
    }

    for (int i = 0; i < c; i++) {

        temp[node].second = min_color_array[i];
        current_con = calculate_conflict(node, n, temp, adj_mat);
        //cout<<node<<"---"<<current_con<<endl;

        if (current_con == 0) {
            min_conflict_colored_graph[node].second = min_color_array[i];
            break;
        }

        if (prev_con > current_con) {
            prev_con = current_con;
            min_conflict_colored_graph[node].second = min_color_array[i];
        }
    }
}

void print(int n, pair<int, char> colored_graph[])
{

    for (int i = 0; i < n; i++) {

        cout << colored_graph[i].first << "," << colored_graph[i].second << endl;
    }
}

int main(int argc, char* argv[])
{

    string num;
    string file_name;
    string color;
    int n;
    int c;
	
	// IF USER FLAG IS NOT 1 , GET INTPUTS FROM USER , ELSE USE DEFUALT FILE AND VALUE TO RUN AN EXAMPLE
    if (argc == 1) {
        cout << "Please enter number nodes in the map:\n>";
        getline(cin, num);
        cout << "Please enter filename containing node mapping:\n>";
        getline(cin, file_name);

        do {
            cout << "Please enter number of colours to use <should be less that or equal to 4>:\n>";
            getline(cin, color);
            c = atoi(color.c_str());
        } while (c > 4);

        c = atoi(color.c_str());

        n = atoi(num.c_str());
    }
    else {
        file_name = "state.txt";
        c = 4;
        n = 50;
    }

    vector<vector<int> > adj_mat(n, vector<int>(n));

    //Intialise adjacency matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            adj_mat[i][j] = 0;
        }
    }

    string line;
    ifstream myfile(file_name.c_str());
    int x, y;
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            int pos = line.find(',');
            x = stoi(line.substr(0, pos));
            y = stoi(line.substr(pos + 1, pos + 3));
            adj_mat[x][y] = 1;
            adj_mat[y][x] = 1;
        }
        myfile.close();
    }
    else {
        cerr << "Unable to open file" << endl;
        exit(1);
    }

    //----------------------------I-------------------IMPLEMENTING USING MRV WITH FORWARD CHECKING ----------------------------------------------

    // DOMAIN COLORS
    vector<vector<char> > domain_mat(n, vector<char>(c));

    //Intialise color matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < c; j++) {
            if (j == 0) {
                domain_mat[i][j] = 'R';
            }
            else if (j == 1) {
                domain_mat[i][j] = 'G';
            }
            else if (j == 2) {
                domain_mat[i][j] = 'B';
            }
            else {
                domain_mat[i][j] = 'Y';
            }
        }
    }

    //Calculate DEGREE Heuristic
    int node_max = 0;
    int degree_heu[n];
    int degree_heuristic_val = calc_max_degree_heuristic(n, adj_mat, node_max, degree_heu);

    //Output Pairs

    pair<int, char> colored_graph[n];
    set<int> complete;

    //INITIALIZE coloring with degree heuristic
    color_node(node_max, domain_mat, colored_graph, c, complete);
    forward_checking(node_max, domain_mat, adj_mat, n, c);
    int next_node = -1;
    while (complete.size() != n) {
        next_node = find_mrv_node(complete, domain_mat, n, c, degree_heu);
        color_node(next_node, domain_mat, colored_graph, c, complete);
        forward_checking(next_node, domain_mat, adj_mat, n, c);
    }
    cout << endl;
    cout << endl;
    cout << "CORRECT SOLUTION FOR MAP COLORING USING MRV AND FORWARD CHECKING" << endl;
    cout << "---------------------------------------------------------------------" << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "NODES ARE COLORED AS BELOW" << endl;
    cout << "--------------------------" << endl;
    cout << endl;
    cout << endl;
    print(n, colored_graph);
    cout << endl;
    cout << endl;

    //Check solution correnctness for MRV algorithm
    check_solution_correctness(n, colored_graph, adj_mat, 1);

    //---------------------------------------------------------MIN CONFLICT IMPLEMENTATION------------------------------------------------------------------------------

    pair<int, char> min_conflict_colored_graph[n];

    char min_color_array[4] = { 'R', 'G', 'B', 'Y' };
     
	int got_solution = -1;
	int num_of_restarts=0;
	
	while(got_solution !=0){
   
    init_random_color_assignment(n, c, min_conflict_colored_graph, min_color_array);
    
    got_solution = check_solution_correctness(n, min_conflict_colored_graph, adj_mat, 0);

    int rand_node = -1;
    
    int MAX_TRY = n * 100;

    if (got_solution == 0) {
        cout << endl;
        cout << endl;
        cout << "CORRECT SOLUTION FOR MAP COLORING USING MIN CONFLICT" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << endl;
        cout << endl;
        cout << "NODES ARE COLORED AS BELOW" << endl;
        cout << "--------------------------" << endl;
        cout << endl;
        print(n, min_conflict_colored_graph);
        cout << endl;
        cout << endl;
        check_solution_correctness(n, min_conflict_colored_graph, adj_mat, 1);
		
    }
    else {

        do {
            rand_node = pick_random_node(n, min_conflict_colored_graph, adj_mat);
            minimise_confilct_node(rand_node, n, c, min_conflict_colored_graph, min_color_array, adj_mat);
            got_solution = check_solution_correctness(n, min_conflict_colored_graph, adj_mat, 0);
            MAX_TRY--;
        } while (got_solution == 1 && MAX_TRY > 0);
    }

    if (got_solution == 0) {
        cout << endl;
        cout << endl;
        cout << "CORRECT SOLUTION FOR MAP COLORING USING MIN CONFLICT" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << endl;
        cout << endl;
        cout << endl;
        check_solution_correctness(n, min_conflict_colored_graph, adj_mat, 1);
        cout << endl;
        cout << "NODES ARE COLORED AS BELOW" << endl;
        cout << "--------------------------" << endl;
        cout << endl;
        print(n, min_conflict_colored_graph);
        cout << endl;
        cout << endl;
    }
    /*else {
        cout << endl;
        cout << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "SOLUTION FOR THIS MAP CANNOT BE FOUND USING -- MIN CONFLICT ALGORITHM-- USING THIS ATTEMPT . TRY AGAIN !!.(INCREASE COLOR IF POSSIBLE)" << endl;
        cout << "--------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << endl;
        cout << endl;
    }*/		
		num_of_restarts++;

}
	cout<<"---NUMBER OF RESTART REQUIRED FOR THE SOLUTION--USING MIN CONFLICT : "<<num_of_restarts<<endl;
	
    //CREATE GV FILE TO PLOT GRAPHICAL OUTPUT FOR --MRV WITH FORWARD CHECKING--

    ofstream outfile;
    outfile.open("MRV.gv");
    outfile << "graph G {" << endl;
    outfile << "label = \"\\n\\n\\n\\n MRV WITH FORWARD CHECKING \\n\\n\"" << endl;
    string line1;
    ifstream myfile1(file_name.c_str());
    int x1, y1;
    if (myfile1.is_open()) {
        while (getline(myfile1, line1)) {
            int pos1 = line1.find(',');
            x1 = stoi(line1.substr(0, pos1));
            y1 = stoi(line1.substr(pos1 + 1, pos1 + 3));
            outfile << "\"" << x1 << "\""
                    << " -- "
                    << "\"" << y1 << "\"" << endl;
        }
        myfile1.close();
    }
    else {
        cerr << "Unable to open file" << endl;
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        if (colored_graph[i].second == 'R') {
            outfile << "\"" << colored_graph[i].first << "\""
                    << "  "
                    << "[shape=circle, style=filled, fillcolor=red]" << endl;
        }
        else if (colored_graph[i].second == 'G') {
            outfile << "\"" << colored_graph[i].first << "\""
                    << "  "
                    << "[shape=circle, style=filled, fillcolor=green]" << endl;
        }
        else if (colored_graph[i].second == 'B') {
            outfile << "\"" << colored_graph[i].first << "\""
                    << "  "
                    << "[shape=circle, style=filled, fillcolor=blue]" << endl;
        }
        else if (colored_graph[i].second == 'Y') {
            outfile << "\"" << colored_graph[i].first << "\""
                    << "  "
                    << "[shape=circle, style=filled, fillcolor=yellow]" << endl;
        }
    }

    outfile << "   }" << endl;

    outfile.close();

    //CREATE GV FILE TO PLOT GRAPHICAL OUTPUT FOR --MIN CONFLICT--
    if (got_solution == 0) {
        ofstream outfile1;
        outfile1.open("Min-conflict.gv");
        outfile1 << "graph G {" << endl;
        outfile1 << "label = \"\\n\\n\\n\\n MIN CONFLICT  \\n\\n\"" << endl;
        string line2;
        ifstream myfile2(file_name.c_str());
        int x2, y2;
        if (myfile2.is_open()) {
            while (getline(myfile2, line2)) {
                int pos2 = line2.find(',');
                x2 = stoi(line2.substr(0, pos2));
                y2 = stoi(line2.substr(pos2 + 1, pos2 + 3));
                outfile1 << "\"" << x2 << "\""
                         << " -- "
                         << "\"" << y2 << "\"" << endl;
            }
            myfile2.close();
        }
        else {
            cerr << "Unable to open file" << endl;
            exit(1);
        }

        for (int i = 0; i < n; i++) {
            if (min_conflict_colored_graph[i].second == 'R') {
                outfile1 << "\"" << min_conflict_colored_graph[i].first << "\""
                         << "  "
                         << "[shape=circle, style=filled, fillcolor=red]" << endl;
            }
            else if (min_conflict_colored_graph[i].second == 'G') {
                outfile1 << "\"" << min_conflict_colored_graph[i].first << "\""
                         << "  "
                         << "[shape=circle, style=filled, fillcolor=green]" << endl;
            }
            else if (min_conflict_colored_graph[i].second == 'B') {
                outfile1 << "\"" << min_conflict_colored_graph[i].first << "\""
                         << "  "
                         << "[shape=circle, style=filled, fillcolor=blue]" << endl;
            }
            else if (min_conflict_colored_graph[i].second == 'Y') {
                outfile1 << "\"" << min_conflict_colored_graph[i].first << "\""
                         << "  "
                         << "[shape=circle, style=filled, fillcolor=yellow]" << endl;
            }
        }

        outfile1 << "   }" << endl;

        outfile1.close();
    }

    //SYSTEM COMMANDS FOR GRAPH GENERATION
    system("dot -Tps MRV.gv -o MRV.ps");
    system("evince  MRV.ps");
    if (got_solution == 0) {
        system("dot -Tps Min-conflict.gv -o Min-conflict.ps");
        system("evince  Min-conflict.ps");
    }

    return 0;
}

