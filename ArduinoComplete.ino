#include <SD.h> // SD library for reading and writing to memory
#include <SPI.h>  // SD module works by SPI communication protocole
#include <TMRpcm.h> // This library contains the audio player


#define SD_ChipSelectPin 53 // Connect the chip select pin of SD module at this pin
#define Relay_Signal 5  // (Relay_Signal == LOW) = Lock closed
#define LockClose  7    // (LockClose == LOW) = close the lock


int A;
int B;
int C;
int D;

File myFile; // for log file
TMRpcm tmrpcm; // for audio play


void setup() {

  /*************************** Initializing digital Pins ************************/
  pinMode(LockClose, INPUT_PULLUP);
  pinMode(Relay_Signal,OUTPUT);
  digitalWrite(Relay_Signal,HIGH);

  pinMode(10,INPUT);
  pinMode(11,INPUT);
  pinMode(12,INPUT);

  A = digitalRead(10);
  B = digitalRead(11);
  C = digitalRead(12);
  
  /*************************** Initializing Serial port **************************/
  
  Serial.begin(9600); // for sending and receiving data to laptop monitor
  

  /*************************** Initializing SD Card Module **************************/
  
  if (!SD.begin(SD_ChipSelectPin)) 
  {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }
  else{   
    Serial.println("SD ok"); 
  }

  tmrpcm.speakerPin = 46; //Connect the speaker positive to this pin
  

}

void loop() {
  String mystr;
  
  if (CloseLock()){
    Lock_Close();
    Serial.println("Door closed");
  }

  if ((A==digitalRead(10))&&(B == digitalRead(11)) &&(C == digitalRead(12))){
    
  }
  else{
    A = digitalRead(10);
    B = digitalRead(11);
    C = digitalRead(12);

    D = A + 2*B + C*4;
    if (D != 0)
    {
      if (D!=7){
        Lock_Open();
        Serial.print("finger print attempted by user : ");
        Serial.println(D);
        Serial.println("Reached before SD log file");
        SD_log_file_Write(D);
        Serial.println("Reached after SD log file");
        
      }
      else{
        Serial.println("finger print attempted by unknown user");
        SD_log_file_Write(D);
        play_audio();
      }
    }
    
  }
  
  //Serial.println("Reached End of the loop");
}



int CloseLock()
{
    if (!digitalRead(LockClose))
      return 1;
    else
      return 0;
}

void Lock_Open()
{
  digitalWrite(Relay_Signal, LOW);
}

void Lock_Close()
{
  digitalWrite(Relay_Signal, HIGH);
}

void SD_log_file_Write(int ID) // log file function
{
  myFile = SD.open("test.txt", FILE_WRITE);

    if (myFile) 
    {
      if(ID!=7){
        myFile.println("fingerprint attempted by finger ID" + (String)ID);
        Serial.println("wrote log file successfully"); 
      }
        
      else{
        Serial.println("reached SD log file for unknown person");
        myFile.println("fingerprint attempted by unknown person");
        Serial.println("wrote log file successfully");
      }
        
      myFile.close();
    }
  
   else 
   {
     Serial.println("error opening test.txt");
   }
}

int play_audio() // audio play function
{
  Serial.println("reached the audio playing function");
  tmrpcm.play("finger.wav");
  return 0;
}
