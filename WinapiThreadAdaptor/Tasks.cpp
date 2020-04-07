#include "Tasks.h"

std::unique_ptr<WinapiMutex> Tasks::task2Mutex = std::make_unique<WinapiMutex>();
std::unique_ptr<WinapiMutex> Tasks::task3Mutex = std::make_unique<WinapiMutex>();
std::unique_ptr<WinapiMutex> Tasks::task4Mutex = std::make_unique<WinapiMutex>();

Tasks::Tasks()
{

}

Tasks::~Tasks()
{

}

void Tasks::task1(void)
{
   std::unique_ptr<WinapiThreadAdaptor> thread = std::make_unique<WinapiThreadAdaptor>(Tasks::threadFunc1, nullptr);
   thread->join();
}

void Tasks::task2(void)
{
   std::vector<std::unique_ptr<WinapiThreadAdaptor>> threads;
   std::string command;
   bool isCommandValid = true;
   for (uint32_t iter = 0; iter < 10; iter++)
   {
      threads.push_back(std::make_unique<WinapiThreadAdaptor>(Tasks::threadFunc2, (void*)iter,
         WinapiThreadAdaptor::ThreadState::SUSPENDED));
   }

   do
   {
      std::getline(std::cin, command);
      isCommandValid = handleCommand(threads, command);
      if (isCommandValid == false)
      {
         std::cout << "Wrong command\n";
      }
   } while (isCommandValid == true);
}

void Tasks::task3(void)
{
   std::vector<std::unique_ptr<WinapiThreadAdaptor>> threads;
   for (uint32_t iter = 1; iter <= 10; iter++)
   {
      threads.push_back(std::make_unique<WinapiThreadAdaptor>(Tasks::threadFunc3, (void*)iter,
         WinapiThreadAdaptor::ThreadState::RESUMED));
   }
   for (uint32_t i = 0; i < 10; i++)
   {
      threads[i]->join();
   }
}

void Tasks::task4(void)
{
   std::vector<std::future<void>> asyncTasks;
   std::atomic<bool> atomicFlags[] = { true, true, true, true, true, true, true, true, true, true };
   std::string command;
   bool isCommandValid = true;
   for (uint32_t iter = 0; iter < 10; iter++)
   {
      asyncTasks.push_back(std::async(Tasks::threadFunc4, iter, std::ref(atomicFlags[iter])));
   }

   do
   {
      std::getline(std::cin, command);
      isCommandValid = isCancelFutureCommand(atomicFlags, command);
      if (isCommandValid == false)
      {
         std::cout << "Wrong command\n";
      }
   } while (isCommandValid == true);
}

uint32_t Tasks::threadFunc1(void* arg)
{
   std::cout << "Hello World!!\n";
   return 0;
}

uint32_t Tasks::threadFunc2(void* arg)
{
   uint32_t threadNo = (uint32_t)(arg);
   for (uint8_t character = 'A'; character <= 'Z'; character++)
   {
      Tasks::task2Mutex->lock();
      std::cout << character << threadNo << "\n";
      Tasks::task2Mutex->unlock();
      WinapiThreadAdaptor::sleep(1000);
   }
   return 0;
}

uint32_t Tasks::threadFunc3(void* arg)
{
   uint32_t threadNo = (uint32_t)(arg);
   for (uint8_t character = 'A'; character <= 'Z'; character++)
   {
      Tasks::task3Mutex->lock();
      std::cout << character << threadNo << "\n";
      WinapiThreadAdaptor::sleep(1000);
      Tasks::task3Mutex->unlock();
   }
   return 0;
}

void Tasks::threadFunc4(uint32_t threadNo, std::atomic<bool>& atomicFlag)
{
   for (uint8_t character = 'A'; character <= 'Z'; character++)
   {
      if (atomicFlag)
      {
         Tasks::task4Mutex->lock();
         std::cout << character << threadNo << "\n";
         Tasks::task4Mutex->unlock();
         WinapiThreadAdaptor::sleep(1000);
      }
      else
      {
         return;
      }
   }
}

bool Tasks::handleCommand(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, const std::string& command)
{
   // locals
   bool rV = true;
   size_t index = command.find(' ');
   size_t dashIndex = 0;
   std::string firstPart = command.substr(0, index);
   std::string secondPart = command.substr(index + 1, command.size() - index);

   std::transform(firstPart.begin(), firstPart.end(), firstPart.begin(), ::tolower);
   if (firstPart == "start" || firstPart == "stop" && secondPart.size() <= 4)
   {
      dashIndex = secondPart.find('-');
      if (dashIndex == 1)
      {
         if (secondPart[0] >= '1' && secondPart[0] <= '9')
         {
            if (secondPart[2] == '1' && secondPart[3] == '0')
            {
               for (uint8_t iter = secondPart[0]; iter <= '9'; iter++)
               {
                  Tasks::invokeThreadMethod(threads, firstPart, iter);
               }
               Tasks::invokeThreadMethod(threads, firstPart, '0');
            }
            else if (secondPart[2] >= '1' && secondPart[2] <= '9' && secondPart[2] >= secondPart[0] &&
               (secondPart[3] <= '0' || secondPart[3] >= '9'))
            {
               for (uint8_t iter = secondPart[0]; iter <= secondPart[2]; iter++)
               {
                  Tasks::invokeThreadMethod(threads, firstPart, iter);
               }
            }
            else
            {
               rV = false;
            }
         }
         else
         {
            rV = false;
         }
      }
      else if(dashIndex == std::string::npos)
      {
         if (secondPart.size() == 2 && secondPart[0] == '1' && secondPart[1] == '0')
         {
            Tasks::invokeThreadMethod(threads, firstPart, '0');
         }
         else if(secondPart[0] >= '1' && secondPart[0] <= '9' && (secondPart[1] <= '0' || secondPart[1] >= '9'))
         { 
            Tasks::invokeThreadMethod(threads, firstPart, secondPart[0]);
         }
         else
         {
            rV = false;
         }
      }
      else
      {
         rV = false;
      }
   }
   else
   {
      rV = false;
   }
   return rV;
}

bool Tasks::invokeThreadMethod(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, const std::string& command, uint8_t index)
{
   // locals
   bool rV = true;

   if (command == "start")
   {
      Tasks::startThread(threads, index);
   }
   else if(command == "stop")
   {
      Tasks::stopThread(threads, index);
   }
   else
   {
      rV = false;
   }
   return rV;
}

void Tasks::startThread(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, uint8_t index)
{
   switch (index)
   {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      {
         threads[index - '0']->resume();
         break;
      }
      default:
      {
         break;
      }
   }
}

void Tasks::stopThread(const std::vector<std::unique_ptr<WinapiThreadAdaptor>>& threads, uint8_t index)
{
   switch (index)
   {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      {
         threads[index - '0']->suspend();
         break;
      }
      default:
      {
         break;
      }
   }
}

bool Tasks::isCancelFutureCommand(std::atomic<bool> atomicFlags[], const std::string& command)
{
   // locals
   bool rV = true;
   size_t index = command.find(' ');
   size_t dashIndex = 0;
   std::string firstPart = command.substr(0, index);
   std::string secondPart = command.substr(index + 1, command.size() - index);

   std::transform(firstPart.begin(), firstPart.end(), firstPart.begin(), ::tolower);
   if (firstPart == "cancel" && secondPart.size() <= 4)
   {
      dashIndex = secondPart.find('-');
      if (dashIndex == 1)
      {
         if (secondPart[0] >= '1' && secondPart[0] <= '9')
         {
            if (secondPart[2] == '1' && secondPart[3] == '0')
            {
               for (uint8_t iter = secondPart[0]; iter <= '9'; iter++)
               {
                  Tasks::cancelThread(atomicFlags[iter - '0']);
               }
               Tasks::cancelThread(atomicFlags[0]);
            }
            else if (secondPart[2] >= '1' && secondPart[2] <= '9' && secondPart[2] >= secondPart[0] &&
                 (secondPart[3] <= '0' || secondPart[3] >= '9'))
            {
               for (uint8_t iter = secondPart[0]; iter <= secondPart[2]; iter++)
               {
                  Tasks::cancelThread(atomicFlags[iter - '0']);
               }
            }
            else
            {
               rV = false;
            }
         }
         else
         {
            rV = false;
         }
      }
      else if (dashIndex == std::string::npos)
      {
         if (secondPart.size() == 2 && secondPart[0] == '1' && secondPart[1] == '0')
         {
            Tasks::cancelThread(atomicFlags[0]);
         }
         else if (secondPart[0] >= '1' && secondPart[0] <= '9' && (secondPart[1] <= '0' || secondPart[1] >= '9'))
         {
            Tasks::cancelThread(atomicFlags[secondPart[0] - '0']);
         }
         else
         {
            rV = false;
         }
      }
      else
      {
         rV = false;
      }
   }
   else
   {
      rV = false;
   }
   return rV;
}

void Tasks::cancelThread(std::atomic<bool>& atomicFlags)
{
   atomicFlags = false;
}