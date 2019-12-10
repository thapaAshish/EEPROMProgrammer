#define SHIFT_DATA 2   //SER or DS of 74hc595
#define SHIFT_CLK 3    //SRCLK or SH_CP(shift register clock pulse)
#define SHIFT_LATCH 4  //RCLK or ST_CP (output register clock pulse) . when set HIGH shows the value present in SHIFT register
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

void printContents(){
  for(int base = 0; base <=255; base += 16){
    byte data[16];
    for(int offset = 0;offset <=15;offset +=1){
      data[offset] = readEEPROM(base+offset);      
    }
    char buf[80];
    sprintf(buf,"%03x: %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",base,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[12],data[13],data[14],data[15]); 

    Serial.println(buf);
  }
}

byte readEEPROM(int address){
   for(int pin=EEPROM_D7;pin>=EEPROM_D0;pin-=1){
    pinMode(pin,INPUT);
  }
   setAddress(address,/*output enable*/ true);
   byte data = 0;
   for(int pin= EEPROM_D7;pin >= EEPROM_D0;pin -= 1){
    data = (data << 1) + digitalRead(pin);
   }
return data;
}

void writeEEPROM(int address, byte data){
   for(int pin=EEPROM_D7;pin>=EEPROM_D0;pin-=1){
    pinMode(pin,OUTPUT);
  }
  
  setAddress(address,false /*no need to output the data.*/);
  for(int pin= EEPROM_D0;pin<=EEPROM_D7;pin+=1){
    digitalWrite(pin,data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN,LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN,HIGH);
  delay(10);
  
}


void setAddress(int address, bool outputEnable){
  //output enable is inversed in 28c16 which means when its true then address remain as it is
  //when it is false bitwise OR with 10000000 --> 0x80
  
  shiftOut(SHIFT_DATA,SHIFT_CLK,MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));  //right shift by 8 for 2nd 74hc595 by 8 for our 2nd 74hc595
  shiftOut(SHIFT_DATA,SHIFT_CLK,MSBFIRST, (address));

  
  digitalWrite(SHIFT_LATCH,LOW);
  digitalWrite(SHIFT_LATCH,HIGH);
  digitalWrite(SHIFT_LATCH,LOW);
}


void setup() {
  pinMode(SHIFT_DATA,OUTPUT);
  pinMode(SHIFT_CLK,OUTPUT);
  pinMode(SHIFT_LATCH,OUTPUT);
  /*
   * digitalWrite before pinMode setting ensures no loss of data
   * digitalWrite to High enables a pullup resistor and pinMode set to output can't be low. 
  */
  digitalWrite(WRITE_EN,HIGH);
  pinMode(WRITE_EN,OUTPUT); 
  Serial.begin(57600);

  writeEEPROM(0,10);
  writeEEPROM(1,5);
  writeEEPROM(2,6);
  writeEEPROM(3,1);
  writeEEPROM(4,5);
  writeEEPROM(5,6); 
  
  printContents();

  
}



void loop() {
  // put your main code here, to run repeatedly:

}
