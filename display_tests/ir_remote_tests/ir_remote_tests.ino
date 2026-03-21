#include <IRremote.hpp>

int receiver = 15; // Signal Pin of IR receiver to Arduino Digital Pin 13

/*-----( Function )-----*/
void translateIR() // takes action based on IR code received
{
  switch(IrReceiver.decodedIRData.command)
  {
  case 0x45: Serial.println("power"); break;
  case 0x46: Serial.println("mode"); break;
  case 0x47: Serial.println("mute"); break;
  case 0x44: Serial.println("pause"); break;
  case 0x40: Serial.println("back"); break;
  case 0x43: Serial.println("forward"); break;
  case 0x07: Serial.println("equalizer"); break;
  case 0x15: Serial.println("minus"); break;
  case 0x09: Serial.println("plus"); break;
  case 0x16: Serial.println("0"); break;
  case 0x19: Serial.println("arrows idk waht this does"); break;
  case 0xD: Serial.println("U/SD I also don't know what this does"); break;
  case 0xC: Serial.println("1"); break;
  case 0x18: Serial.println("2"); break;
  case 0x5E: Serial.println("3"); break;
  case 0x8: Serial.println("4"); break;
  case 0x1C: Serial.println("5"); break;
  case 0x5A: Serial.println("6"); break;
  case 0x42: Serial.println("7"); break;
  case 0x52: Serial.println("8"); break;
  case 0x4A: Serial.println("9"); break;

  default: 
    Serial.print("Unknown code: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
  }

  delay(500); // Do not get immediate repeat
}

void setup()
{
  Serial.begin(115200);
  Serial.println("IR Receiver Button Decode"); 
  
  IrReceiver.begin(receiver, DISABLE_LED_FEEDBACK); // Start the receiver
  
  Serial.print("Ready to receive IR signals on pin ");
  Serial.println(receiver);
}

void loop()
{
  if (IrReceiver.decode()) // have we received an IR signal?
  {
    translateIR(); 
    //IrReceiver.printIRResultShort(&Serial);  // prints everything it received, useful for mapping to other remotes :)
    IrReceiver.resume(); // Enable receiving of the next value
  }  
}