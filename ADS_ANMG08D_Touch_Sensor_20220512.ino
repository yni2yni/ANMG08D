// ADS Touch Sensor Test Example Program (IC P/N:ANMG08D )
// Code:
// Date: 2022.05.12  Ver.: 0.0.6
// H/W Target: ARDUINO UNO R3, S/W: Arduino IDE  1.8.19
// Author: Park, Byoungbae (yni2yni@hanmail.net)
// Note: More information? Please ,send to e-mail.
// Uno R3, A4:SDA,A5: SCL, Leonardo 2:SDA,3:SCL, Nano V3.0  A4:SDA,A5:SCL
// Register setting values are subject to change without prior notice to improve touch operation.

#include <Wire.h>

#define LF        0x0A //New Line
#define CR        0x0D //Carriage  return
#define SPC       0x20 //Space

#define Ch_enable    0x01 //Touch Channel Enable/Disable
#define Chip_id      0x48  //IC Chip ID
#define Clock_ctrl   0x34 //Clock Control Register (init_cal_opt, clk_sel, rb_sel)
#define Global_ctrl1 0x36 //Global Option Control Register1
                          //(response_off_ctrl, response_ctrl, bf_mode, Software Reet)
#define State_count 0x37 //Cal_pre_scale
#define Global_ctrl2 0x38 //Global Option Control Register2
                          //(imp_sel,Single/Multi ,Cal_Hold_time,clock_off)

// Sensitivity level (threshold, Register Value X 0.025% = (1 Step=0.025%)							
#define Sensitivity1  0x39 //ch1,Default: 0x1C X 0.025% = 0.70% (threshold)
#define Sensitivity2  0x3A //ch2
#define Sensitivity3  0x3B //ch3
#define Sensitivity4  0x3C //ch4
#define Sensitivity5  0x3D //ch5
#define Sensitivity6  0x3E //ch6
#define Sensitivity7  0x3F //ch7
#define Sensitivity8  0x40 //ch8

#define Cal_speed1   0x41 //Calibration Speed Control at BF mode
#define Cal_speed2   0x42 //Calibration Speed Control at BS mode

#define PWM_ctrl1    0x43 //LED PWM control Register (D2,D1)
#define PWM_ctrl2    0x44 //LED PWM control Register (D4,D3)
#define PWM_ctrl3    0x45 //LED PWM control Register (D6,D5)
#define PWM_ctrl4    0x46 //LED PWM control Register (D8,D7)

#define Port_Mode    0x4F //Select the output port operation mode of each channel.

#define  Unlock_op_en   0x64 //Register Lock Enable/Disable
#define  Reg_unlock     0x65 //Register Lock/unlock 
#define  Dread_unlock   0x66 //Register address 0x62h, 0x63h Lock/unlock
#define  BIST_unlock    0xA9 //BIST(Buli-In Self Test) Register address 0x67h ~0x77h Lock/unlock

#define Texp_Ctrl1      0x69 //Select the time base for touch expiration
#define Texp_Ctrl2      0x6A //Resolution Option for touch expiration time
#define Alarm2          0x8A //expiration time Status

#define Dtr_Ctrl1       0x6B //Set the Touch Percent Limit (bit 7~0)
#define Dtr_Ctrl2       0x6C //Set the Touch Percent Limit (bit 15~8)
#define Dtr_Ctrl3       0x6D //DTR Mode
#define Alarm2          0x8B //DTR Status

#define BIST_Start1     0x6E //BIST(Buli-In Self Test)
#define BIST_Stas1      0x79 //BIST(Buli-In Self Test)
#define BIST_Iresult    0x7C //BIST(Buli-In Self Test)

#define Output  0x2A //Touch Key Output Data Register

// ============= ANMG08D I2C Slave Address ============================//
#define ANMG08D_ID  0x24 //0x48 >>1( 0b0100100 7bit + R/W 1 Bit , 7bit=0x24, 8bit=0x48)
// ============= ANMG08D I2C Slave Address ============================//

void  Init_ANMG08D(void); //Initialize ANMG08D

#define RESET_PIN 7 //Reset pin
#define EN_PIN    6 //I2C Enable Pin

void Register_Dump()
{
   byte read_data[1] = {0};

   for (int i = 0; i < 256; i += 16)
   {
      for (int j = 0; j <= 15; j++)
      {
         Wire.beginTransmission(ANMG08D_ID);    // sned ic slave address
         Wire.write((i + j));                   // sends register address
         Wire.endTransmission();                // stop transmitting
         Wire.requestFrom(ANMG08D_ID, 1);       // data read (2 byte)
         read_data[0] = Wire.read();            //
         print2hex(read_data, 1);               //
      }
      Serial.write(LF);
      Serial.write(CR);
   }
   delay(500);
}

void print2hex(byte *data, byte length) //Print Hex code
{
   Serial.print("0x");
   for (int i = 0; i < length; i++)
   {
      if (data[i] < 0x10)
      {
         Serial.print("0");
      }
      Serial.print(data[i], HEX);
      Serial.write(SPC);
   }
}

void setup(){
  delay(200); //wait for 200[msec], Power on Reset

  Wire.begin();          // join i2c bus (address optional for master)
  Wire.setClock(200000); // 200Khz (200Kbps)
  Serial.begin(115200);  // start serial for output (Speed)
  //put your setup code here, to run once:

  pinMode(RESET_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  
  Init_ANMG08D(); //Initialize ANMG08D
  delay(100); //wait for 100[msec]
    
}
void loop() {

   byte read_data[2] = {0};

   Wire.beginTransmission(ANMG08D_ID); // sned ic slave address
   Wire.write(byte(Output));           // sends register address
   Wire.endTransmission();             // stop transmitting
   Wire.requestFrom(ANMG08D_ID, 1);    // key data read (1 byte)
   while ( Wire.available() )
    {
      read_data[0] = Wire.read();
   }
   //Wire.endTransmission(); // I2C Stop

   Serial.write(10);
   Serial.print("-------Touch Sensor Output Data  ---- > "); // Test Code
   delay(20);

   print2hex(read_data, 1);
   //Serial.print(read_data, HEX);
   //Serial.write(SP);
   //Serial.write(LF);
   //Serial.write(CR);

   delay(20);   
}

void  Init_ANMG08D(void)
{
   // ---------------- Register unlock Control Start----------------------------

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Unlock_op_en));     // 0x64h
   Wire.write(0x80);
   // 0x80 : Register unlock/unlock 
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Reg_unlock));       // 0x65h
   Wire.write(0xAA);
   // 0xAA : Register Unlock Enable (Address 0x00h ~ 0x61h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Reg_unlock));       // 0x65h
   Wire.write(0x55);
   // 0x55 : Register Unlock Enable (Address 0x00h ~ 0x61h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dread_unlock));     // 0x66h
   Wire.write(0xAA);
   // 0xAA : Register Unlock Enable (Address 0x62h, 0x63h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dread_unlock));     // 0x66h
   Wire.write(0x55);
   // 0x55 : Register unlock Enable (Address 0x62h, 0x63h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(BIST_unlock));     // 0xA9h
   Wire.write(0xAA);
   // 0xAA :  Register unlock Enable (Address 0x67h~0x77h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(BIST_unlock));     // 0xA9h
   Wire.write(0x55);
   // 0x55 :  Register unlock Enable (Address 0x67h~0x77h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Unlock_op_en));     // 0x64h
   Wire.write(0x01);
   // 0x01 : Register unlock/unlock
   Wire.endTransmission(); //

// ---------------- Register unlock Control End----------------------------
   
   //------------------ Software Reset Enable (Set)----------------------
   Wire.beginTransmission(ANMG08D_ID); // 
   Wire.write(byte(Global_ctrl1)); //  0x36h
   Wire.write(byte(0x49)); // Software Reset Enable, Response time ON =3, OFF=3
   Wire.endTransmission(); //

   // --------------- BIST Register Unlock ---------------------------------
   // Register Address 0x67h ~ 0x77h
   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(BIST_unlock));      // address 0x9Ah
   Wire.write(byte(0xAA));             // data, 0xAA
   Wire.endTransmission();             // 
   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(BIST_unlock));      // address 0x9Ah
   Wire.write(byte(0x55));             // data, 0x55
   Wire.endTransmission();             // 
   //-----------------------------------------------------------------------

   // --------------- Hidden Register Start ---------------------------------
   // user does not change the register. please contact us to change.
   // -----------------------------------------------------------------------
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x05)); // address
   Wire.write(byte(0x80)); // data
   Wire.endTransmission(); //   

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x08)); // address
   Wire.write(byte(0x11)); //0x08h
   Wire.write(byte(0x11)); //0x09h
   Wire.write(byte(0x11)); //0x0Ah
   Wire.write(byte(0x11)); //0x0Bh
   Wire.write(byte(0x11)); //0x0Ch
   Wire.write(byte(0x11)); //0x0Dh
   Wire.write(byte(0xA1)); //0x0Eh
   Wire.write(byte(0x10)); //0x0Fh   
   Wire.endTransmission(); //   

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x10)); // address
   Wire.write(byte(0xCE)); //0x10h // IC Reset Value: 0xF5 (-0.55%) ->0xCE (-2.55%)
   Wire.write(byte(0x20)); //0x11h
   Wire.write(byte(0xFF)); //0x12h
//Wire.write(byte(0x88));  //0x12h
//Change the value of the register 0x12h when a problem occurs due to voltage drop
   Wire.write(byte(0x92)); //0x13h
   Wire.write(byte(0x87)); //0x14h
//IC Reset Value: 0x83 (3ch) -> 0x87(7ch) , -% Hold Channel 3ch -> 7ch
// Set when there is a possibility of touching multiple touch pads due to the narrow spacing of the touch keypad.
   Wire.write(byte(0x73)); //0x15h
   Wire.write(byte(0x64)); //0x16h
   Wire.write(byte(0xFF)); //0x17h 
   Wire.write(byte(0x2B)); //0x18h  
   Wire.write(byte(0x11)); //0x19h
   Wire.write(byte(0x03)); //0x1Ah 
   // IC Reset Value: 0x00 ->0x03 (Init Fast Cal., Up Fast, Down Slow)
   Wire.write(byte(0xFF)); //0x1Bh  
   Wire.endTransmission(); //  

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x21)); // address 
   Wire.write(byte(0x00)); // data
   Wire.endTransmission(); // 

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x35)); // address
   Wire.write(byte(0x02)); // High  Frequency Band , -% Calibration Speed Fast (bit6~bit0)
   //Wire.write(byte(0xC0)); // Default Value (Low Frequency Band)
   //Wire.write(byte(0x40)); // High  Frequency Band
   //IC Reset Value = 0xC0 (Sensing Frequency Low) -> 0x40 (Sensing Frequency High)
   //Change the value to improve the Low-Frequency-Noise.
   Wire.endTransmission(); //   

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x47)); // address
   Wire.write(byte(0x0D)); // data
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x48)); // address
   Wire.write(byte(0x00)); // data
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x49)); // address
   Wire.write(byte(0x80)); // data
   Wire.endTransmission(); //    

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x4A)); // address
   Wire.write(byte(0x04)); // data
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(0x59));// 1st address 0x59h (0x59h~ 0x63h)
   Wire.write(byte(0x00)); //0x59h
   Wire.write(byte(0x00)); //0x5Ah
   Wire.write(byte(0x00)); //0x5Bh
   Wire.write(byte(0x00)); //0x5Ch
   Wire.write(byte(0x00)); //0x5Dh
   Wire.write(byte(0x02)); //0x5Eh
   Wire.write(byte(0x08)); //0x5Fh
   Wire.write(byte(0x02)); //0x60h
   Wire.write(byte(0x08)); //0x61h
   Wire.write(byte(0x00)); //0x62h
   Wire.write(byte(0x00)); //0x63h
   Wire.endTransmission(); // stop

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(0x67)); // address
   Wire.write(byte(0x01)); // data (SEN_INIT_OPTION bit=1)
   Wire.endTransmission(); //

   // --------------- Hidden Register End-------------------------------

   // ---------------- user code ---------------------//   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Ch_enable)); // 0x01h
   Wire.write(0xFF); // 0:Touch Key Disable, 1: Touch Key Enable
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Chip_id));          // 0x06h
   Wire.write(0x48);                   // Chip Address 0x48
   Wire.endTransmission();             //

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Clock_ctrl)); //  0x34h
   Wire.write(0x65); // //IC Reset value 0x05 -> 0x65
   Wire.endTransmission(); // 

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Global_ctrl1)); // 0x36h 
   Wire.write(0x4D); // 
   Wire.endTransmission(); // 

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(State_count)); // 0x37
   Wire.write(0xE6); // //IC Reset value 0xFF -> 0xE6 
   // Calibration Speed Pre-scaler , (bit4 ~ bit0)
   //The value can be changed according to the operating conditions of the set.
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Global_ctrl2)); // 0x38h
   Wire.write(0xBC); // Expire time Max set (0b1111 x 8sec Step = 120Sec)
   // Referencr to  Register Address 0x69h, 0x6Ah
   //Wire.write(0x80); // Expire time disable
   Wire.endTransmission(); //  
   
//------------ Sensitivity control  -----------------------------------
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity1)); // 0x39h
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //      

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity2)); // 0x3Ah
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity3)); // 0x3Bh
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity4)); // 0x3Ch
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity5)); // 0x3Dh
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity6)); // 0x3Eh
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity7)); // 0x3Fh
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Sensitivity8)); // 0x40h
   Wire.write(0x24); // HEX x 0.025 = 0.90%
   Wire.endTransmission(); //  
 
// ------------ Calibration Speed Control ------------------------ 
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Cal_speed1)); // 0x41h
   Wire.write(0x67); // Down Calibration Speed = Normal -> Slow
   //Wire.write(0x66); // Default Value 
   Wire.endTransmission(); //   
   
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Cal_speed2)); // 0x42h
   Wire.write(0x67); // Down Calibration Speed = Normal -> Slow
   //Wire.write(0x66); // Default Value
   Wire.endTransmission(); //   
// -------------------- LED PWM Control -----------------------------
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(PWM_ctrl1)); // 0x43h
   Wire.write(0x00); // D2,D1
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(PWM_ctrl2)); // 0x44h
   Wire.write(0x00); // D4,D3
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(PWM_ctrl3)); // 0x45h
   Wire.write(0x00); // D6,D5
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(PWM_ctrl4)); // 0x46h
   Wire.write(0x00); // D8,D7
   Wire.endTransmission(); //
// ---------------- Port Mode Control Register --------------------------
   Wire.beginTransmission(ANMG08D_ID);// 
   Wire.write(byte(Port_Mode)); // 0x4Fh
   Wire.write(0x00); // 0: Parallel output mode, 1: LED Drive mode
   Wire.endTransmission(); //

// ---------------- Output Expiration Contrl Register --------------------------
   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Texp_Ctrl1));       // 0x69h
   Wire.write(0x07);
   // 0x01 : Resolution Option for touch expiration time  00-1sec, 1-> time base system frepuency
   // Time set : Register address 0x38h -> Output expiration Time control bit
   // Time = 0x38h : 1111 x 0x69h Value : Resolution 1sec = 15sec
   // Wire.write(0x07);  // -> 0x38h Time 0b1111 x 8sec Resolution = 120sec
   Wire.endTransmission();             //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Texp_Ctrl2));       // 0x6Ah
   Wire.write(0xFF);                   
   // 0xFF : To channel seletion for the touch expiration function (8ch)
   // Wire.write(0x01); // if 0x01 = CS1 , 0xFF = CS8 ~ CS1
   Wire.endTransmission();             //

   // ---------------- DTR (Direct touch reset) Contrl Register --------------------------
   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dtr_Ctrl1));        // 0x6Bh
   //Wire.write(0x4E);                   // 0x4E : Set the Touch Nagative Percent Limit. -2.4%
   Wire.write(0x27);                   // 0x27 : Set the Touch Nagative Percent Limit. -1.2%
   Wire.endTransmission();             //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dtr_Ctrl2));        // 0x6Ch
   Wire.write(0x00);
   // Set the Touch -% Percent Limit.
   // (( 0x6Ch Value *256 + 0x6Bh Value) * 0.4) / (0x47h Value * -1) = Nagative %
   // Ex, 0x6Ch = 0x00, 0x6B = 0x4E, 0x47h = 0x0D, ((0 *256 + 78)*0.4) / (13 * -1) = -2.4%
   // Ex, 0x6Ch = 0x00, 0x6B = 0x27, 0x47h = 0x0D, ((0 *256 + 39)*0.4) / (13 * -1) = -1.2%
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dtr_Ctrl3));        // 0x6Dh
   Wire.write(0xF1);
   // DTR OP Mode
   Wire.endTransmission(); //

   //------------------ Software Reset Disable (Clear) ---------------------
   Wire.beginTransmission(ANMG08D_ID); // 
   Wire.write(byte(Global_ctrl1)); // 
   Wire.write(byte(0x48)); // Software Reset Disable, Response time ON =3, OFF=3
   Wire.endTransmission(); //

   // ---------------- Register Lock Control ---------------------------------
   // It is recommended to use it to prepare for communication errors. 
   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Unlock_op_en));     // 0x64h
   Wire.write(0x80);                   
   // 0x80 : Register Lock Enable (Address 0x00h ~ 0x63h)
   Wire.endTransmission();             //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Reg_unlock));       // 0x65h
   Wire.write(0x01);                   
   // 0x01 : Register Lock Enable (Address 0x00h ~ 0x61h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(Dread_unlock));     // 0x66h
   Wire.write(0x01);                   
   // 0x01 : Register Lock Enable (Address 0x62h, 0x63h)
   Wire.endTransmission(); //

   Wire.beginTransmission(ANMG08D_ID); //
   Wire.write(byte(BIST_unlock));      // 0xA9h
   Wire.write(0x01);                   
   // 0x01 : BIST Register Lock Enable (Address 0x67h ~ 0x77h)
   Wire.endTransmission(); //

   //================ END Initialize ANMG08D ==================================
   }

// End

