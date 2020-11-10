#include <SD.h>
#include <string.h>

#define chipSelect SS1
#define PIN_IR_OUT 7
#define PIN_IR_IN 6
#define MAXPULSE 50000

Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;

unsigned int currentPulse = 0;
unsigned long pulses[200] = {0};
String Pulses = "";

enum STATE
{
    RESET = 0,
    IDLE,
    WAIT_SEND_IR,
    WAIT_REC_IR,
    IR_RECEIVED,
    SEND_IR,
    LOAD_DATA,
    STORE_DATA = 7
};

enum DEV_STATE
{
    PRINT_SD_INFO = 8,
    WRITE_FILE,
    READ_FILE,
    DELETE_FILE,
    FIND_FILE
};

int c_state = RESET;
int n_state = RESET;

/**************
 * IR lib
 *************/

void custom_delay_usec(unsigned long uSecs)
{

    unsigned long Start = micros();
    unsigned long endMicros = Start + uSecs;
    if (endMicros < Start)
    { // Check if overflow
        while (micros() > Start)
        {
        } // wait until overflow
    }
    while (micros() < endMicros)
    {
    } // normal wait
}

void space(unsigned int Time)
{
    digitalWrite(PIN_IR_OUT, LOW);
    if (Time > 0)
    {
        unsigned long Start = micros();
        unsigned long endMicros = Start + Time - 4;
        custom_delay_usec(Time);
    }
}

void mark(unsigned int Time)
{
    unsigned long Start = micros();
    unsigned long endMicros = Start + Time;
    int count = 0;

    while (micros() < endMicros)
    {
        digitalWrite(PIN_IR_OUT, HIGH);
        custom_delay_usec(10);
        digitalWrite(PIN_IR_OUT, LOW);
        custom_delay_usec(9);
        count++;
    }
    //Serial.println("count : " + count);
}

void Send_IR(String signal)
{
    unsigned int tmp_ir[500] = {0};
    int start = 0, end = 0, i = 0;
    while (1)
    {
        end = signal.indexOf(',', start);
        if (end == -1)
            break;
        String substr = signal.substring(start, end);
        int tmp_int = substr.toInt();
        tmp_ir[i++] = (unsigned int)tmp_int;
        start = end + 1;
    }

    digitalWrite(PIN_IR_OUT, LOW);

    for (unsigned int i = 0; tmp_ir[i] != 0; i++)
    {
        unsigned long Start = micros();
        if (i & 1)
            space(tmp_ir[i]);
        else
            mark(tmp_ir[i]);
    }

    digitalWrite(PIN_IR_OUT, LOW);
}

String Recv_IR()
{
    String IR_out = "";

    unsigned long highPulse = 0; // Pulse length
    unsigned long lowPulse = 0;
    unsigned long currTime = micros();
    char tmp[16];

    while (digitalRead(PIN_IR_IN))
    {
        highPulse = micros() - currTime;

        if ((highPulse >= MAXPULSE) && (currentPulse != 0))
        {
            // for (int i = 0; i < currentPulse - 1; i++)
            // {
            //     itoa(pulses[i * 2 + 1], tmp, 10);
            //     IR_out.concat(tmp);
            //     IR_out.concat(",");

            //     itoa(pulses[(i + 1) * 2], tmp, 10);
            //     IR_out.concat(tmp);
            //     IR_out.concat(",");
            // }
            // itoa(pulses[currentPulse * 2 - 1], tmp, 10);
            // IR_out.concat(tmp);

            currentPulse = 0;

            //return IR_out;
            return Pulses;
        }
    }
    //pulses[currentPulse * 2] = highPulse;

    if (currentPulse != 0)
    {
        itoa(highPulse, tmp, 10);
        Pulses.concat(tmp);
        Pulses.concat(",");
    }

    currTime = micros();

    while (!digitalRead(PIN_IR_IN))
    {
        lowPulse = micros() - currTime;
    }
    //pulses[currentPulse * 2 + 1] = lowPulse;

    itoa(lowPulse, tmp, 10);
    Pulses.concat(tmp);
    Pulses.concat(",");
    currentPulse++;

    if (currentPulse == 100)
        return Pulses;
}

/**************
 * init SD card
 *************/

void SD_init()
{
    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
    }
    Serial.println("card initialized.");

    if (!card.init(SPI_HALF_SPEED, chipSelect))
    {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return;
    }
    else
        Serial.println("Wiring is correct and a card is present.");

    Serial.print("\nCard type: ");
    switch (card.type())
    {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
    default:
        Serial.println("Unknown");
    }

    if (!volume.init(card))
    {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return;
    }
}

void Print_SD_Info()
{
    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();

    volumesize = volume.blocksPerCluster();
    volumesize *= volume.clusterCount();
    volumesize *= 512;
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);

    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    root.ls(LS_R | LS_DATE | LS_SIZE);
}

void printDirectory(File dir, int numTabs)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }

        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }

        Serial.print(entry.name());

        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

/***********
database_controll
************/

String LoadFile(String filename, String signal_name)
{
    myFile = SD.open(filename);
    String buf("");

    if (myFile)
    {
        char c;
        while (myFile.available())
        {
            char c = myFile.read();
            buf.concat(c);
        }

        myFile.close();

        if (buf.indexOf(signal_name) != -1)
        {
            return String("FOUND!!!");
        }
        else
        {
            return String("NOT_FOUND");
        }
    }
    else
    {
        myFile.close();
        return String("FILE_OPEN_FAIL");
    }
}

void StoreFile(String filename, String data)
{
    AppendWriteFile(filename, data);
}

String ReadFile(String filename)
{
    myFile = SD.open(filename);
    String buf("");

    if (myFile)
    {
        char c;
        while (myFile.available())
        {
            char c = myFile.read();
            buf.concat(c);
        }

        myFile.close();
        return buf;
    }
    else
    {
        myFile.close();
        return String("");
    }
}

int OverWriteFile(String filename, String data)
{
    if (SD.exists(filename))
        SD.remove(filename);
    else
        myFile = SD.open(filename, FILE_WRITE);

    if (myFile)
    {
        Serial.println(data);
        myFile.println(data);

        Serial.println("Write file success..");
        myFile.close();
        return 1;
    }
    else
    {
        Serial.println("Write file fail!!!");
        myFile.close();
        return 0;
    }
}

int AppendWriteFile(String filename, String data)
{
    myFile = SD.open(filename, FILE_WRITE);

    if (myFile)
    {
        Serial.println(data);
        myFile.println(data);

        Serial.println("Write file success..");
        myFile.close();
        return 1;
    }
    else
    {
        Serial.println("Write file fail!!!");
        myFile.close();
        return 0;
    }
}

int DeleteFile(String filename)
{
    return SD.remove(filename);
}

String SerialKeyWait()
{
    int flag = 0;
    String serial_str;

    while (1)
    {
        if (Serial.available())
        {
            flag = 1;
            char serial_key = Serial.read();
            serial_str.concat(serial_key);
        }
        else if (flag != 0)
            return serial_str.substring(0, serial_str.length());
    }
}

char DipKeyWait()
{
    while (1)
    {
        if (1)
        {
            return 'c';
        }
    }
}

void init_remotee()
{
    SD_init();
    Print_SD_Info();
}

void setup()
{
    Serial.begin(9600);
    pinMode(PIN_IR_IN, INPUT);
    init_remotee();
}

void loop()
{

    String serial_str = "";
    String read_data = "";
    String read_ir = "";
    char serial_chr = 0;

    while (1)
    {
        switch (n_state)
        {
            if (!(n_state == LOAD_DATA && n_state == SEND_IR))
                serial_str = String("");

        case RESET:
            c_state = RESET;
            Serial.println("Current state is RESET");

            n_state = IDLE;

            break;
        case IDLE:
            c_state = IDLE;
            Serial.println("Current state is IDLE");

            serial_str = SerialKeyWait();

            if (serial_str[0] == 's')
                n_state = WAIT_SEND_IR;
            else if (serial_str[0] == 'r')
                n_state = WAIT_REC_IR;
            else if (serial_str[0] == 'P')
                n_state = PRINT_SD_INFO;
            else if (serial_str[0] == 'R')
                n_state = READ_FILE;
            else if (serial_str[0] == 'W')
                n_state = WRITE_FILE;
            else if (serial_str[0] == 'D')
                n_state = DELETE_FILE;
            else if (serial_str[0] == 'F')
                n_state = FIND_FILE;

            break;

        case WAIT_SEND_IR:
            c_state = WAIT_SEND_IR;
            Serial.println("Current state is WAIT_SEND_IR");

            serial_str = SerialKeyWait();

            if (serial_str.length() > 0)
            {
                if (serial_str[0] == 'i')
                    n_state = IDLE;
                else
                    n_state = LOAD_DATA;
            }

            break;

        case WAIT_REC_IR:
            c_state = WAIT_REC_IR;
            Serial.println("Current state is WAIT_REC_IR");
            Serial.println("waiting for get IR signal...");

            read_ir = String("");
            Pulses = String("");

            while (read_ir.length() == 0)
            {
                read_ir = Recv_IR();
            }

            Serial.println(read_ir);
            Serial.println("get IR signal!!!");

            if (read_ir.length() != 0)
                n_state = IR_RECEIVED;
            else
                n_state = IDLE;

            break;

        case IR_RECEIVED:
            c_state = IR_RECEIVED;
            Serial.println("Current state is IR_RECEIVED");
            Serial.println("if you want to store IR signal, press 's'");

            serial_str = SerialKeyWait();

            if (serial_str[0] == 's')
                n_state = STORE_DATA;
            else if (serial_str[0] == 'r')
                n_state = WAIT_REC_IR;
            else if (serial_str[0] == 'p')
                Serial.println(read_ir);

            break;

        case SEND_IR:
            c_state = SEND_IR;
            Serial.println("Current state is SEND_IR");

            Send_IR(read_data);
            Serial.println("Send IR complete!!!");

            n_state = WAIT_SEND_IR;

            break;

        case LOAD_DATA:
        {
            c_state = LOAD_DATA;
            Serial.println("Current state is LOAD_DATA");

            String Signal_name("SIGNAL1.TXT");
            read_data = ReadFile(Signal_name);
            Serial.println("Load " + Signal_name + "...");
            Serial.println(read_data);

            if (read_data.compareTo("NOT_FOUND") == 0)
            {
                Serial.println("signal not found...");
                n_state = WAIT_SEND_IR;
            }
            else
                n_state = SEND_IR;

            break;
        }

        case STORE_DATA:
        {
            c_state = STORE_DATA;
            Serial.println("Current state is STORE_DATA");

            char base_file_name[] = "SIGNAL";
            char file_name[50] = "";
            for (int i = 1; i < 10000; i++)
            {
                sprintf(file_name, "%s%d.TXT", base_file_name, i);
                if (!SD.exists(file_name))
                    break;
                else
                    strcpy(file_name, base_file_name);
            }

            Serial.print("stored file name is ");
            Serial.println(file_name);

            StoreFile(String(file_name), String(read_ir));

            read_ir = String("");
            read_data = String("");
            n_state = IDLE;

            break;
        }
            //////////////FOR DEV BELOW//////////////////

        case PRINT_SD_INFO:
            c_state = PRINT_SD_INFO;
            Serial.println("PRINT_SD_INFO");

            Print_SD_Info();
            myFile = SD.open("/");

            Serial.println("========DIR========");
            printDirectory(myFile, 0);

            n_state = RESET;

            break;
        case WRITE_FILE:
        {
            c_state = WRITE_FILE;
            Serial.println("WRITE_FILE");

            serial_str = SerialKeyWait();

            String tmp("DATABASE.TXT");
            OverWriteFile(tmp, serial_str);

            n_state = RESET;

            break;
        }

        case READ_FILE:
        {
            c_state = READ_FILE;
            Serial.println("READ_FILE");

            String tmp("DATABASE.TXT");
            read_data = ReadFile(tmp);
            Serial.println("<FILE START>");
            Serial.print(read_data);
            Serial.println("<FILE END>");

            n_state = RESET;

            break;
        }

        case DELETE_FILE:
        {
            c_state = DELETE_FILE;
            Serial.println("DELETE_FILE");

            String tmp("DATABASE.TXT");
            DeleteFile(tmp);

            n_state = RESET;

            break;
        }

        case FIND_FILE:
            c_state = FIND_FILE;
            Serial.println("FIND_FILE");
            n_state = RESET;

            break;
        }
    }
}
