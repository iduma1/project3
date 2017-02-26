/***************************************************************************
 fifo.cpp  -  code to allow interprocess communication via a fifo, or "names pipe"
 *
* copyright : (C) 2009 by Jim Skon
*
* This code permits the creation and use of FIFOs for communication
* between processes.  
* 
* the named piped is created and used in /tmp
*
***************************************************************************/

#include "fifo.h"

using namespace std;

Fifo::Fifo(){
  fd = 0;
}

Fifo::Fifo(string name){
  // create a named pipe (FIFO)
  // build the name string
  string path = PATH;
  string sig = SIG;
  pipename = path + sig + name;

  umask(0);
  // Create (or open) the fifo
  int result = mknod(pipename.c_str(),MODE | S_IFIFO, 0);

  if ((result == -1) && (errno != EEXIST)) {
    cout << "Error creating pipe: " << name << endl;
    return;
  }
  cout << "Success creating pipe: " << name << endl;
  fd = 0;
  return;

}

void Fifo::openwrite() {
  if (fd !=0) {
    cout << "Fifo already opened: " << pipename << endl;
    return;
  }
  // Open the pipe
  fd = open(pipename.c_str(),O_WRONLY);

  // Check if open succeeded
  if (fd ==-1) {
	cout << "Error - bad input pipe: " << pipename << endl;
	return;
  }

}
void Fifo::openread() {
  if (fd !=0) {
    cout << "Fifo already opened: " << pipename << endl;
    return;
  }
  // Open the pipe
  fd = open(pipename.c_str(),O_RDONLY);

  // Check if open succeeded
  if (fd ==-1) {
	cout << "Error - bad input pipe: " << pipename << endl;
	return;
  }
}
  
void Fifo::fifoclose() {
  close(fd);
  fd = 0;
  
}


// Receive a message from a FIFO (named pipe)
string Fifo::recv() {
  if (fd ==0) {
    cout << "Fifo not open for read: " << pipename << endl;
    return ("");
  }

  int length, i;
  string message;
  bool done;
  int bytes;
  char inbuff;

  // clear message buffer
  message = "";
  // read until we see an end of message line
  done = false;
  i = 0;

  while (i<MaxMess && !done) {
    // Read the next character in the fifo
    bytes = read(fd, &inbuff,1);

    // -1 means something isn't working
    if (bytes ==-1) {
      cout << "Error - bad read on input pipe: " << pipename << endl;
      return("");
    }
    // check if nothing was read
    if (bytes > 0) {
      // Check if end of message
      if (inbuff == MESSTERM && (i > 0)) {
	done = true;
      } else {
	i++;
	message += inbuff;
      }
    } else {
      // Nothing to read, try to open
      fifoclose();
      openread();
    }
  }
  return(message);
}

// Send a message to a FIFO (named pipe)
// Return 0 if fails, 1 if succeeds
void Fifo::send(string message) {
  if (fd ==0) {
    cout << "Fifo not open for send: " << pipename << endl;
    return;
  }

  int bytes;

  // Append end of message terminator
  message = message + MESSTERM;
  bytes = write(fd, message.c_str(),message.length());
  if (bytes ==-1) {
    cout << "Error - bad write on output pipe: " << pipename << endl;
    return;
  }
    if (bytes == 0) {
      cout << "Error - nothing written: " << pipename << endl;
      return;
    }
  return;
}