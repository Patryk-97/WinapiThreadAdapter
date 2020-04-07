#pragma once

#ifndef __ZADANIA_H__
#define __ZADANIA_H__

#include <stdint.h>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <future>
#include <atomic>

#include "WinapiThreadAdaptor.h"
#include "WinapiMutex.h"

class Tasks
{
public:

   Tasks();
   ~Tasks();

// task functions
   void task1(void);
   void task2(void);
   void task3(void);
   void task4(void);

private:

// thread functions
   static uint32_t threadFunc1(void* arg);
   static uint32_t threadFunc2(void* arg);
   static uint32_t threadFunc3(void* arg);
   static void threadFunc4(uint32_t threadNo, std::atomic<bool>& atomicFlag);

// static helpful functions
   static bool handleCommand(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, const std::string& command);
   static bool invokeThreadMethod(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, const std::string& command, uint8_t index);
   static void startThread(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, uint8_t index);
   static void stopThread(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, uint8_t index);
   static bool isCancelFutureCommand(std::atomic<bool> atomicFlags[], const std::string& command);
   static void cancelThread(std::atomic<bool>& atomicFlags);

// mutexes
   static std::unique_ptr<WinapiMutex> task2Mutex;
   static std::unique_ptr<WinapiMutex> task3Mutex;
   static std::unique_ptr<WinapiMutex> task4Mutex;
};

#endif // __ZADANIA_H__
