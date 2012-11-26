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
 */

#ifndef KTCONCURRENTQUEUE_HH_
#define KTCONCURRENTQUEUE_HH_

#include "KTLogger.hh"

#include <boost/thread.hpp>

#include <deque>

namespace Katydid
{
    KTLOGGER(queuelog, "katydid.utility");

    template< class XDataType >
    class KTConcurrentQueue
    {

        public:
            KTConcurrentQueue() :
                fQueue(),
                fInterrupt(false),
                fMutex(),
                fConditionVar()
            {
            }

            virtual ~KTConcurrentQueue()
            {
                fQueue.clear();
            }

        private:
            std::deque< XDataType > fQueue;
            bool fInterrupt;

            mutable boost::mutex fMutex;
            boost::condition_variable fConditionVar;

        public:
            void push(XDataType const& data)
            {
                KTDEBUG(queuelog, "Attempting to push to queue");
                boost::mutex::scoped_lock lock(fMutex);
                KTDEBUG(queuelog, "Pushing to concurrent queue; size: " << fQueue.size());
                fQueue.push_back(data);
                lock.unlock();
                fConditionVar.notify_one();
                return;
            }

            bool empty() const
            {
                boost::mutex::scoped_lock lock(fMutex);
                return fQueue.empty();
            }

            bool size() const
            {
                boost::mutex::scoped_lock lock(fMutex);
                return fQueue.size();
            }

            bool try_pop(XDataType& popped_value)
            {
                boost::mutex::scoped_lock lock(fMutex);
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
                boost::mutex::scoped_lock lock(fMutex);
                fInterrupt = false;
                while(fQueue.empty())
                {
                    fConditionVar.wait(lock);
                    if (fInterrupt)
                    {
                        fInterrupt = false;
                        return false;
                    }
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

    };

} /* namespace Katydid */
#endif /* KTCONCURRENTQUEUE_HH_ */
