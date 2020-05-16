#include <SoftwareSerial.h>

/*
    L293D connections
                                      ___
 enable   servo1= 5V arduino       1-|   |-16  5V arduino to feed L293D
 input1  = pin 9 arduino           2-| L |-15  Input4  = pin 6 arduino
 output1 = terminal1 servo1        3-| 2 |-14  Output4 = terminal2 servo2
 GND arduino                       4-| 9 |-13  GND arduino
 GND arduino                       5-| 3 |-12  GND arduino
 output2 = borne2 servo1           6-| D |-11  Output3 = terminal1 servo2
 input2  = pin 10 arduino          7-|   |-10  Input3  = pin 5 arduino
 positive terminal 9 volt battery  8-|___|-9   Enable2 = 5V arduino    
*/


/* 
 * ATTENTION: This program requires the following conditions in order to work:
 * 
 * 1. the wifi module has already been assigned a local ip address 
 *
 * 2. the wifi module has already saved the wifi SSID.
 *
 * The reason this initial setup was not included in this program is that
 * the wifi module is not very reliable the first time you try to configure it.
 * This problem becomes more evident if the wifi module is feed via the 3.3 volt arduino pin
 * instead of an external 3.3 volt source.
 * You can find more info about how to make this initial setup at  
 * https://hetpro-store.com/TUTORIALES/control-de-leds-pagina-web-arduino-leds/
 */

// arduino pins that send signals to the servomotor
int right1  =  9;   
int right2  = 10;
int left1   =  6;
int left2   =  5;

// wifiModule will be connected to pins 3,2 wich are RX and TX respectively
SoftwareSerial wifiModule(3,2);               

void ahead() {                              
  digitalWrite(right1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left1, HIGH);
  digitalWrite(left2, LOW);
}

void back(){
  digitalWrite(right1, LOW);                 
  digitalWrite(right2, HIGH);
  digitalWrite(left1, LOW);
  digitalWrite(left2, HIGH);
}

// left rear wheel spins backwards, right rear wheel spins ahead
// torque is created and cars turns left
void left(){                             
  digitalWrite(right1, HIGH);                
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(left2, HIGH);
}

// torque created in other direction
void right(){                               
  digitalWrite(right1, LOW);                 
  digitalWrite(right2, HIGH);
  digitalWrite(left1, HIGH);
  digitalWrite(left2, LOW);
}

void stop(){                                 
  digitalWrite(right1, LOW);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(left2, LOW);
}

void setup(){
  // these pins are set as output given they send signals to the servomotors
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);

  // start serial port to communicate with wifi module
  Serial.begin(9600);

  wifiModule.begin(9600);                     

  // start listening at port 80. It will act a as small webserver
  wifiModule.println("AT+CIPSERVER=1,80");     

}

void send(String html){         
  // before sending data back to the client, you need to execute
  // AT+CIPSEND=0,L where 0 is the connection id 
  // and L represents the size of the message (in bytes)
  wifiModule.print("AT+CIPSEND=0,");  
  wifiModule.println(html.length());

  if (wifiModule.find(">")){           // wifi module is ready to send
    wifiModule.println(html); 
    delay(10);               
  }
}

void serveWebPage(){
    // I have to send the page in parts, because the wifi module doesnt work
    // if i try to send everything in one go                              
    send("HTTP/1.1 200 OK\r\n");          
    send("Content-Type: text/html\r\n");
    send("Connection: close\r\n");
    send("\r\n\r\n");
    send("<!doctype html>");
    send("<html>");
    send("<head>");
    send("<meta name=\"description\" content=\""
                 "Web interface to RC car\">");
    send("<meta name=\"keywords\" content=\"html,css,javascript,arduino,"
                 "RC car\">");
    send("<meta charset=\"utf-8\">");
    send("<title>Control RC Arduino car</title>");
    send("</head>");
    send("<body>");
    send("<h1> Click a command button to control your car</h1>");
    send("<p>Christian 2017<p>");
    send("<div align=\"center\">");
    send("<div> <input type=\"button\" onclick=\"location.href='"
                 "http://xxx.xxx.xxx.xxx/m=1'\" value=\"ahead\" name=\"button1\""
                 "style=\"width:100px;height:50px;\"/></div>");
   
    send("<div>");
    send("<input type=\"button\" onclick=\"location.href='"
                 "http://xxx.xxx.xxx.xxx/m=2'\" value=\"left\" name=\"button2\" "
                 "style=\"width:100px;height:50px;\"/>");
                 
    send("<input type=\"button\" onclick=\"location.href='"
                 "http://xxx.xxx.xxx.xxx/m=3'\" value=\"stop\" name=\"button3\" "
                 "style=\"width:100px;height:50px;\"/>");
                 
    send("<input type=\"button\" onclick=\"location.href='"
                 "http://xxx.xxx.xxx.xxx/m=4'\" value=\"right\" name=\"button4\" "
                 "style=\"width:100px;height:50px;\"/>");
                 
    send("</div>");
    send("<div> <input type=\"button\" onclick=\"location.href='"
                 "http://xxx.xxx.xxx.xxx/m=5'\" value=\"back\" name=\"button5\"/ "
                 "style=\"width:100px;height:50px;\"/></div>");
                 
    send("</div>");
    send("</body>");
    send("</html>");
}

// main
void loop() {
  char command;
  boolean isClientConnected = false;
                                                   
  // The wifiModule has a 64 byte buffer for managing requests and replies to clients. 

  while (wifiModule.available()){
    if (wifiModule.find("GET")){                       
      delay(1000);   
      serveWebPage();   
      isClientConnected = true;
    }
  }

  /* 
   * if the clients click a command button in the webpage
   * it sends a get request with a query parameter representing the command the user wishes the car
   * to perform
   */

  while (isClientConnected){
    if ( wifiModule.find("m=") ){  
      delay(1000);
      command = wifiModule.read(); 
      
      switch (command){                     
        case 1:
          ahead();
          break;
        case 2:
          left();
          break;
        case 3:
          stop();
          break;
        case 4:
          right();
          break;
        case 5:
          back();
          break;
        default:
          stop();
      } 
    }
  }
}
