#include <comedilib.h>
#include <time.h>
#include <zmq.hpp>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;
int main(int argc, char **argv)
{
  zmq::context_t context (1);
  zmq::socket_t publisher (context, ZMQ_PUB);
  publisher.bind("tcp://*:5557");





  int chan = 0;
  int subdev= 0;
  int range = 0;
  int aref = AREF_GROUND;
  int retval;

  comedi_t* daq = comedi_open("/dev/comedi0");
  int maxdata = comedi_get_maxdata(daq, subdev, chan);
  comedi_range* cr = comedi_get_range(daq, subdev, chan, range);
  lsampl_t data;

float packed = 0;
  while (true)
  {
   bool caught = false;
   retval = comedi_data_read(daq, subdev, chan, range, aref, &data);
  float packed  = comedi_to_phys(data, cr, maxdata);

 //  sleep(1);


   zmq::message_t message(30);
   snprintf((char *) message.data(),30,"rawtorque %f", packed);
   publisher.send(message);

  cout<<packed<<endl;
   usleep(1e4);

  }


  return 0;
}
