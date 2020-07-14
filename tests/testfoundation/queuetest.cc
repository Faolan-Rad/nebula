//------------------------------------------------------------------------------
//  queuetest.cc
//  (C) 2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "queuetest.h"
#include "util/queue.h"
#include "util/arrayqueue.h"

namespace Test
{
__ImplementClass(Test::QueueTest, 'DQET', Test::TestCase);

using namespace Util;

bool Compare(Util::Queue<int> const & q, Util::ArrayQueue<int> const & a)
{
    bool same = true;
    if (a.Size() != q.Size())
    {
        return false;
    }
    for (int i = 0; i < q.Size(); i++)
    {
        same &= a[i] == q[i];
    }
    return same;
}

//------------------------------------------------------------------------------
/**
*/
void
QueueTest::Run()
{
    Queue<int> queue0;
    VERIFY(queue0.Size() == 0);
    VERIFY(queue0.IsEmpty());

    queue0.Enqueue(1);
    queue0.Enqueue(2);
    queue0.Enqueue(3);
    VERIFY(queue0.Size() == 3);
    VERIFY(!queue0.IsEmpty());
    VERIFY(queue0.Peek() == 1);
    VERIFY(queue0.Peek() == 1);
    VERIFY(queue0.Contains(3));
    VERIFY(!queue0.Contains(4));
    VERIFY(queue0[0] == 1);
    VERIFY(queue0[1] == 2);
    VERIFY(queue0[2] == 3);    

    Queue<int> queue1 = queue0;
    VERIFY(queue0 == queue1);
    VERIFY(!(queue0 != queue1));
    VERIFY(queue1.Dequeue() == 1);
    VERIFY(queue1.Size() == 2);
    VERIFY(queue1[0] == 2);
    VERIFY(queue1[1] == 3);
    VERIFY(queue1 != queue0);
    VERIFY(queue1.Dequeue() == 2);
    VERIFY(queue1.Dequeue() == 3);
    VERIFY(queue1.Size() == 0);
    VERIFY(queue1.IsEmpty());

    queue0.EraseIndex(1);
    VERIFY(queue0.Size() == 2);
    VERIFY(queue0[0] == 1);
    VERIFY(queue0[1] == 3);    

    queue0.Clear();
    VERIFY(queue0.Size() == 0);
    VERIFY(queue0.IsEmpty());

    SizeT capacity = 2* queue0.Capacity();
    for (IndexT i = 0; i < capacity; i++)
    {
        queue0.Enqueue(i);
    }
    VERIFY(queue0.Size() == capacity);
    bool same = true;
    for (IndexT i = 0; i < capacity; i++)
    {
        same &= queue0.Dequeue() == i;
    }
    VERIFY(same);

    capacity = queue0.Capacity() >> 1;

    for (IndexT i = 0; i < capacity; i++)
    {
        queue0.Enqueue(i);
        queue0.Dequeue();
    }
    IndexT limit = 2 * capacity - 1;
    for (IndexT i = 0; i < limit; i++)
    {
        queue0.Enqueue(i);
    }
    same = true;
    for (IndexT i = 0; i < limit; i++)
    {
        same &= queue0[i] == i;
    }
    VERIFY(same);

    queue0.Grow();
    same = true;
    for (IndexT i = 0; i < limit; i++)
    {
        same &= queue0[i] == i;
    }
    VERIFY(same);

    queue0.Clear();
    queue1.Clear();

    Queue<int> q0;
    Queue<int> q1;
    ArrayQueue<int> aq;
    IndexT count = 16;
    IndexT half = count >> 1;
    for (IndexT i = 0; i < half + (half>>1); i++)
    {
        q0.Enqueue(i);
        aq.Enqueue(i);
    }
    
    for (IndexT i = 0; i < half; i++)
    {
        q0.Enqueue(i);
        q0.Dequeue();
        aq.Enqueue(i);
        aq.Dequeue();
    }
    q1 = q0;

    // q0 i having a wraparound, q1 is linear in memory from index 0
    VERIFY(q0 == q1);
    VERIFY(Compare(q0, aq));
    q0.EraseIndex(2);
    q1.EraseIndex(2);
    aq.EraseIndex(2);
    VERIFY(q0 == q1);    
    q0.EraseIndex(q0.Size() - 2);
    q1.EraseIndex(q1.Size() - 2);
    aq.EraseIndex(aq.Size() - 2);
    VERIFY(q0 == q1);
    VERIFY(Compare(q0, aq));    
    q0.EraseIndex(q0.Size() - 1);
    q1.EraseIndex(q1.Size() - 1);
    aq.EraseIndex(aq.Size() - 1);
    VERIFY(q0 == q1);
    VERIFY(Compare(q0, aq));
}

}; // namespace Test
