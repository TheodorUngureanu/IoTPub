#include <EEPROM.h>

void setup()
{
  Serial.begin(9600);

  uint addr = 0;

  // fake data
  struct { 
    uint val = 0;
    char str[20] = "";
  } data;

  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
  // this step actually loads the content (512 bytes) of flash into 
  // a 512-byte-array cache in RAM
  EEPROM.begin(512);

  // read bytes (i.e. sizeof(data) from "EEPROM"),
  // in reality, reads from byte-array cache
  // cast bytes into structure called data
  EEPROM.get(addr,data);
  Serial.println("Old values are: "+String(data.val)+","+String(data.str));

  // fiddle with the data "read" from EEPROM
  data.val += 5;
  if ( strcmp(data.str,"hello") == 0 )
      strncpy(data.str, "jerry",20);
  else 
      strncpy(data.str, "hello",20);

  // replace values in byte-array cache with modified data
  // no changes made to flash, all in local byte-array cache
  EEPROM.put(addr,data);

  // actually write the content of byte-array cache to
  // hardware flash.  flash write occurs if and only if one or more byte
  // in byte-array cache has been changed, but if so, ALL 512 bytes are 
  // written to flash
  EEPROM.commit();  

  // clear 'data' structure
  data.val = 0; 
  strncpy(data.str,"",20);

  // reload data for EEPROM, see the change
  //   OOPS, not actually reading flash, but reading byte-array cache (in RAM), 
  //   power cycle ESP8266 to really see the flash/"EEPROM" updated
  EEPROM.get(addr,data);
  Serial.println("New values are: "+String(data.val)+","+String(data.str));
}

void loop()
{
  delay(5000);
  setup();
}
