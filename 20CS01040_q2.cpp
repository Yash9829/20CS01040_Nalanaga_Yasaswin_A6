#include<bits/stdc++.h>
using namespace std;

class state{
    public :
        int state_num;
        vector<vector<string>> list; // this contains the list of all productions in the state with . marker
        map<string, int> adj; // this contains the adj list of the node
        vector<int> reduce;//to mark the node as reducing by a given rule to reduce, if there is R-R conflicts then also we manage by vector of int
        int accept;
    public : 
        state(){
            accept = 0;
        }
};

string removeSpaces(string str) {

    string temp = "";
    for(int i = 0; i < str.size(); i++) {

        if(str[i] != ' '){

            temp += str[i];

        }
    }
    return temp;
}

void processInput(string str, map<string, vector<string>> &grammar) {

    string k = removeSpaces(str);
    string token;
    istringstream iss(k);
    int count = 0;
    string the_key;
    while(getline(iss, token, '|')) {

        if(count == 0) {

            string t = "";
            t = t + token[0];

            the_key = t;

            t = "";
            for(int i = 3; i < token.size(); i++) {

                t += token[i];
            
            }
            
            grammar[the_key].push_back(t);

        } else {

            grammar[the_key].push_back(token);
        }

        count++;

    }
}

void form_dfa(map<string, vector<string>> grammar, vector<vector<string>> curr_initials, int &curr_no, unordered_map<string, int> non_terminals, map<vector<vector<string>>, int> &visited, map<int, state *> &num_to_ptr, map<vector<string>, int>rules){

    state * curr_node = new state();
    curr_node -> state_num = curr_no;
    num_to_ptr[curr_no] = curr_node;
    curr_no++;
    visited[curr_initials] = curr_no;

    //curr_initials are what we pass for that transition
    map<string, int> visited_for_curr_list;
    vector<string> chars_to_be_visited;
    for(auto itr : curr_initials){

        string left = itr[0];
        string right = itr[1];
        string temp = "";
        int i = 0;
        string req = "";
        while(curr_no != 1 && i < right.size()){

            if(right[i] == '.'){
                
                i++;

                if(i < right.size()){
                    
                    temp += right[i];
                    if(right[i] == '$'){
                        //make accept as 1 if in the curr list we got . at end after $
                        curr_node -> accept = 1;

                    }
                }

                if(i + 1 < right.size()){

                    req += right[i + 1];

                }
                temp += '.';

                
            } else {

                temp += right[i];

            }

            i++;
        }

        if(curr_no == 1){

            temp = right;
            temp = '.' + temp;
            req = right[0];
        }

        
        (curr_node -> list).push_back({left, temp});
        
        //check for . and call its productions and put dots in them
        // the char after dot is req
        if(visited_for_curr_list[req] == 0){
            visited_for_curr_list[req] = 1;
            chars_to_be_visited.push_back(req);
        }
        
    }

    //now that we visited the productions that are responsible for us to be here
    //lets worry about the productions where these produtions will yield us and put .s

    for(int i = 0; i < chars_to_be_visited.size(); i++){
        
        string x = chars_to_be_visited[i];
        if(non_terminals[x] == 1){

            // (curr_node -> list).push_back(x);
            for(auto g : grammar[x]){

                string temp = "";
                temp = g;

                string req = "";
                req += temp[0];

                if(visited_for_curr_list[req] == 0){
                    visited_for_curr_list[req] = 1;
                    chars_to_be_visited.push_back(req);
                }
                
                if(req != "ε")
                temp = '.' + temp;
                else
                temp = ".";

                (curr_node -> list).push_back({x, temp});
                // string c = "";
                // if(g.size() != 0){

                //     c += g[0];

                // }

                // if(c.size() != 0 && visited_for_curr_list[c] == 0){

                //     visited_for_curr_list[c] = 1;
                //     chars_to_be_visited.push_back(c);

                // }
            }

        }
    }

    // now that we have our list for the curr state, we have to do dfs for the states that have a term, nonterm
    // we have to group the things together, O(n ^ 2) time and a thing to track the visited and unvisited
    int n = (curr_node -> list).size();
    vector<int> already_grouped(n, 0);
    // the list has the list of all the productions with dots now we just have to call the groups and transitions
    //lets try to update the reductions in the class object
    for(int i = 0; i < n; i++){

        //for each string in the list find .
        string temp = "";
        for(int j = 0; j < (curr_node -> list)[i][1].size(); j++){

            if((curr_node -> list)[i][1][j] == '.' && j == (curr_node -> list)[i][1].size() - 1){

                (curr_node -> reduce).push_back(rules[{(curr_node -> list)[i][0], temp}]);

            }

            temp += (curr_node -> list)[i][1][j];

        }
    }

    

    for(int i = 0; i < n; i++){
        
        //find the dot and take the next to dot char
        vector<vector<string>> curr_transition;
        if(already_grouped[i] == 1)
        continue ;
        else
        already_grouped[i] = 1;

        string required_char = "";
        vector<string> starting_set;
        for(int k = 0; k < (curr_node -> list)[i][1].size(); k++){

            if((curr_node -> list)[i][1][k] == '.'){

                if(k + 1 != (curr_node -> list)[i][1].size()){

                    required_char += (curr_node -> list)[i][1][k + 1];

                }
            }
        }

        if(required_char.size() != 0){

            curr_transition.push_back((curr_node->list)[i]);
            for(int j = i + 1; j < n; j++){

                string curr_char = "";
                if(already_grouped[j] == 1){

                    continue ;
                
                } else {

                    for(int k = 0; k < (curr_node -> list)[j][1].size(); k++){

                        if((curr_node -> list)[j][1][k] == '.'){

                            if(k + 1 != (curr_node -> list)[j][1].size()){

                                curr_char += (curr_node -> list)[j][1][k + 1];

                            }
                        }
                    }

                    if(curr_char == required_char){

                        already_grouped[j] = 1;
                        curr_transition.push_back((curr_node -> list)[j]);
                    }

                }
            }

            if(visited[curr_transition] == 0){

                form_dfa(grammar, curr_transition, curr_no, non_terminals, visited, num_to_ptr, rules);
            
            }
            (curr_node -> adj)[required_char] = visited[curr_transition];

        }
    }

    
}

int main() {

    // LL(0) parser
    // state diagrams, adjacency lists structs, ptrs
    
    cout << "NOTE : all the terminals and non terminals should be single characters\n";

    int term;
    cout << "enter the no.of terminals\n";
    cin >> term;
    vector<string> list_of_terminals(term + 1);
    cout << "enter the list of all terminals\n";
    for(int i = 0; i < term; i++){

        cin >> list_of_terminals[i];

    }
    list_of_terminals[term] = "$";

    int non_term;
    cout << "enter the no.of non-terminals\n";
    cin >> non_term;
    vector<string> list_of_non_terminals(non_term);
    cout << "enter the list of all non terminals\n";
    for(int i = 0; i < non_term; i++){

        cin >> list_of_non_terminals[i];
        
    }

    map<vector<string>, int> rules;
    cout << "Enter the no.of productions\n";
    int n;
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    map<string, vector<string>> grammar;

    string start_symbol;
    cout << "Enter the production rules\n";
    for(int i = 0; i < n; i++){

        string input;
        // cin >> input;
        getline(cin, input);
        if(i == 0){

            string temp = "";
            for(int j = 0; j < input.size(); j++){

                if(input[j] != ' '){

                    temp += input[0];
                    break;

                }
            }
            start_symbol = temp;

        }
        processInput(input, grammar);

    }

    cout << "Grammar\n";
    unordered_map<string, int> non_terminals;
    int count = 1;
    for(auto itr : grammar){

        cout << itr.first << " -> ";
        non_terminals[itr.first]++;
        for(auto itr2 : itr.second){

            cout << itr2 << "|";
            rules[{itr.first, itr2}] = count;
            count++;

        } 
        cout << endl;

    }
    
    cout << "The rules are as follows\n";
    // as the key value in the map is a vector of strings then they are sorted according to that 
    // inorder to print the rules in indexing order then itr over grammar and print them
    for(auto x : grammar){

        string temp = x.first;
        for(int i = 0; i < x.second.size(); i++){

            cout << rules[{temp, x.second[i]}] << ")" << temp << "->" << x.second[i] << "\n";

        }

    }

    //LL(0) parser 
    state * starting_node = new state();
    // map<string, set<string>> starting_set;
    vector<vector<string>> starting_set;
    for(auto temp : grammar[start_symbol]){

        // string temp = grammar[start_symbol][0];
        starting_set.push_back({start_symbol, temp});

    }
    int curr_no = 0;
    map<vector<vector<string>>, int> visited;
    visited[starting_set] = curr_no + 1;

    //node number to ptr convertor is a map
    map<int, state *> num_to_ptr;
    form_dfa(grammar, starting_set, curr_no, non_terminals, visited, num_to_ptr, rules);

    // we have the dfa
    // have to create a parse table for this dfa

    //lets just print the adjacency list of all the nodes and their reduce actions if present
    cout << "The automaton for the LR(0) parser is as shown below\n";
    for(int i = 0; i < curr_no; i++){

        // num_to_ptr[i] this gives the node with the index as i
        cout << "\nFor state : " << i << "\n";
        for(auto x : num_to_ptr[i] -> adj){

            cout << x.first << " : " << x.second - 1<< endl;

        }

        cout << "List of all the productions in the node "<<i<<" is :\n";
        for(int j = 0; j < (num_to_ptr[i] -> list).size(); j++){

            cout << (num_to_ptr[i] -> list)[j][0] << "->" << (num_to_ptr[i] -> list)[j][1] << endl; 
        }

        cout << "Reduce actions are : ";
        for(auto x : num_to_ptr[i] -> reduce){

            cout << x << " ";

        }
        cout << endl;
    }

    //now checking that the above are correct I, Yasaswin, will now create a LR(0) parse table from the automaton
    // vector<vector<vector<int>>> LR_0_parse_table(curr_no, vector<vector<int>>(term + non_term));

    map<pair<int, string>, vector<int>> LR_0_parse_table;
    for(int i = 0; i < curr_no; i++){

        for(int j = 0; j <= term; j++){

            if((num_to_ptr[i] -> adj)[list_of_terminals[j]] != 0)
            LR_0_parse_table[{i, list_of_terminals[j]}].push_back((num_to_ptr[i] -> adj)[list_of_terminals[j]] - 1);

        }

        for(int j = 0; j < non_term; j++){

            if((num_to_ptr[i] -> adj)[list_of_non_terminals[j]] != 0)
            LR_0_parse_table[{i, list_of_non_terminals[j]}].push_back((num_to_ptr[i] -> adj)[list_of_non_terminals[j]] - 1);
        }

        for(auto x : num_to_ptr[i] -> reduce){

            int temp = (-1) * x;
            for(int j = 0; j <= term; j++)
            LR_0_parse_table[{i, list_of_terminals[j]}].push_back(temp);

        }

    }

    //printing the parse table
    cout << "The req parse table is as shown below(action part is till terminals and do to part is for non terminals) :\n";
    cout << "\t";
    for(int j = 0; j <= term; j++){

        cout << list_of_terminals[j] << "\t";
    }

    for(int j = 0; j < non_term; j++){

        cout << list_of_non_terminals[j] << "\t";
    }
    cout << endl;

    for(int i = 0; i < curr_no; i++){
        //action part
        cout << i << ":\t";
        for(int j = 0; j <= term; j++){

            for(int k = 0; k < LR_0_parse_table[{i, list_of_terminals[j]}].size(); k++){

                if(LR_0_parse_table[{i, list_of_terminals[j]}][k] >= 0)
                cout << "S" <<LR_0_parse_table[{i, list_of_terminals[j]}][k] << "|";
                else
                cout << "R" << abs(LR_0_parse_table[{i, list_of_terminals[j]}][k]) << "|";

                if(num_to_ptr[i]->accept == 1){
                    cout << "acc" << "|";
                }
            }
            cout << "\t";
        }
        //go to part
        for(int j = 0; j < non_term; j++){

            for(int k = 0; k < LR_0_parse_table[{i, list_of_non_terminals[j]}].size(); k++){

                cout << LR_0_parse_table[{i, list_of_non_terminals[j]}][k] << "|";
            }
            cout << "\t";
        }

        cout << endl;

    }

}