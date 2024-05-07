#include "graph.h"
#include <unistd.h>

Graph::Graph(string file_name){
    std::cout << "use graph constructor"; 
    vertex = new vector<int>();
    adj = new vector< list<int>* >();
    make_graph_from_file(file_name);
}

Graph::~Graph(){
    std::cout << "use graph distructor"; 
    delete vertex;
    for (size_t i = 0; i < adj->size(); ++i) {
        delete (*adj)[i]; 
    delete adj;
    }
}

void Graph::make_graph_from_file(string filename){
    cout << "start to built graph " << endl;
    char filepath[PATH_MAX];
    getcwd(filepath, sizeof(filepath));
    ifstream file (string(filepath) +"/"+ filename);
    string line; 
    if (file.is_open()) { 
        while (getline(file, line)) { 
            stringstream ss(line);
            string node_id;
            vector<string> nodes;
            while(ss >> node_id){
                nodes.push_back(node_id);
            }
            if (nodes.size() !=2){
                std::cerr << "line not as expected: " << line << endl;
            }
            int from = stoi(nodes[0]);
            int to = stoi(nodes[1]);
            if (from != to){
                add_vertex(from , to);
                add_vertex(to , from);
            }
        }            
        file.close(); 
    } 
    else { 
        cerr << "Unable to open file!" << endl; 
    } 
    cout << "finish to built graph " << endl;
}

bool Graph::is_vertex(int data){
    for (size_t i = 0; i < vertex->size(); ++i) {
        if ((*vertex)[i] == data) {
            return true;
        }
    }
    return false;
}

bool Graph::is_neighbor(int source_index, int neighbor){
    list <int> *source_neighbors = (*adj)[source_index];
    for (std::list<int>::iterator it = source_neighbors->begin(); it != source_neighbors->end(); ++it) {
        if(*it == neighbor){
            return true;
        }
    }     
    return false;
}

int Graph::find_index(int data){
    for(int index = 0 ; index < (*vertex).size(); index++){
        if((*vertex)[index] == data)
            return index;
    }
    return -1;
}

void Graph::add_vertex(int source , int target){
    bool is_vertex_exist = is_vertex(source);
    if(!is_vertex_exist){
        vertex->push_back(source);
        list<int>* toAdd = new list<int>();
        toAdd->push_back(target);
        adj->push_back(toAdd);
    }
    else {
        int source_adj_index = find_index(source);
        if (!is_neighbor(source_adj_index, target)) {
            (*adj)[source_adj_index]->push_back(target);
        }
    }
}


vector<int> Graph::BFS(int from, int to) {
    queue<int> q;
    vector<int> path;
    vector<bool> visit(vertex->size(), false);
    vector<int> parent(vertex->size(), -1);

    if (!is_vertex(from) || !is_vertex(to)) {
        std::cerr << "Invalid vertex, no path :(" << std::endl;
        return path;
    }

    q.push(from);
    visit[find_index(from)] = true;

    while (!q.empty()) {
        int current = q.front();
        int curr_idx = find_index(current);
        q.pop();

        list <int> *source_neighbors = (*adj)[curr_idx];
        std::list<int>::iterator it = source_neighbors->begin();
        for(; it != source_neighbors->end(); ++it){
            int neighbor = *it;
            int ne_idx = find_index(neighbor);
            if(!visit[ne_idx]){
                visit[ne_idx] = true;
                parent[ne_idx] = current;
                q.push(neighbor);
            }
        }
        // Check if we have reached the destination node
        if (visit[find_index(to)]) {
            // Construct the path from 'to' to 'from'
            int temp = to;
            while (temp != -1) {
                path.push_back(temp);
                temp = parent[find_index(temp)];
            }
            reverse(path.begin(), path.end()); // Reverse the path to get it from 'from' to 'to'
            return path;
        }
    }

    return path; // Return an empty path if no path is found
}