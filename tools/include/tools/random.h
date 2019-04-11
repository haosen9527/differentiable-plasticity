#ifndef RANDOM_H
#define RANDOM_H

/*******************
 * use C++11 random
 *******************/

#include <iostream>
#include <random>
#include <vector>

namespace Tools {

using namespace std;

inline int RandomIntTool(pair<int,int> randge)
{

  static std::mt19937 rng;
  //rng.seed(std::random_device()());
  //SetSeed.rngseed
  //rng.seed(SetSeed.rngseed);
  std::uniform_int_distribution<std::mt19937::result_type> range_num(randge.first,randge.second);
  return range_num(rng);

}


inline double RandomDoubleTool(pair<double,double> randge)
{

  static std::mt19937 rng;
  //rng.seed(std::random_device()());
  //SetSeed.rngseed
  //rng.seed(SetSeed.rngseed);
  std::uniform_real_distribution<double> range_num(randge.first,randge.second);
  return range_num(rng);

}
inline int RandomSeed(int min,int max)
{
  int Number;
  Number = (rand()%(max-min)+min);
  return Number;
}
inline vector<int> arange(int start,int end,int step=1)
{
  //std::srand(SetSeed.rngseed);
  vector<int> NumberList;
  int diff = end-start;
  //std::cout<<"diff:"<<diff<<std::endl;
  for(int i=start;i<end;i++)
  {
    NumberList.push_back(i);
  }
  //std::cout<<"Numlist:"<<NumberList.size()<<std::endl;
  return NumberList;
}
inline vector<int>vectorShuffleInt(vector<int> unshuffled)
{
  vector<int> shuffled;
  //std::srand(SetSeed.rngseed);
  unsigned int p,size=unshuffled.size();
  while(size)
  {
    p=rand()%size--;
    shuffled.push_back(unshuffled[p]);
    unshuffled.erase(unshuffled.begin()+p);
  }
  return shuffled;
}
inline vector<string>vectorShuffleString(vector<string> unshuffled)
{
  vector<string> shuffled;
  //std::srand(SetSeed.rngseed);
  unsigned int p,size=unshuffled.size();
  while(size)
  {
    p=rand()%size--;
    shuffled.push_back(unshuffled[p]);
    unshuffled.erase(unshuffled.begin()+p);
  }
  return shuffled;
}

inline vector<int>vectorNbclasses(vector<int> unchoice,int num)
{

  //std::cout<<"unchoice:"<<unchoice.size()<<std::endl;
  vector<int> choice;
  for(int i=0;i<num;i++)
  {
    choice.push_back(unchoice[i]);
  }

  return choice;
}


}//namespace tools

#endif // RANDOM_H
