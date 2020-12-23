#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution
#include <atomic>
using namespace std;

int n,k;

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
        node->next.store(temp0);
        pointer_t temp(node,0);
        head.store(temp);
        tail.store(temp);
    }
    
    void enqueue(int val)
    {
        node_t *node =  new node_t();
        node->value = val;
        pointer_t temp0(NULL,0);
        node->next.store(temp0);
        pointer_t check_tail, check_next;
        while(1){
            check_tail = tail.load();
            check_next = check_tail.ptr->next.load();
            if(comparePointer(check_tail, tail.load())){
                if(check_next.ptr == NULL){
                    pointer_t temp(node,check_next.count+1);
                    if(check_tail.ptr->next.compare_exchange_strong(check_next, temp)){
                        break;
                    }
                }
                else{
                    pointer_t temp(check_next.ptr,check_tail.count+1);
                    tail.compare_exchange_strong(check_tail, temp);
                }
            }
        }
        pointer_t temp(node,check_tail.count+1);
        tail.compare_exchange_strong(check_tail,temp );
    }
    
    int dequeue()
    {
        pointer_t check_tail, check_head,check_next;
        int value;
        while(1)
        {
            check_head = head.load();
            check_tail = tail.load();
            check_next = check_head.ptr->next.load();
            if(comparePointer(check_head, head.load())){
                if(check_head.ptr == check_tail.ptr){
                    if(check_next.ptr == NULL){
                        return -1;
                    }
                    pointer_t temp(check_next.ptr,check_tail.count+1);
                    tail.compare_exchange_strong(check_tail, temp);
                }
                else{
                    value = check_next.ptr->value;
                    pointer_t temp(check_next.ptr,check_head.count+1);
                    if(head.compare_exchange_strong(check_head,temp))
                        break;
                }
            }
            
        }
        delete check_head.ptr;
        return value;
    }
};

queue_t ms_q;
void test_random(int id){
	for(int i=0;i<k;i++){
		//srand(time(0));
		if(rand()&1){
            int enq = rand()%100;
            ms_q.enqueue(enq);
            printf("thread %d enqueued %d\n",id+1,enq );
            /*****************************************************
             comment above line and uncomment below lines
             to get time stamps. This could give outofrange error
             which we could not rectify, but it does not have anything
             to do with the working of the queue
            ******************************************************/
            // time_t end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
		    // string t = ctime(&end_time);
		    // t = t.substr(10,9);
            // printf("thread %d enqueued %d at %s\n",id+1,enq,t.c_str() );
		}
		else{
			int ret = ms_q.dequeue();
            printf("thread %d dequeued %d\n", id+1, ret);
            /*****************************************************
             comment above line and uncomment below lines
             to get time stamps. This could give outofrange error
             which we could not rectify, but it does not have anything
             to do with the working of the queue
            ******************************************************/
            // time_t end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
            // string t = ctime(&end_time);
            // t = t.substr(10,9);
            // printf("thread %d dequeued %d at %s\n", id+1, ret, t.c_str());
		}
	}
}

void test_alternate(int id){
	for(int i=0;i<k;i++){
		if(i&1){
            int enq = rand()%100;
			ms_q.enqueue(enq);
            printf("thread %d enqueued %d\n",id+1,enq );
            /*****************************************************
             comment above line and uncomment below lines
             to get time stamps. This could give outofrange error
             which we could not rectify, but it does not have anything
             to do with the working of the queue
            ******************************************************/
            // time_t end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
		    // string t = ctime(&end_time);
		    // t = t.substr(10,9);
            // printf("thread %d enqueued %d at %s\n",id+1,enq,t.c_str() );

		}
		else{
			int ret = ms_q.dequeue();
            printf("thread %d dequeued %d\n", id+1, ret);
            /*****************************************************
             comment above line and uncomment below lines
             to get time stamps. This could give outofrange error
             which we could not rectify, but it does not have anything
             to do with the working of the queue
            ******************************************************/
            // time_t end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
            // string t = ctime(&end_time);
            // t = t.substr(10,9);
            // printf("thread %d dequeued %d at %s\n", id+1, ret, t.c_str());
		}
	}
}


int main(){

    cout<<"Enter number of threads (n) : "<<endl;
    cin>>n;
    cout<<"Enter number of operations for each thread (k): "<<endl;
    cin>>k;
    thread test_threads[n];
    auto EnterTime = chrono::steady_clock::now();
    for(int i=0;i<n;i++)
    {
        test_threads[i] = thread(test_random,i);    
    }
	auto ExitTime = chrono::steady_clock::now();
	auto total_time = chrono::duration_cast<chrono::nanoseconds>(ExitTime-EnterTime);
    auto time =(total_time.count());

    for(int i=0;i<n;i++)
    {
        test_threads[i].join();
    }


    /*********************************************************
    comment lines 200-212 and uncomment the below lines 220-231
    to test the average time per operation of test_random
    **********************************************************/
    
    // auto EnterTime = chrono::steady_clock::now();
    // for(int i=0;i<n;i++)
    // {
    //     test_threads[i] = thread(test_alternate,i);    
    // }
	// auto ExitTime = chrono::steady_clock::now();
	// auto total_time = chrono::duration_cast<chrono::nanoseconds>(ExitTime-EnterTime);
    // auto time =(total_time.count());
    // for(int i=0;i<n;i++)
    // {
    //     test_threads[i].join();
    // }
    
    cout<<"Total time taken : "<<time/1000<<" microseconds"<<endl;
    time/=(n*k);
    cout<<"Average time taken for each operation : "<<time<<" nanoseconds"<<endl;

    return 0;
}