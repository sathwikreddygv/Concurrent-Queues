#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution
#include <atomic>
using namespace std;
#define MAX_HOPS 3

default_random_engine generator;
float l1,l2;

class pointer_t;
class node_t;
class queue_t;

void free_chain(queue_t *q,pointer_t head,pointer_t new_head);

class pointer_t
{
public:
	node_t* ptr;
	bool deleted;
	int tag;

	pointer_t() noexcept
    {
        ptr = NULL;
        deleted = 0;
        tag = 0;
    }
	pointer_t(node_t* p, bool marked, int t){
		ptr = p;
		deleted = marked;
		tag = t;
	}
};

class node_t
{
public:
	int value;
	atomic<pointer_t> next;
};

bool comparePointer(pointer_t a,pointer_t b)
{
    if((a.ptr != b.ptr)||(a.tag != b.tag)||(a.deleted != b.deleted))
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
		node_t* nd = new node_t();      //Allocate a new node
		pointer_t useit(NULL,0,0);
		nd->next.store(useit);
		// nd->next.ptr = NULL;
		// nd->next.deleted = 0;
		// nd->next.tag = 0;

		pointer_t temp(nd,0,0);
		// temp.ptr = nd;
		// temp.deleted = 0;
		// temp.tag =  0;
		tail.store(temp);               //tail points to nd nd with tag 0
		head.store(temp);
	}

	void enqueue(queue_t* q, int val)
	{
		node_t* nd = new node_t();
        nd->value = val;
		pointer_t check_tail, check_next;
        while(1)
        {
        	check_tail = q->tail.load();  //full confusemnet. ok write
        	check_next = check_tail.ptr->next.load();
			if(comparePointer(check_tail,q->tail.load()))
			{
				if(check_next.ptr==NULL)
				{
					pointer_t useitagain(NULL,0,check_tail.tag+2);
					nd->next.store(useitagain);
					// nd->next.ptr=NULL;
					// nd->next.deleted =0;
					// nd->next.tag = check_tail.tag+2;
					pointer_t temp(nd,0,check_tail.tag+1);
					// temp.ptr=nd;
					// temp.deleted = 0;
					// temp.tag=check_tail.tag+1;
					if((q->tail.load().ptr->next).compare_exchange_strong(check_next, temp))
					{
						//CAS
						pointer_t temp2(nd,0,check_tail.tag+1);
						// pointer_t temp2;
						// temp2.ptr=nd;
						// temp2.deleted = 0;
						// temp2.tag=check_tail.tag+1;
						(q->tail).compare_exchange_strong(check_tail, temp2);
						return;
					}
					check_next = check_tail.ptr->next.load();
					while((check_next.tag==check_tail.tag+1)&&!(check_next.deleted))
					{
						//backoff
						exponential_distribution<double> distribution_one(l1);
					    auto t1 = distribution_one(generator);
					    sleep((int)(t1));
						// nd->next = check_next;
						nd->next.store(check_next);
						if((q->tail.load().ptr->next).compare_exchange_strong(check_next, temp))
						{
							return;
						}
						check_next = check_tail.ptr->next.load();
					}
				}
				else
				{
					while((check_next.ptr->next.load().ptr != NULL)&&comparePointer(q->tail.load(),check_tail))
					{
						check_next = check_next.ptr->next.load();
					}
					//CAS
					pointer_t temp1(check_next.ptr, 0 , check_tail.tag+1);
					// pointer_t temp1;
					// temp1.ptr=check_next.ptr;
					// temp1.deleted = 0;
					// temp1.tag=check_tail.tag+1;
					q->tail.compare_exchange_strong(check_tail, temp1);
				}
				
			}
        	
        }
	}

    int dequeue(queue_t* q,int i)
    {
        while(1)
        {
            pointer_t check_head = q->head.load();
            pointer_t check_tail = q->tail.load();
            pointer_t check_next = check_head.ptr->next.load();

            if(comparePointer(check_head, q->head.load()))
            {
                if(check_head.ptr == check_tail.ptr)
                {
                    if(check_next.ptr == NULL)
                        return -1;

                    while((check_next.ptr->next.load().ptr != NULL) && comparePointer(q->tail.load(),check_tail))
                    {
                        check_next = check_next.ptr->next.load();
                    }
                    //CAS
					pointer_t temp1(check_next.ptr,0,check_tail.tag+1);
					// temp1.ptr=check_next.ptr;
					// temp1.deleted = 0;
					// temp1.tag=check_tail.tag+1;
					(q->tail).compare_exchange_strong(check_tail, temp1);
                }
                else
                {
                    pointer_t iter = head;
                    int hops = 0;

                    while((check_next.deleted && iter.ptr != check_tail.ptr) && comparePointer(q->head.load(), check_head))
                    {
                        iter = check_next;
                        check_next = iter.ptr->next.load();
                        hops++;
                    }
                    if(!comparePointer(q->head.load(),check_head)) 
                        continue;
                    else if(iter.ptr == check_tail.ptr)
                        free_chain(q, check_head, iter);
                    else
                    {
                        int value = check_next.ptr->value;
                        pointer_t temp(check_next.ptr,1,check_next.tag+1);
						// pointer_t temp;
						// temp.ptr=check_next.ptr;
						// temp.deleted = 1;
						// temp.tag=check_next.tag+1;
						
                        if(iter.ptr->next.compare_exchange_strong(check_next,temp))//CAS
                        {
                            if(hops >= MAX_HOPS)
                                free_chain(q, check_head, check_next);
                            return value;
                        }
                       // backoff-scheme();
					   exponential_distribution<double> distribution_one(l2);
					   auto t1 = distribution_one(generator);
					   sleep((int)(t1));
                    }
                }
                
                
            }
        }
    }
};

void free_chain(queue_t *q,pointer_t check_head,pointer_t new_head)
{
	pointer_t temp(new_head.ptr, 0, check_head.tag+1);
	if(q->head.compare_exchange_strong(check_head,temp))
	{
		pointer_t check_next;
		while(check_head.ptr != new_head.ptr)
		{
			check_next = check_head.ptr->next;
			//reclaim
			delete check_head.ptr;
			check_head = check_next;
		}
	}
}

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
        int ret = opt_q.dequeue(&opt_q,i);
        auto end_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string t = ctime(&end_time);
        t = t.substr(10,9);
        cout<<"deqed "<<i+1<<" at "<<t<<endl;
    }
}

int main()
{
	l1=1.0/10;
	l2=1.0/10;
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