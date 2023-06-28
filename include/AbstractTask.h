#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H
#include <cstdlib>
#include <thread>
#include <atomic>
#include <ProgressReporter.h>

class AbstractTask
{
    public:
        AbstractTask();
        virtual ~AbstractTask();
        virtual bool finished() = 0;
        virtual const char *name() = 0;
        virtual int numSteps() = 0;
        virtual void doStep(int stepId) = 0;
        void runAll();
        static void cancel();
        static void setProgressReporter(ProgressReporter *value);
        static void finalize();
        inline void setPrev(AbstractTask *value)
        {
            _prev = value;
        }
        inline void setNext(AbstractTask *value)
        {
            _next = value;
        }
        inline int getNumTasks()
        {
            int result = 1;
            AbstractTask *prev = _prev;
            while (prev != NULL)
            {
                ++result;
                prev = prev->_prev;
            }
            AbstractTask *next = _next;
            while (next != NULL)
            {
                ++result;
                next = next->_next;
            }
            return result;
        }
        inline int getTaskId()
        {
            int result = 0;
            AbstractTask *prev = _prev;
            while (prev != NULL)
            {
                ++result;
                prev = prev->_prev;
            }
            return result;
        }

    protected:
        static std::atomic<bool> _cancelled;

    private:
        AbstractTask *_prev;
        AbstractTask *_next;
        static std::thread *_taskThread;
        static ProgressReporter *_progressReporter;
};

#endif // ABSTRACTTASK_H
