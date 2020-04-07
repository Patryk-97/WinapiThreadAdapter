#include <memory>
#include <atomic>
#include <Windows.h>

#include "Tasks.h"

int main(void)
{
   std::unique_ptr<Tasks> tasks = std::make_unique<Tasks>();
   uint16_t taskNo;
   uint8_t decision;

   std::cout << "Zadanie 2\n";
   std::cout << "\tDozwolone komendy: start 1-9\n";
   std::cout << "\tstop 8-10\n";
   std::cout << "\tstart 4\n";
   std::cout << "\tstop 7\n";
   std::cout << "gdzie liczby to identyfikatory watkow\n\n";

   std::cout << "Zadanie 4\n";
   std::cout << "\tDozwolone komendy: cancel 1-9\n";
   std::cout << "\tcancel 6\n";
   std::cout << "gdzie liczby to identyfikatory watkow\n\n";

   do
   {
      std::cout << "Wpisz numer zadania (1-4): ";
      std::cin >> taskNo;
      std::cin.ignore();
      system("cls");
      switch (taskNo)
      {
         case 1:
         {
            tasks->task1();
            break;
         }
         case 2:
         {
            tasks->task2();
            break;
         }
         case 3:
         {
            tasks->task3();
            break;
         }
         case 4:
         {
            tasks->task4();
            break;
         }
         default:
         {
            std::cout << "Wrong task number\n";
            break;
         }
      }
      std::cout << "Do you want to try again? (Y-N): ";
      std::cin >> decision;
      system("cls");
   } while (decision == 'Y' || decision == 'y');

   return 0;
}