#include <ld3320.h>//语音识别库
#include<IRremote.h>//红外控制库

const int irReceiverPin=A1;//红外传感器引脚接模拟输入A1
const int voicePin=A0;//声音传感器引脚接模拟输入A0
const int ledPin7=7;//接绿色指示灯，灯亮代表进入音乐模式，灯灭代表退出音乐模式
const int ledPin8=8;//接红色和黄色指示灯，红灯亮开始校准红外传感器，黄灯亮代表开始正常工作
const int ledPin10=10;//定义LED控制引脚,用来控制灯珠亮度
const int redPin=3;
const int greenPin=5;
const int bluePin=6;//共阳极RGB全彩灯
const boolean invert=true;//阳极设置为ture
int color=0;//从0—255表示色调的值
int R,G,B;//红，绿，蓝
int brightness=0,brightness1=0;
double a=0;

const int numberOfKeys=10;//红外遥控可控按键数目，10键学习功能
long irKeyCodes[numberOfKeys];//保存每个按键的编码

IRrecv irrecv(irReceiverPin);//创建IR库
decode_results results;//IR数据放在这里

VoiceRecognition Voice;//声明一个语音识别对象

void setup() 
{
    Serial.begin(9600); //配置9600
    pinMode(ledPin7,OUTPUT);
    pinMode(ledPin8,OUTPUT);
    pinMode(ledPin10,OUTPUT);//声明ledPin为输出
    
    pinMode(irReceiverPin,INPUT);
    irrecv.enableIRIn();//启动红外接收器
    digitalWrite(ledPin8,HIGH);//红灯亮，黄灯灭
    learnKeycodes();//学习遥控器的键码
    digitalWrite(ledPin8,LOW);//黄灯亮，红灯灭
                
    Serial.print("Uart start!");                
    Voice.init();                               //初始化VoiceRecognition模块   
    //添加指令，参数（指令内容，指令标签（可重复））
    Voice.addCommand("kai",0);             
    Voice.addCommand("kai deng",0);        
    Voice.addCommand("guan",1);          
    Voice.addCommand("guan deng",1);         
    Voice.addCommand("shui jiao",1); 
    Voice.addCommand("liang",2); 
    Voice.addCommand("liang yi dian",2);
    Voice.addCommand("an",3); 
    Voice.addCommand("an yi dian",3); 
    Voice.addCommand("hong se",4);
    Voice.addCommand("hong",4);
    Voice.addCommand("lv se",5);
    Voice.addCommand("lv",5);
    Voice.addCommand("lan se",6);
    Voice.addCommand("lan",6);
    Voice.addCommand("yin yue",7);
    Voice.addCommand("miu rei ke",7);
    Voice.addCommand("tui chu",8);
    Voice.addCommand("ting",8);
    Voice.addCommand("hui fu",9);
    Voice.addCommand("fu yuan",9);
    //添加垃圾词汇
    Voice.addCommand("en",10);
    Voice.addCommand("heng",10);
    Voice.addCommand("yi yu",10);
    Voice.addCommand("yi",10);
    Voice.start();//开始识别
    Serial.print("Initialized\n");

   analogWrite(ledPin10,LOW);  //主控灯初始化时关闭  
   Serial.println("Press a remote key");    
}

boolean lightState=false;//跟踪音乐标志灯是否点亮
unsigned long last=millis();//记录最后一次收到一个控制开关音乐模式的IR消息的时间

void loop() 
{
  long key;
  int brightness2;
  
  int Voiceread=Voice.read();//读取声音传感器的值

  if(irrecv.decode(&results))//红外传感器接收到数据
  {
    irrecv.resume();
    key=convertCodeToKey(results.value);
    if(key>=0)
    {
      Serial.print("Got key ");
      Serial.println(key);
      if(key<5)
      {
        brightness2=map(key,0,4,0,255);
        analogWrite(ledPin10,brightness2);
      }//控制4个挡位的灯光
      else
      {
       switch (key)
      {
        case 5://音乐模式
          {
            if(millis()-last>250)//离上一条消息是否过了250ms
            {
              lightState=!lightState;//是，切换LED
              last=millis();
              Voiceread=7;
              break;
            }
          }
        case 6://红灯
          {
            Voiceread=4;
            break;
          }
        case 7://绿灯
          {
            Voiceread=5;
            break;
          }
        case 8://蓝灯
          {
            Voiceread=6;
            break;
          }
        case 9://熄灭
          {
            Voiceread=9;
            break;
          }
        }
      }
    }
  }

  switch(Voiceread)  //判断识别
  {
    case 0:                                     //若是指令“kai deng”
      if( brightness==0)
      {
        brightness=50;
        analogWrite(ledPin10,brightness);    
      }
      Serial.print("LED ON ");   
      Serial.println(brightness);
      break;
    case 1:                                     //若是指令“guan deng”
      if( brightness>0)
      {
        brightness=0;
        analogWrite(ledPin10,brightness);
      }
      Serial.print("LED OFF ");
      Serial.println(brightness);
      break;  
    case 2:                                    //提高亮度
      if( brightness<250)
      {
        brightness=brightness+50;
        analogWrite(ledPin10,brightness);
      }
      Serial.print("liang ");
      Serial.println(brightness);
      break;  
    case 3:                                    //降低亮度
      if( brightness>0)
      {
        brightness=brightness-50;
        analogWrite(ledPin10,brightness);
      }
      Serial.print("an ");
      Serial.println(brightness);
      break;  
    case 4:
      {
        bianse(0,255,0);//红灯
        analogWrite(redPin,R);
        analogWrite(greenPin,G);
        analogWrite(bluePin,B);
        Serial.println("hong se ");
        break;  
      }
     case 5:
      {
        bianse(0,255,2);//绿灯
        analogWrite(redPin,R);
        analogWrite(greenPin,G);
        analogWrite(bluePin,B);
        Serial.println("lv se ");
        break;  
      }
     case 6:
      {
        bianse(0,255,4);//蓝灯
        analogWrite(redPin,R);
        analogWrite(greenPin,G);
        analogWrite(bluePin,B);
        Serial.println("lan se ");
        break;  
      }
     case 9:
      {//熄灭
        analogWrite(redPin,255);
        analogWrite(greenPin,255);
        analogWrite(bluePin,255);
        Serial.println("fu yuan  ");
        break;  
      }
    case 7://音乐模式
      {
        digitalWrite(ledPin7,HIGH);//音乐指示灯亮
        while(Voice.read()!=8)//如果不退出音乐模式（音控）
        {
          if(irrecv.decode(&results))//如果不退出音乐模式（红外控）
          {
            irrecv.resume();
            key=convertCodeToKey(results.value);
            if(key>=0)
            {
              Serial.print("Got key ");
              Serial.println(key);
              if(key==5&& millis()-last>250)
              {
                break;
              }
            }
          }
        int val;
        val=analogRead(voicePin);   //从模拟口A0读取声音传感器的值
        a=val;
        a=(a/550)*255;
        brightness1=a;
        color=a;
        Serial.println(brightness1);
        hueToRGB(color,brightness1);//调用函数色调转换为RGB
        analogWrite(redPin,R);//把RGB的值写到引脚
        analogWrite(greenPin,G);
        analogWrite(bluePin,B);
        delay(150);
      }
     lightState=!lightState;
     last=millis();
     digitalWrite(ledPin7,LOW); 
     break;  
    }
    case 10:      //垃圾词汇                             
      Serial.println("la ji ci hui");   
      break;
    default:
      break;
  }    
}

void hueToRGB(int hue ,int brightness1)//把颜色转换为红绿蓝分量的函数
{
  unsigned int scaleHue=(hue*6);//沿着色轮的0到5区
  unsigned int segment=scaleHue/256;//区内位置
  unsigned int segmentOffset=scaleHue-(segment * 256);
  unsigned int complement=0;
  unsigned int prev=(brightness1*(255-segmentOffset))/256;
  unsigned int next=(brightness1*segmentOffset)/256;
  if(invert)
  {
    brightness1=255-brightness1;
    complement=255;
    prev=255-prev;
    next=255-next;
   }

  switch(segment)
  {
  case 0://红
    R=brightness1;
    G=next;
    B=complement;
    break;
  case 1://黄
    R=prev;
    G=brightness1;
    B=complement;
    break;
  case 2://绿
    R=complement;
    G=brightness1;
    B=next;
    break;
  case 3://青
    R=complement;
    G=prev;
    B=brightness1;
    break;
  case 4://蓝
    R=next;
    G=complement;
    B=brightness1;
    break;
   case 5://洋红
   default:
    R=brightness1;
    G=complement;
    B=prev;
    break;
  }
}

void bianse(int hue ,int brightness1,int co)//变色函数
{
  unsigned int scaleHue=(hue*6);
  unsigned int segment=scaleHue/256;
  unsigned int segmentOffset=scaleHue-(segment * 256);
  unsigned int complement=0;
  unsigned int prev=(brightness1*(255-segmentOffset))/256;
  unsigned int next=(brightness1*segmentOffset)/256;
  if(invert)
  {
    brightness1=255-brightness1;
    complement=255;
    prev=255-prev;
    next=255-next;
   }

  switch(co)
  {
    case 0://红
  R=brightness1;
  G=next;
  B=complement;
  break;
    case 1://黄
  R=prev;
  G=brightness1;
  B=complement;
  break;
    case 2://绿
  R=complement;
  G=brightness1;
  B=next;
  break;
    case 3://青
  R=complement;
  G=prev;
  B=brightness1;
  break;
    case 4://蓝
  R=next;
  G=complement;
  B=brightness1;
  break;
    case 5://洋红
    default:
  R=brightness1;
  G=complement;
  B=prev;
  break;
  }
}

void learnKeycodes()//获取遥控编码
{
  while(irrecv.decode(&results))
  {
    irrecv.resume();
  }
  Serial.println("Ready to learn remote codes");
  long prevValue=-1;
  int i=0;
  while(i<numberOfKeys)
  {
    Serial.print("press remote key");
    Serial.print(i);
    while(true)
    {
      if(irrecv.decode(&results))
      {
        if(results.value!=-1&&results.value!=prevValue)
        {
          showReceivedData();
          irKeyCodes[i]=results.value;
          i=i+1;
          prevValue=results.value;
          irrecv.resume();//接收下一个值
          break;
        }
         irrecv.resume();//接收下一个值
      }
    }
  }
  Serial.println("Lreaning complete");
}

int convertCodeToKey(long code)//将一个遥控协议代码转换为逻辑键码（或者-1，如果没有接收到任何数字）
{
    for(int i=0;i<numberOfKeys;i++)
    { 
      if(code==irKeyCodes[i])
      {
        return i;
      }
    }
    return -1;
}
   
void showReceivedData()//显示协议类型和值
{
  if(results.decode_type==UNKNOWN)
  {
    Serial.println("-Could not decode message");
  }
  else 
  {
    if(results.decode_type==NEC)
    {
       Serial.print("-decoded NEC:");
    }
    else if(results.decode_type==SONY)
    {
       Serial.print("-decoded SONY:");
    }
    else if(results.decode_type==RC5)
    {
       Serial.print("-decoded RC5:");
    }
    else if (results.decode_type==RC6)
    {
       Serial.print("-decoded RC6:");
    }
    else if (results.decode_type==DISH)
    {
       Serial.print("-decoded DISH:");
    }
    else if (results.decode_type==SHARP)
    {
       Serial.print("-decoded SHARP:");
    }
    else if (results.decode_type==PANASONIC)
    {
       Serial.print("-decoded PANASONIC:");
    }
    else if (results.decode_type==JVC)
    {
       Serial.print("-decoded JVC:");
    }
    else if (results.decode_type==SANYO)
    {
    Serial.print("-decoded SANYO:");
    }
    else if (results.decode_type==MITSUBISHI )
    {
       Serial.print("-decoded MITSUBISHI :");
    }
      Serial.print("hex value = ");
      Serial.println(results.value,HEX);
    }     
}
     
