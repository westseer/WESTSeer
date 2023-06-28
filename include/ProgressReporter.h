#ifndef PROGRESSREPORTER_H
#define PROGRESSREPORTER_H

class ProgressReporter
{
    public:
        ProgressReporter();
        virtual ~ProgressReporter();
        virtual void report(const char *taskName, int taskId, int numTasks, int taskProgress) = 0;

    protected:


    private:
};

#endif // PROGRESSREPORTER_H
