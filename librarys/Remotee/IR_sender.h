#ifndef IR_SENDER_H
#define IR_SENDER_H

#define PINIR 6
#define MAXPULSE 50000

#include <Arduino.h>
#include <IRremote.h>
#include <vector>
#include <string.h>

class IR_sender
{
private:
    vector<String> remote_names;
    IRsend irsend;
    vector<vector<unsigned int>> IR_signals;

public:
    IR_sender();
    ~IR_sender();
    void send_signal(unsigned int *rawbuf, int rawlen);
    void get_IR_signal(vector<String> &remote_names, vector<vector<unsigned int>> &IR_signals);
};