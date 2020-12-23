#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include <thread>    //for threads
#include <random> //for exponential distribution
#include <atomic>

using namespace std;
class pointer_t;
class node_t;
class queue_t;
int n,k;

void fixList(queue_t *q,pointer_t tail,pointer_t head);

class pointer_t{
    public:
    node_t* ptr;
    int tag;
    pointer_t() noexcept
    {
        ptr = NULL;
        tag = 0;
    }
};

class node_t{
    public:
    int value;
    pointer_t next;
    pointer_t prev;
    node_t()
    {
        value = 0;
    }
};

bool comparePointer(pointer_t a,pointer_t b)
{
    if((a.ptr != b.ptr)||(a.tag != b.tag))
    {
        return false;
    }
    return true;
}

class queue_t{
    public:
    atomic<pointer_t> tail;
    atomic<pointer_t> head;

    queue_t()
    {
        node_t* nd = new node_t();
        
        nd->next.ptr = NULL;
        nd->next.tag = 0;
        pointer_t temp;
        temp.ptr=nd;
        temp.tag=0;
        (tail).store(temp);
        (head).store(temp);
    }

    void enqueue(int val)
    {
        pointer_t check_tail;
        node_t* nd = new node_t();
        nd->value = val;
        while(1)
        {
            check_tail = (tail).load();
            nd->next.ptr = check_tail.ptr;
            nd->next.tag = check_tail.tag+1;
            pointer_t temp;
            temp.ptr = nd;
            temp.tag = check_tail.tag+1;
            if((tail).compare_exchange_strong(check_tail, temp)) //
            {
                (check_tail.ptr)->prev.ptr = nd;
                (check_tail.ptr)->prev.tag = check_tail.tag;
                break;
            }
        }
    }

    int dequeue()
    {
        pointer_t check_tail,check_head,firstNodePrev;
        int val;
        while(1)
        {
            check_head = (head).load();
            check_tail = (tail).load();
            firstNodePrev = (check_head.ptr)->prev;
            if(comparePointer(check_head, (head).load()))
            {
                if(!comparePointer(check_tail,check_head))
                {
                    if(firstNodePrev.tag!=check_head.tag)
                    {
                        fixList(this,check_tail,check_head);
                        continue;
                    }
                    val = (firstNodePrev.ptr)->value;
                    pointer_t temp;
                    temp.ptr = firstNodePrev.ptr;
                    temp.tag = check_head.tag+1;
                    if((head).compare_exchange_strong(check_head, temp)) //
                    {
                        delete check_head.ptr ;//????
                        return val;
                    }
                }
                else
                {
                    return INT_MIN;
                }
            }
        }
    }

};


void fixList(queue_t *q,pointer_t tail,pointer_t head)
{
    pointer_t curNode, curNodeNext;
    curNode = tail;
    while((comparePointer(head,(q->head.load())))&& (!comparePointer(curNode, head)))
    {
        curNodeNext = (curNode.ptr)->next;
        (curNode.ptr)->prev.ptr = curNode.ptr; 
        (curNode.ptr)->prev.tag = curNode.tag - 1;
        curNode.ptr = curNodeNext.ptr;
        curNode.tag = curNode.tag - 1;
    }
}

queue_t opt_q;

void test_random(int id){
	for(int i=0;i<k;i++){
		//srand(time(0));
		if(rand()&1){
            int enq = rand()%100;
            opt_q.enqueue(enq);
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
			int ret = opt_q.dequeue();
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
			opt_q.enqueue(enq);
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
			int ret = opt_q.dequeue();
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
    comment lines 222-234 and uncomment the below lines 242-253
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