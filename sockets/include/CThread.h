/*
 * CThread.h
 *
 *  Created on: Jan 29, 2019
 *      Author: vxuser
 */

#ifndef SRC_CTHREAD_H_
#define SRC_CTHREAD_H_

#include <future>
#include <functional>
#include <mutex>

class CThread
{
public:
    CThread(std::function<void (double)> function, double freq);

    virtual ~CThread();

    inline unsigned int GetIndexThread() { return index; }

protected:
    std::function<void (double)> function;

    double freq;
    uint64_t delay;

    void execute();

    std::chrono::steady_clock::time_point time_point;

    unsigned int index;

    bool threadIsNotFinish = true;

private:
    std::future<void> thread;

    CThread() = delete;
    CThread(CThread &) = delete;
};

#endif /* SRC_CTHREAD_H_ */
