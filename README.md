# remotee project
Clone every remote control in the world and make it yours! Through this project, you can create an all-around remote control that can be controlled without a remote control by replicating the device. The project is designed for Arduino support board.

# circuit diagram
![아두이노 회로도 small](https://user-images.githubusercontent.com/53033449/98937280-84fd1f00-2529-11eb-830a-0f5e8226b947.jpg)

## materials
    Arduino MKR Zero board x 1
    1k Ohm resistor        x 5
    IR receiver            x 1
    IR sender              x 1
    push button            x 3
    LED                    x 2

# state diagram
![state_machine](https://user-images.githubusercontent.com/53033449/98940696-b75d4b00-252e-11eb-9602-30d94bb3ab9a.jpg)


## signal data format

    SIGNAL1.TXT 9033,4464,594,535...\n
    SIGNAL2.TXT 9012,4451,563,524...\n
    ...

the signal captured at state `IR_received` will be stored at root of SD card.
data format looks like above, seperated by comma.
each number mean mark and space length of signal(_us_).
_2n_ th signal is space and _2n-1_ th signal mean mark.
For example first line of signal, `9033` and `594` corresponds to mark

# upload Sketch

clone repository to your directory 
```
git clone https://github.com/thsvkd/remotee.git
```

in `Arduino` folder you can see four directory.

## IR_receiver
for receive IR signal from remote control.

## IR_sender
for send IR signal

## button_test
for button test

## remotee
this is the main sketch

### just turn on Arduino IDE and upload the sketch, after make your own Aduino board.