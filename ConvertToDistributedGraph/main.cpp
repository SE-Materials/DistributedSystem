//
//  main.cpp
//  ConvertToDistributedGraph
//
//  Created by Aniket Bhushan on 05/01/20.
//  Copyright © 2020 Aniket Bhushan. All rights reserved.
//

#include <iostream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>

// Define a graph here
struct Node
{
    std::vector<Node*> adj;
};

struct DNode; // fwd declare DNode

struct Message{
    int from;
    std::string message;
    
    Message(int from, std::string message) : from(from), message(message)
    {}
};

struct MessageQueue
{
    std::queue<Message> q;
    std::mutex mu;
    std::condition_variable cv;
};

struct NeighborData
{
    int color;
};

struct DNode
{
private:
    MessageQueue recievedQueue;
    std::thread* t;
    
public:
    std::unordered_map<int, NeighborData> neighbors;
    int nNeighbors;
    int color;
    int id;
    
    explicit DNode(int nNeighbors) : nNeighbors{nNeighbors}
    {
        t = new std::thread([](){});
    }
    
    void AddMessage(Message message)
    {
        std::unique_lock<std::mutex> lk(recievedQueue.mu);
        recievedQueue.cv.wait(lk, []{return true;});
        recievedQueue.q.push(message);
    }
    
    void ProcessRecievedMessages()
    {
        std::unique_lock<std::mutex> lk(recievedQueue.mu);
        recievedQueue.cv.wait(lk);
    }
};


class DistributedGraph
{
    Node* source;
    std::unordered_map<Node*, int> mp;
    std::unordered_map<int, DNode*> dmp;
    
public:
    explicit DistributedGraph(Node* source) : source(source)
    {
    }
    
    int GenerateId()
    {
        static int i = 0;
        return ++i;
    }
    
    int GraphToDistributed(Node* s)
    {
        DNode* ds = new DNode((int)s->adj.size());
        int id = GenerateId();
        
        mp.insert(std::make_pair(s, id));
        dmp.insert(std::make_pair(id, ds));
        
        for (auto& v : s->adj)
        {
            auto iter = mp.find(v);
            if (iter != mp.end())
            {
                ds->neighbors.insert(std::make_pair(GraphToDistributed(v), NeighborData()));
            }
            else
            {
                ds->neighbors.insert(std::make_pair(iter->second, NeighborData()));
            }
        }
        return id;
    }
    
    void SendMessage(int to, Message&& message)
    {
        auto it = dmp.find(to);
        if (it != dmp.end())
        {
            it->second->AddMessage(message);
        }
    }
    
    void WorkPackage(DNode* curr)
    {
        
        // Wait till all adj with higher ids have been colored
        while (std::any_of(curr->neighbors.begin(), curr->neighbors.end(),
                           [](std::pair<int, NeighborData> n){
                               return n.second.color == 0;
                           }))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Find least unused color
        
        curr->color = 1;
        
        for (auto v : curr->neighbors)
        {
            SendMessage(v.first, Message(curr->id, std::to_string(1)));
        }
    }
};


int main(int argc, const char * argv[]) {
    return 0;
}
