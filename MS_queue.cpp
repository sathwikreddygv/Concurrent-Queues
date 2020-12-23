#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution
#include <atomic>
using namespace std;

class pointer_t;
class node_t;


class pointer_t
{
    public:
    node_t *ptr;
    int count;
    pointer_t() noexcept
    {
        ptr = NULL;
        count = 0;
    }
    pointer_t(node_t *p, int c){
        count = c;
        ptr = p;
    }
}
;

class node_t{
    public:
    int value;
    atomic<pointer_t> next;

};

bool comparePointer(pointer_t a,pointer_t b)
{
    if((a.ptr != b.ptr)||(a.count != b.count))
    {
        return false;
    }
    return true;
}

class queue_t
{
    public:
    atomic<pointer_t> tail;
    atomic<pointer_t> head;
    queue_t()
    {
        node_t *node =  new node_t();
        pointer_t temp0(NULL,0);
       // node->next.load().ptr = NULL;
        node->next.store(temp0);
        pointer_t temp(node,0);
        head.store(temp);
        tail.store(temp);
    }
    
    void enqueue(queue_t* q, int val)
    {
        node_t *node =  new node_t();
        node->value = val;
       // node->next.load().ptr = NULL;
        pointer_t temp0(NULL,0);
       // node->next.load().ptr = NULL;
        node->next.store(temp0);
        pointer_t check_tail, check_next;
        while(1){
            check_tail = q->tail.load();
            check_next = check_tail.ptr->next.load();
            if(comparePointer(check_tail, q->tail.load())){
                if(check_next.ptr == NULL){
                    pointer_t temp(node,check_next.count+1);
                    if(check_tail.ptr->next.compare_exchange_strong(check_next, temp)){
                        break;
                    }
                }
                else{
                    pointer_t temp(check_next.ptr,check_tail.count+1);
                    q->tail.compare_exchange_strong(check_tail, temp);
                }
            }
        }
        pointer_t temp(node,check_tail.count+1);
        q->tail.compare_exchange_strong(check_tail,temp );
    }
    
    int dequeue(queue_t* q)
    {
        pointer_t check_tail, check_head,check_next;
        int value;
        while(1)
        {
            check_head = q->head.load();
            check_tail = q->tail.load();
            check_next = check_head.ptr->next.load();
            if(comparePointer(check_head, q->head.load())){
                if(check_head.ptr == check_tail.ptr){
                    if(check_next.ptr == NULL){
                        return -1;
                    }
                    pointer_t temp(check_next.ptr,check_tail.count+1);
                    q->tail.compare_exchange_strong(check_tail, temp);
                }
                else{
                    value = check_next.ptr->value;
                    pointer_t temp(check_next.ptr,check_head.count+1);
                    if(q->head.compare_exchange_strong(check_head,temp))
                        break;
                }
            }
            
            
        }
        free(check_head.ptr);
        return value;
    }
};

queue_t opt_q;
void test(int i){
    if(i<7){
        opt_q.enqueue(&opt_q, i+1);
        auto end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string t = ctime(&end_time);
        t = t.substr(10,9);
        cout<<"enqed "<<i+1<<" at "<<t<<endl;
    }
    else{
        int ret = opt_q.dequeue(&opt_q);
        auto end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string t = ctime(&end_time);
        t = t.substr(10,9);
        cout<<"deqed "<<i+1<<" at "<<t<<endl;
    }
}



int main(){

    // queue_t q = new 
    int n = 10;
    thread test_threads[n];
    for(int i=0;i<n;i++)
    {
        test_threads[i] = thread(test,i);    
    }
    
    for(int i=0;i<n;i++)
    {
        test_threads[i].join();
    }
    return 0;
}