#include <iostream>
#include <vector>
#include <set>
#include <queue>
using namespace std;

// Enum class representing event type
enum class Event
{
    infection,
    recovery,
    susceptible // susceptible => Still not infected or no event
};

// Node of graph
struct node_graph_t
{
    int nodeId;
    int timeStamp;
    Event event_type;
};
typedef struct node_graph_t node_graph;

// Comparator class used to order the elements of priority queue in ascending order of timestamp
class Comparator
{
public:
    int operator()(node_graph p1, node_graph p2)
    {
        return p1.timeStamp > p2.timeStamp;
    }
};

// Creating edge of graph using adjacency list
void createEdge(vector<node_graph *> adj_list[], node_graph *u, node_graph *v)
{
    adj_list[u->nodeId].push_back(v);
    adj_list[v->nodeId].push_back(u);
}

// Printing adjacency list representing graph
void printGraph(vector<node_graph *> adj_list[], int V)
{
    for (int i = 0; i < V; i++)
    {
        cout << i;
        for (auto v : adj_list[i])
            cout << "-> " << v->nodeId;
        printf("\n");
    }
}

// Tossing coin where 0 represents head and 1 represents tail
int tossCoin()
{
    return rand() % 2;
}

// Using BFS to find the shortest distance between two nodes
int distance_find(int rootId, int destId, vector<node_graph *> list[], int V)
{
    if (rootId == destId)
        return 0;

    // labels which represent shortest distance
    int label[V];
    for (int i = 0; i < V; i++)
    {
        label[i] = 1000;
    }

    queue<int> arr;
    arr.push(rootId);
    label[rootId] = 0;

    vector<node_graph *> ptr = list[rootId];
    while (!arr.empty())
    {
        for (int i = 0; i < ptr.size(); i++)
        {
            if (label[ptr[i]->nodeId] == 1000)
            {
                label[ptr[i]->nodeId] = label[arr.front()] + 1;
                if (ptr[i]->nodeId == destId)
                    return label[ptr[i]->nodeId];
                else
                {
                    arr.push(ptr[i]->nodeId);
                }
            }
        }
        arr.pop();
        ptr = list[arr.front()];
    }

    // Node absent case
    cout << "Node with nodeId: " << destId << " Not Found\n";
    return -1;
}

// Creating a node
node_graph *CreateNode(int id, int t, Event e)
{
    node_graph *new_node = new node_graph{id, t, e};
    return new_node;
}

int main()
{
    int V = 100;
    vector<node_graph *> adj_list[V];

    // Pointers to each node in graph
    node_graph *vertices[V];
    for (int i = 0; i < V; i++)
    {
        vertices[i] = CreateNode(i, -1, Event::susceptible);
    }

    // Tossing coins to generate edges between vertices of graph
    for (int i = 0; i < V; i++)
    {
        srand(time(0));
        for (int j = i + 1; j < V; j++)
        {
            int x = tossCoin();
            if (x == 0)
                createEdge(adj_list, vertices[i], vertices[j]);
        }
    }

    // Sets representing S - Susceptible, I - Infected, R - Recovered
    //  T - Timestamps of every event except Event::susceptible(None or no event)
    set<int> S, I, R, T;

    // Set consisting of pairs
    // t - timestamp
    set<pair<int, int>> infected_people_with_t, recovered_people_with_t;

    // Priority queue to implement Min-heap
    priority_queue<node_graph, vector<node_graph>, Comparator> Q;

    // Initially all nodes are susceptible
    for (int i = 0; i < V; i++)
    {
        S.insert(vertices[i]->nodeId);
    }

    // Choosing arbitray node with infection event and timestamp 0
    // srand(time(0));
    int temp = rand() % V;
    node_graph *start = vertices[temp];
    start->event_type = Event::infection;
    start->timeStamp = 0;
    T.insert(0);
    infected_people_with_t.insert({start->timeStamp, start->nodeId});
    Q.push(*start);

    // This loop runs until priority queue becomes empty
    while (!Q.empty())
    {
        node_graph r = Q.top();
        node_graph *ele = &r;

        // Recovery event
        if (ele->event_type == Event::recovery)
        {
            R.insert(ele->nodeId);
            I.erase(ele->nodeId);
        }
        else if (ele->event_type == Event::infection) // Infection event
        {
            I.insert(ele->nodeId);
            S.erase(ele->nodeId);

            // Finding Susceptible neighbours of current infected node
            vector<node_graph *> x = adj_list[ele->nodeId];
            for (int i = 0; i < x.size(); i++)
            {
                if (x[i]->timeStamp == -1) // Timestamp = -1 => Node is susceptible
                {
                    // Tossing coin five times to know whether the susceptible neighbour was infected due to current infected node
                    // srand(time(0));
                    int j;
                    for (j = 1; j < 6; j++)
                    {
                        int p = tossCoin();
                        if (p == 0) // p= 0 => Head on tossing coin
                        {
                            // Generating infection event of susceptible neighbour
                            x[i]->timeStamp = ele->timeStamp + j;
                            x[i]->event_type = Event::infection;

                            node_graph s = {x[i]->nodeId, x[i]->timeStamp, x[i]->event_type};
                            Q.push(s);

                            infected_people_with_t.insert({s.timeStamp, s.nodeId});
                            T.insert(s.timeStamp);
                            break;
                        }
                    }

                    // Head not occurred on tossing coin 5 times
                    // Check for next neighbour
                    if (j == 6)
                        continue;
                    else
                    {
                        // Generaing recovery event of neighbours infected due to current infected node
                        // srand(time(0));
                        int k = (rand() % 5) + ele->timeStamp + j + 1;
                        x[i]->timeStamp = k;
                        x[i]->event_type = Event::recovery;

                        node_graph s = {x[i]->nodeId, x[i]->timeStamp, x[i]->event_type};
                        Q.push(s);

                        recovered_people_with_t.insert({s.timeStamp, s.nodeId});
                        T.insert(s.timeStamp);
                    }
                }
            }
        }
        // Pop function removes the element with smallest timestamp
        // This is because we are implementing min-Heap
        Q.pop();
    }

    // Generating recovery event of starting node
    // srand(time(0));
    int k = (rand() % 5) + start->timeStamp + 1;
    start->timeStamp = k;
    start->event_type = Event::recovery;
    T.insert(k);
    recovered_people_with_t.insert({start->timeStamp, start->nodeId});

    cout << "Plotting the number of susceptible, infected and recovered individuals, against i \n";
    cout << "(no_of_susceptible, no_of_infected, no_of_recovered, timeStamp)\n";
    for (auto l = T.begin(); l != T.end(); l++)
    {
        int i = *l;
        int count_sus, count_inf = 0, count_rec = 0;

        // Counting no. of infected nodes at timestamp i
        for (auto itr = infected_people_with_t.begin(); itr != infected_people_with_t.end(); itr++)
        {
            int temp = (*itr).second;
            node_graph *m = vertices[temp];
            pair<int, int> rip = {m->timeStamp, m->nodeId};
            if ((*itr).first <= i && i < (*recovered_people_with_t.find(rip)).first)
                count_inf++;
            else if (i < (*itr).first)
                break;
        }

        // Counting no. of recovered nodes at timestamp i
        for (auto itr = recovered_people_with_t.begin(); itr != recovered_people_with_t.end(); itr++)
        {
            if ((*itr).first <= i)
                count_rec++;
            else if (i < (*itr).first)
                break;
        }

        // Counting no. of susceptible nodes at timestamp i
        count_sus = V - count_inf - count_rec;
        cout << count_sus << ", " << count_inf << ", " << count_rec << ", " << i << endl;
    }

    cout << "\nComparing the instant at which a node gets infected with it's shortest distance from the start node s with nodeId = " << start->nodeId << endl;
    cout << "(nodeId_current_node, shortest_dist, infection_timestamp)\n";
    for (auto itr = infected_people_with_t.begin(); itr != infected_people_with_t.end(); itr++)
    {
        cout << (*itr).second << ", " << distance_find(start->nodeId, (*itr).second, adj_list, V) << ", " << (*itr).first << endl;
    }
    return 0;
}