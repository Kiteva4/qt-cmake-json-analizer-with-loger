/*
 * CThread.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: vxuser
 */

#include "CThread.h"
#include <iostream>

using namespace std::chrono;

CThread::CThread(std::function<void (double)> function, double freq) :
    function(function), freq(freq),
    delay(static_cast<uint64_t>(1000000000L / freq)),
    time_point(steady_clock::now()), index(0)
{
    thread = std::async(std::launch::async, &CThread::execute, this);
}

CThread::~CThread()
{
    threadIsNotFinish = false;
}

void CThread::execute()
{
    while (threadIsNotFinish)
    {
        time_point += nanoseconds(delay);
        std::this_thread::sleep_until(time_point);
        function(freq);
        index++;
    }
}
