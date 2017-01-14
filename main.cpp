//  main.cpp
//  AIGame
//  Created by Aishwarya Verghese on 10/5/16.

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

int n;
string mode;
char player;
int d;
int infinity;

struct node
{
    vector<pair<string,int>> unoccupied; //to maintain order of pairs
    unordered_map<string,int> x_occupied;
    unordered_map<string,int> o_occupied;
    char player;
    int depth;
    string move;
};

node potential_next_state;
string potential_occupying_cell;

node getinput()
{
    //read from input file
    ifstream input;
    input.open("input.txt");
    
    node initial_state;
    initial_state.depth = 0;
    
    if (input.is_open())
    {
        input >> n;
        input >> mode;
        input >> player;
        input >> d;
        
        initial_state.player = player;
        infinity = 99*n*n;  //the maximum value a player can score if they occupy all the square and they all have value 99
        int *values[n];
        for (int i = 0;i < n; ++i)
            values[i] = new int[n];
        
        //Cell values
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                input >> values[i][j];
        
        //Board state
        char occupied;
        
        for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
            {
                input >> occupied;
                string row = to_string(i+1);
                char col = char(j+65);
                string id = col + row;
                
                if(occupied == 'X')
                    initial_state.x_occupied[id] = values[i][j];
                else if(occupied == 'O')
                    initial_state.o_occupied[id] = values[i][j];
                else
                    initial_state.unoccupied.push_back(make_pair(id,values[i][j]));
            }
    }
    return initial_state;
}

void checkState(node state)
{
    cout << "X occupied" << endl;
    // show content:
    for (unordered_map<string,int>::iterator it=state.x_occupied.begin(); it!=state.x_occupied.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    
    cout << "O occupied" << endl;
    for (unordered_map<string,int>::iterator it=state.o_occupied.begin(); it!=state.o_occupied.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    
    cout << "unoccupied" << endl;
    for (int i=0; i<state.unoccupied.size(); ++i)
        cout << state.unoccupied[i].first << " => " << state.unoccupied[i].second << '\n';
}

bool terminalTest(node state)
{
    if(state.unoccupied.size() == 0)    //Board is full
        return true;
    if(state.depth >= d)
        return true;
    return false;
}

int utility(node state)
{
    int score = 0, x_score = 0, o_score = 0;
    
    //Find X's score
    for (unordered_map<string,int>::iterator it=state.x_occupied.begin(); it!=state.x_occupied.end(); ++it)
        x_score += it -> second;
    
    //Find O's score
    for (unordered_map<string,int>::iterator it=state.o_occupied.begin(); it!=state.o_occupied.end(); ++it)
        o_score += it -> second;
    
    if(player == 'X')
        score = x_score - o_score;
    else
        score = o_score - x_score;
    
    return score;
}

string leftOf(int row,int col)
{
    //b is left of a (row = row; col = col - 1)
    if( col != 1 )
    {
        string b = char(col - 1 + 64) + to_string(row);
        return b;
    }
    return "-1";
}

string rightOf(int row,int col)
{
    //b is right of a (row = row; col = col + 1)
    if( col != n )
    {
        string b = char(col + 1 + 64) + to_string(row);
        return b;
    }
    return "-1";
}

string above(int row,int col)
{
    //b is above a (row = row - 1; col = col)
    if( row != 1 )
    {
        string b = char(col + 64) + to_string(row - 1);
        return b;
    }
    return "-1";
}
string below(int row,int col)
{
    //b is below a (row = row + 1; col = col)
    if( row != n )
    {
        string b = char(col + 64) + to_string(row + 1);
        return b;
    }
    return "-1";
}

bool canRaid(node state,string occupying_cell)
{
    //obtain occupying row and column from index 1
    int row = stoi(occupying_cell.substr(1));
    int col = int(occupying_cell[0]) - 64;
    
    //get players occupied cells
    unordered_map<string,int> player_occupied = state.o_occupied;
    if(state.player == 'X')
        player_occupied = state.x_occupied;
    
    string b;
    b = leftOf(row,col);
    if ( player_occupied.find(b) != player_occupied.end() )
        return true;
    
    b = rightOf(row,col );
    if ( player_occupied.find(b) != player_occupied.end() )
        return true;
    
    b = above(row, col);
    if ( player_occupied.find(b) != player_occupied.end() )
        return true;
    
    b = below(row, col);
    if ( player_occupied.find(b) != player_occupied.end() )
        return true;
    
    return false;
}

bool canConquer(node state,string occupying_cell)
{
    if(canRaid(state,occupying_cell))
    {
        //obtain occupying row and column from index 1
        int row = stoi(occupying_cell.substr(1));
        int col = int(occupying_cell[0]) - 64;
        
        //get players occupied cells
        unordered_map<string,int> enemy_cells = state.x_occupied;
        if(state.player == 'X')
            enemy_cells = state.o_occupied;
        
        string b;
        
        b = leftOf(row,col);
        if ( enemy_cells.find(b) != enemy_cells.end() )
            return true;
        
        b = rightOf(row,col);
        if ( enemy_cells.find(b) != enemy_cells.end() )
            return true;
        b = above(row,col);
        if ( enemy_cells.find(b) != enemy_cells.end() )
            return true;
        
        b = below(row,col);
        if ( enemy_cells.find(b) != enemy_cells.end() )
            return true;

    }
    return false;
}

node conquerIt(node state,string conquering_cell)
{
    cout << "Conquering Enemy!" << endl;
    if(state.player == 'X')
    {
        state.x_occupied[conquering_cell] = state.o_occupied[conquering_cell];
        state.o_occupied.erase(conquering_cell);
    }
    else
    {
        state.o_occupied[conquering_cell] = state.x_occupied[conquering_cell];
        state.x_occupied.erase(conquering_cell);
    }
    return state;
}

bool conquer(node &state,string occupying_cell)
{
    bool flag = false;
    //obtain occupying row and column from index 1
    int row = stoi(occupying_cell.substr(1));
    int col = int(occupying_cell[0]) - 64;
    
    //get players occupied cells
    unordered_map<string,int> enemy_cells = state.x_occupied;
    if(state.player == 'X')
        enemy_cells = state.o_occupied;
    
    string b;
    
    b = leftOf(row,col);
    if ( enemy_cells.find(b) != enemy_cells.end() )
    {
        state = conquerIt(state,b);
        flag = true;
    }
    
    b = rightOf(row,col);
    if ( enemy_cells.find(b) != enemy_cells.end() )
    {
        state = conquerIt(state,b);
        flag = true;
    }
    
    b = above(row,col);
    if ( enemy_cells.find(b) != enemy_cells.end() )
    {
        state = conquerIt(state,b);
        flag = true;
    }
    
    b = below(row,col);
    if ( enemy_cells.find(b) != enemy_cells.end() )
    {
        state = conquerIt(state,b);
        flag = true;
    }
    
    return flag;
}

node create_new_node(node state,string occupying_cell,int occupying_value)
{
    node new_node;
    string move = "Stake";
    if(canRaid(state,occupying_cell))
        if(conquer(state,occupying_cell))
            move = "Raid";
    
    cout<<"Move = "<<move<<endl;
    new_node.move = move;
    
    new_node.depth = state.depth+1;
    
    new_node.x_occupied = state.x_occupied;
    new_node.o_occupied = state.o_occupied;
    
    if(state.player == 'X')
    {
        new_node.player = 'O';
        new_node.x_occupied[occupying_cell] = occupying_value;
    }
    else
    {
        new_node.player = 'X';
        new_node.o_occupied[occupying_cell] = occupying_value;
    }
    
    //remove occupied cell from unoccupied list
    new_node.unoccupied = state.unoccupied;
    vector<pair<string,int>>::iterator position = find(new_node.unoccupied.begin(), new_node.unoccupied.end(), make_pair(occupying_cell,occupying_value));
    if (position != new_node.unoccupied.end())
        new_node.unoccupied.erase(position);
    return new_node;
}

int maximum(int a, int b)
{
    if(a>b)
        return a;
    return b;
}

int minimum(int a, int b)
{
    if(a<b)
        return a;
    return b;
}

int min(node state,int alpha,int beta);    //define min before defining max's body

int max(node state, int alpha, int beta)
{
    cout << "In max\n";
    //Check for terminal state
    if(terminalTest(state))
    {
        int u = utility(state);
        cout << "Terminal state encountered with utility "<<u<<endl;
        return u;
    }

    int v = -infinity;
    
    //iterate over unoccupied squares
    vector<pair<string,int>> raids;
    vector<pair<string,int>> actions;
    for(int i = 0;i < state.unoccupied.size();i++)
    {
        string cell = state.unoccupied[i].first;
        int value = state.unoccupied[i].second;
        if(canConquer(state,cell))
            raids.push_back(make_pair(cell,value));
        else
            actions.push_back(make_pair(cell,value));
    }
    
    actions.insert(actions.end(), raids.begin(), raids.end());
    
    //iterate over actions
    for(int i = 0;i < actions.size();i++)
    {
        //create new min nodes
        string occupying_cell = actions[i].first;
        int occupying_value = actions[i].second;
        
        cout << "Occupying cell "<<occupying_cell<<endl;
        node new_node = create_new_node(state,occupying_cell,occupying_value);
        
        int new_v = maximum(v,min(new_node,alpha,beta));
    
        //root node - keep track of best move
        if(state.depth == 0)
        {
            cout << "In root node";
            if(v != new_v)//change if we find a better utility
            {
                potential_next_state = new_node;
                potential_occupying_cell = occupying_cell;
            }
        }
        
        v = new_v;
        
        //alpa-beta pruning
        if( mode == "ALPHABETA" )
        {
            if(v >= beta)
            {
                return v;
            }
            alpha = maximum(alpha,v);
        }
    }

    cout << "Max v = "<<v<<endl;
    return v;
}

int min(node state, int alpha, int beta)
{
    cout << "In min\n";
    //Check for terminal state
    if(terminalTest(state))
    {
        int u = utility(state);
        cout << "Terminal state encountered with utility "<<u<<endl;
        return u;
    }
    
    int v = infinity;
    
    //iterate over unoccupied squares
    vector<pair<string,int>> raids;
    vector<pair<string,int>> actions;
    for(int i = 0;i < state.unoccupied.size();i++)
    {
        string cell = state.unoccupied[i].first;
        int value = state.unoccupied[i].second;
        if(canConquer(state,cell))
            raids.push_back(make_pair(cell,value));
        else
            actions.push_back(make_pair(cell,value));
    }
    
    actions.insert(actions.end(), raids.begin(), raids.end());
    
    //iterate over actions
    for(int i = 0;i < actions.size();i++)
    {
        //create new max nodes
        string occupying_cell = actions[i].first;
        int occupying_value = actions[i].second;
        
        cout << "Occupying cell "<<occupying_cell<<endl;
        node new_node = create_new_node(state,occupying_cell,occupying_value);
        
        v = minimum(v,max(new_node,alpha,beta));
        
        //alpa-beta pruning
        if( mode == "ALPHABETA" )
        {
            if(v <= alpha)
                return v;
            beta = minimum(beta,v);
        }
    }
    cout<<"Min v: "<<v<<endl;
    return v;

}

void minimax(node initial_state)
{
    int alpha = -infinity;
    int beta = infinity;
    int minimax_value = max(initial_state,alpha,beta);
    cout << minimax_value <<endl;
}

void printOutput()
{
    ofstream output;
    output.open("output.txt");
    
    if(output.is_open())
    {
        output << potential_occupying_cell << " ";
        output << potential_next_state.move << "\n";
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                string row = to_string(i+1);
                char col = char(j+65);
                string id = col + row;
                
                if(potential_next_state.x_occupied.find(id) != potential_next_state.x_occupied.end())
                    output << "X";
                else if(potential_next_state.o_occupied.find(id) != potential_next_state.o_occupied.end())
                    output << "O";
                else
                    output << ".";
            }
            output << "\n";
        }
    }
    else
    {
        cout << "Error opening output file \n";
        exit(0);
    }
    output.close();
}

int main()
{
    node initial_state = getinput();
    checkState(initial_state);
    minimax(initial_state);
    printOutput();
}
