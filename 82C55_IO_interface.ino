//cmpe127 lab14 Zhaoqin Li 012055706 
//operating 82C55 in MODE 1

const byte interruptPin = 2;
void AD_output() //set AD pins to output
{ 
  for (int i = 52; i > 37 ; i = i - 2) 
  {
    pinMode(i, OUTPUT);
  }
}
void AD_input()  //set AD pins to input 
{
for (int i = 52; i > 37 ; i = i - 2)
  {
    pinMode(i, INPUT);
  } 
}
int stage_1()
{
  digitalWrite(45, HIGH); //WR', WE'
  delay(3);
  digitalWrite(43, HIGH); //RD', OE'
  delay(3);
  digitalWrite(49, LOW); //ALE
  delay(3);
  digitalWrite(39, HIGH); //ACK'A
}
void Setpins(byte dat)
{  
  //byte variable = 0xFF;
  int bit_number[8]={0,0,0,0,0,0,0,0};
  for(int i = 7 ; i >-1 ; i--)
  {
  bit_number [i] = ((dat & (1 << i)) > 0) ? 1 : 0; 
  }
  int pin_position[8] = {38,40,42,44,46,48,50,52}; 
  for(int i = 7 ; i >=0 ; i--)
  {
    digitalWrite(pin_position[i],bit_number[i]);
  }
}
int Dataread()
{
  int res = 0;
  for (int i = 44; i > 37 ; i = i - 2)
  {
    res <<= 1;
    res |= digitalRead(i);
  }
  //Serial.print(res,BIN/*,HEX*/);
  return res;
}

/////Basic Output concept as MEM_WRITE///////
void IO_Output(byte addre, byte data){
 //activate certain register
  digitalWrite(51, LOW); //M/IO'
  delay(3);
  AD_output();
  stage_1();
  Setpins(addre);
  digitalWrite(49, HIGH); //ALE
  delay(3);
  digitalWrite(49, LOW); //ALE
  delay(3);
 ////////////////////////////
 //input control word (A output, B input)
  digitalWrite(45, LOW); //WR', WE': CPU output-> 8255-> device
  delay(3);
  Setpins(data);
  digitalWrite(45, HIGH); //WR', WE': CPU output-> 8255-> device
  delay(3);
  //digitalWrite(39, LOW); //ACK'A
  //delay(3);
  //digitalWrite(39, HIGH); //ACK'A
}

/////Basic Input concept as MEM_READ///////
int IO_Input(byte addre){
  //activate certain register (B)
  digitalWrite(51, LOW); //M/IO'
  delay(3); 
  AD_output();
  stage_1();
  Setpins(addre);
  digitalWrite(49, HIGH); //ALE
  delay(3);
  digitalWrite(49, LOW); //ALE
  delay(3);
 ////////////////////////////
 //read
  AD_input();
  digitalWrite(43, LOW); //RD', OE': device output ->8255 ->CPU
  delay(3);
  int res = Dataread();
  digitalWrite(43, HIGH); //RD', OE': device output ->8255 ->CPU
  delay(3);
  return res;
}

int good()
{
  //x=0
  //int x = 1;
  IO_Output(0xC3,0x04);
  Serial.print("  ISR has been called,");
  Serial.print("the key you pressed is: ");
  //if(start == true){ 
char key[16] = {'1','2','3','A',
                '4','5','6','B',
                '7','8','9','C',
                '*','0','#','D'};
//IO_Output(0xC3,0x82);
IO_Output(0xC0,0x07);   //give 0000 1110 to A7 ~ A0 
int c1 = IO_Input(0xC1);     //read from B3 ~ B0
//‘1’,‘2’,‘3’,‘A’?
for(int i = 0; i < 4; ++i) {
  if((c1&(1<<i)) == 0) {
    Serial.print(key[3-i]);
  }
}//Serial.print(" ");


//IO_Output(0xC3,0x82);
IO_Output(0xC0,0x0B);   //give 0000 1101 to A7 ~ A0 
int c2 = IO_Input(0xC1);     //read from B3 ~ B0
//‘4’,‘5’,‘6’,‘B’
for(int i = 0; i < 4; ++i) {
  if((c2&(1<<i)) == 0) {
    Serial.print(key[3-i+1*4]);
  }
}//Serial.print(" ");

//IO_Output(0xC3,0x82);
IO_Output(0xC0,0x0D);   //give 0000 1011 to A7 ~ A0 
int c3 = IO_Input(0xC1);     //read from B3 ~ B0
//‘7’,‘8’,‘9’,‘C’
for(int i = 0; i < 4; ++i) {
  if((c3&(1<<i)) == 0) {
    Serial.print(key[3-i+2*4]);
  }
}//Serial.print(" ");

//IO_Output(0xC3,0x82);
IO_Output(0xC0,0x0E);   //give 0000 0111 to A7 ~ A0 
int c4 = IO_Input(0xC1);     //read from B3 ~ B0
//‘*’,‘0’,‘#’,‘D’
for(int i = 0; i < 4; ++i) {
  if((c4&(1<<i)) == 0) {
    Serial.print(key[3-i+3*4]);
  }
}
delay(3);
Serial.println("");
//IO_Output(0xC3,0x05);
}

boolean start = true;
void setup() {
  //put your setup code here, to run once:
  Serial.begin(9600);
  //74ls138
  pinMode(51, OUTPUT); //M/IO'
  //74ls373
  pinMode(49, OUTPUT);//ALE
  //6116_SRAM
  pinMode(43, OUTPUT); //RD', OE': device output ->8255 ->CPU
  pinMode(45, OUTPUT); //WR', WE': CPU output-> 8255-> device

  pinMode(39, OUTPUT);  //ACK'A

  attachInterrupt(digitalPinToInterrupt(interruptPin), good, HIGH);

}

void loop(){
  IO_Output(0xC3,0x05);
  IO_Output(0xC3,0xA6);
  IO_Output(0xC0,0x00);
  Serial.println("no key has been pressed");
  //Serial.println("");
  //IO_Output(0xC3,0x06);
  delay (1000);
}
