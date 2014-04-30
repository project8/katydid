/*
 * KTConcurrentQueue.hh
 *
 *  Created on: Oct 22, 2012
 *      Author: nsoblath
 *
 *      FIFO Queue
 *
 *      Based almost exactly on the class concurrent_queue from:
 *      http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
 *      Post author: Anthony Williams
 *      Copyright  2005-2013 Just Software Solutions Ltd. All rights reserved.
 */

#ifndef KTCONCURRENTQUEUE_HH_
#define KTCONCURRENTQUEUE_HH_

#include "KTLogger.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <deque>

namespace Katydid
{
    KTLOGGER(queuelog, "KTConcurrentQueue");

    template< class XDataType >
    class KTConcurrentQueue
    {
        public:
            typedef std::deque< XDataType > Queue;

            struct QueueNotEmpty
            {
                Queue& fQueue;
                QueueNotEmpty(Queue& aQueue) :
                    fQueue(aQueue)
                {}
                bool operator()() const
                {
                    return ! fQueue.empty();
                }
            };

            typedef boost::unique_lock< boost::mutex > ScopedLock;

        public:
            KTConcurrentQueue() :
                fQueue(),
                fInterrupt(false),
                fTimeout(boost::posix_time::milliseconds(1000)),
                fMutex(),
                fConditionVar()
            {
            }

            virtual ~KTConcurrentQueue()
            {
                fQueue.clear();
            }

        private:
            Queue fQueue;
            bool fInterrupt;

            boost::posix_time::time_duration fTimeout; /// Timeout duration in milliseconds

            mutable boost::mutex fMutex;
            boost::condition_variable fConditionVar;

        public:
            void push(XDataType const& data)
            {
                KTDEBUG(queuelog, "Attempting to push to queue");
                ScopedLock lock(fMutex);
                KTDEBUG(queuelog, "Pushing to concurrent queue; size: " << fQueue.size());
                fQueue.push_back(data);
                lock.unlock();
                fConditionVar.notify_one();
                return;
            }

            bool empty() const
            {
                ScopedLock lock(fMutex);
                return fQueue.empty();
            }

            bool size() const
            {
                ScopedLock lock(fMutex);
                return fQueue.size();
            }

            bool try_pop(XDataType& popped_value)
            {
                ScopedLock lock(fMutex);
                fInterrupt = false;
                if(fQueue.empty())
                {
                    return false;
                }

                popped_value=fQueue.front();
                fQueue.pop_front();
                return true;
            }

            bool wait_and_pop(XDataType& popped_value)
            {
                ScopedLock lock(fMutex);
                fInterrupt = false;
                fConditionVar.wait(lock, QueueNotEmpty(fQueue));
                if (fInterrupt)
                {
                    fInterrupt = false;
                    return false;
                }

                popped_value=fQueue.front();
                fQueue.pop_front();
                KTDEBUG(queuelog, "Popping from concurrent queue; size: " << fQueue.size());
                return true;
            }

            bool timed_wait_and_pop(XDataType& popped_value)
            {
                ScopedLock lock(fMutex);
                fInterrupt = false;
                boost::system_time const waitUntil = boost::get_system_time() + fTimeout;
                if (! fConditionVar.timed_wait(lock, waitUntil, QueueNotEmpty(fQueue)))
                {
                    KTDEBUG(queuelog, "Queue wait has timed out");
                    return false;
                }
                if (fInterrupt)
                {
                    fInterrupt = false;
                    return false;
                }

                popped_value=fQueue.front();
                fQueue.pop_front();
                KTDEBUG(queuelog, "Popping from concurrent queue; size: " << fQueue.size());
                return true;
            }

            void interrupt()
            {
                fInterrupt = true;
                fConditionVar.notify_one();
                return;
            }

            inline unsigned get_timeout() const
            {
                return fTimeout.total_milliseconds();
            }

            inline void set_timeout(unsigned duration)
            {
                fTimeout = boost::posix_time::milliseconds(duration);
                return;
            }
    };

} /* namespace Katydid */
#endif /* KTCONCURRENTQUEUE_HH_ */
