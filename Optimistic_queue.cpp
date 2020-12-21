#include <bits/stdc++.h>
#include <stdlib.h> 
#include <unistd.h>
#include <ctime> //for times
#include <chrono>    // for timers 
#include<thread>    //for threads
#include<random> //for exponential distribution

class pointer_t{
    public:
    node_t* ptr;
    int tag;
    pointer_t()
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

void fixList(queue_t *q,pointer_t tail,pointer_t head)
{
    pointer_t curNode, curNodeNext;
    curNode = tail;
    while((head == q->head)&&(curNode != head))
    {
        curNodeNext = (curNode.ptr)->next;
        (curNode.ptr)->prev.ptr = curNode.ptr; 
        (curNode.ptr)->prev.tag = curNode.tag - 1;
        curNode.ptr = curNodeNext.ptr;
        curNode.tag = curNode.tag - 1;
    }
}

class queue_t{
    public:
    pointer_t tail;
    pointer_t head;

    queue_t(queue_t *q)
    {
        node_t* nd = new node_t();
        
        nd->next.ptr = NULL;
        nd->next.tag = 0;
        q->tail.ptr = nd;
        q->tail.tag = 0;
        q->head.ptr = nd;
        q->head.tag = 0;
    }

    void enqueue(queue_t* q, int val)
    {
        pointer_t tail;
        node_t* nd = new node_t();
        nd->value = val;
        while(1)
        {
            tail = q->tail;
            nd->next.ptr = tail.ptr;
            nd->next.tag = tail.tag+1;
            if() //
            {
                (tail.ptr)->prev.ptr = nd;
                (tail.ptr)->prev.tag = tail.tag+1;
                break;
            }
        }
    }

    int dequeue(queue_t* q)
    {
        pointer_t tail,head,firstNodePrev;
        int val;
        while(1)
        {
            head = q->head;
            tail = q->tail;
            firstNodePrev = (head.ptr)->prev;
            if(head == q->head)
            {
                if(tail!=head)
                {
                    if(firstNodePrev.tag!=head.tag)
                    {
                        fixList(q,tail,head)
                        continue;
                    }
                    val = (firstNodePrev)->value;
                    if(CAS) //
                    {
                        delete head.ptr //????
                        return val;
                    }
                }
            }
            else
            {
                return NULL;
            }
        }
    }

};

