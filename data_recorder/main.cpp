#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

using namespace std;


int main()
{
ofstream savefile;
ofstream timeelapsed;
savefile.open("DATA.csv");
timeelapsed.open("timeelapsed.txt");
zmq::context_t context(1);
zmq::socket_t datasub(context, ZMQ_SUB);
datasub.setsockopt(ZMQ_SUBSCRIBE, "", 0);
datasub.connect("tcp://*:5557");//raw torque
datasub.connect("tcp://*:5556");//unfiltered abdo
datasub.connect("tcp://*:5561");//filtered abdo
datasub.connect("tcp://*:5565");//camera position
datasub.connect("tcp://*:5566");//stop data alert
timespec time;
clock_gettime(CLOCK_REALTIME, &time);
long  begsec = time.tv_sec;
long  begnsec = time.tv_nsec;
double begtime = begsec + begnsec/10e8;



int i = 0;

while(true)
{

zmq::message_t datamessage;
  datasub.recv(&datamessage);

clock_gettime(CLOCK_REALTIME, &time);

long  sec = time.tv_sec;
long  nsec = time.tv_nsec;
double nowtime = sec + nsec/10e8-begtime;





  stringstream message(static_cast<char*>(datamessage.data()));
  string topic;
  string value;

  message>>topic>>value;
  if(topic == "STOP")
  {
    timeelapsed<< nowtime <<endl;
    break;
  }
 cout<<topic<<","<<value<<","<<nowtime<<endl;
  savefile<<topic<<","<<value<<","<<nowtime<<endl;
 i++;


}


savefile.close();




return 0;

}






















