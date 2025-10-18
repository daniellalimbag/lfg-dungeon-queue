 #include "utils.h"
 
 #include <random>
 
 namespace Utils {
   std::chrono::seconds randomDuration(int t1, int t2) {
     if (t1 > t2) std::swap(t1, t2);
     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<int> dist(t1, t2);
     return std::chrono::seconds{dist(gen)};
   }
 }
