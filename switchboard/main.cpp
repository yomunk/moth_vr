
#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <boost/thread.hpp>
#include <string>


using namespace std;



float unpacked = 0;

float torque;
float abdotrack;
string program = "GO";



void threadedtorque()
{

 zmq::context_t context (1);
 zmq::socket_t sub (context, ZMQ_SUB);
 const char *filter = "rawtorque";
 sub.setsockopt(ZMQ_SUBSCRIBE,filter, strlen(filter));
sub.connect("tcp://localhost:5557");




  while(true)
  {

zmq::message_t torquemes;
    sub.recv(&torquemes);

stringstream iss(static_cast<char*>(torquemes.data()));

string topic;
iss>>topic>>torque;
//cout<<torque<<endl;
  }





}





void threadedabdotracker()
{
 zmq::context_t context (1);
 zmq::socket_t sub (context, ZMQ_SUB);

 const char *filter = "filterx";
 sub.setsockopt(ZMQ_SUBSCRIBE,filter,strlen(filter));
sub.connect("tcp://localhost:5561");



  while(true)
  {
zmq::message_t abdotrackermes;
sub.recv(&abdotrackermes);

stringstream iss(static_cast<char*>(abdotrackermes.data()));


string topic;

iss >> topic >>abdotrack ;

cout<<topic<<" "<<abdotrack<<endl;
  }



}


void listenforstop()
{

 zmq::context_t context (1);
 zmq::socket_t sub (context, ZMQ_SUB);

 sub.setsockopt(ZMQ_SUBSCRIBE,"STOP",4);
sub.connect("tcp://localhost:5566");
while(true)
{
zmq::message_t stop;
sub.recv(&stop);
stringstream iss(static_cast<char*>(stop.data()));

string restofmessage;

iss >>program>>restofmessage;

}
}

int main()
{

 zmq::context_t context(1);
 boost::thread thread1(threadedtorque);
 boost::thread thread2(threadedabdotracker);
 boost::thread thread3(listenforstop);

 zmq::socket_t replier (context, ZMQ_REP);
 replier.bind("tcp://*:5560");
while(true)
{
  cout<<program<<endl;
  zmq::message_t request;
replier.recv(&request);

stringstream iss(static_cast<char*>(request.data()));

string whattype;
iss>>whattype;

cout<<whattype<<endl;
zmq::message_t response(20);
if(program == "GO")
{
if(whattype == "torque")
{
snprintf ((char *) response.data(), 20, "torque %0.5f", torque);
}
else if(whattype == "abdo")
{
snprintf ((char *) response.data(), 20, "abdo %0.5f", abdotrack);
cout<<abdotrack<<endl;
}
}
else
{
cout<<"STOOOOOOOOOOOP"<<endl;
snprintf ((char *) response.data(),4, "STOP");
}
replier.send(response);


}

}
