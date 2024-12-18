
// bibliotecas ultilizadas
//#include <ESP32Servo.h> // para funcionamento do motor servo
#include <WiFi.h>
#include<PubSubClient.h>

// definição de portas ultilizadas do ESP32
#define TRIG_PIN_VAGA01 23
#define  ECHO_PIN_VAGA01 22
#define TRIG_PIN_VAGA02 21
#define  ECHO_PIN_VAGA02 19
#define LED_VAGA01 2
#define LED_VAGA02 4
//#define servo_in 13
//#define servo_out 12

//definição das variáveis
unsigned int duracao_01=0;
unsigned int distancia_01=0;
unsigned int duracao_02=0;
unsigned int distancia_02=0;
String vaga_01="Indefinida";
String vaga_02="Indefinida";

// definição dos servos
//Servo servoMotor_in;
//Servo servoMotor_out; 


//WIFI settings
const char* ssid="LIVE TIM_D5BD";
const char* password="KsGKwKJqQ7";  

//MQTT broker settings
const char* mqtt_server="broker.hivemq.com";
//const char* mqtt_username=" ";
//const char* mqtt_password=" ";
const int mqtt_port=1883;
const char* mqttTopic = "Vaga01";

WiFiClient espClient;                   //Cria um cliente WIFI
PubSubClient client(espClient);    //Cria um cliente MQTT utilizando um cliente WIFI

long lastMsg=0;   //Variavel para controlar o tempo das mensagens

void setup() {
  pinMode(TRIG_PIN_VAGA01, OUTPUT);
  pinMode(ECHO_PIN_VAGA01, INPUT);
  pinMode(TRIG_PIN_VAGA02, OUTPUT);
  pinMode(ECHO_PIN_VAGA02, INPUT);
  pinMode(LED_VAGA01, OUTPUT);
  pinMode(LED_VAGA02, OUTPUT);  
  //servoMotor_in.attach(servo_in); //definição do servo de entrada
  //servoMotor_out.attach(servo_out); //definição do servo de saida
  Serial.begin(115200); //definição da comunicação serial  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to");
  Serial.println("ssid");

  WiFi.begin(ssid, password); //Conecta-se a rede WIFI com as credenciais fornecidas

//Enquanto não conseguir conectar no WIFI
while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
}

//Depois de conectar
Serial.println("");
Serial.println("Wifi connected");
Serial.println("IP address:");
Serial.println(WiFi.localIP());

}

void(reconectar()){
  while(!client.connected()){
    Serial.println("Aguardando conexão");
    if(client.connect("ESP32client")){
      Serial.println("conectado ao servidor MQTT");
      client.subscribe("esp32/data");
      }else{
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println("Try again in 5 seconds");
        delay(5000);
        }    
  }
}

void loop() {
  if(!client.connected()){
    reconectar();              //Se o cliente MQTT não tiver conectado, tenta reconectar
  }
  client.loop();               //Permite o cliente MQTT lidar com eventos e mensagens
  long now=millis();
  

  digitalWrite(TRIG_PIN_VAGA01, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_VAGA01, LOW);
  duracao_01=pulseIn(ECHO_PIN_VAGA01, HIGH);
  distancia_01=duracao_01*0.017;
  //Serial.print("Distancia_01=");
  Serial.println(distancia_01);
  //Serial.print("cm");
  delay(500);  

  if(distancia_01>8){    
    digitalWrite(LED_VAGA01,LOW);
    vaga_01="Vaga01Livre";
    if(now-lastMsg>5000){
    lastMsg=now;
    client.publish("Vaga01","Livre");   //Publica a mensagem no tópico "esp32" 
  }
    vaga_01="Vaga01Livre";
  }else{
    digitalWrite(LED_VAGA01,HIGH);
    vaga_01="Vaga01Ocupada";
    if(now-lastMsg>5000){
    lastMsg=now;
    client.publish("Vaga01","Ocupada");   //Publica a mensagem no tópico "esp32"
    }
  }

  digitalWrite(TRIG_PIN_VAGA02,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_VAGA02,LOW);
  duracao_01=pulseIn(ECHO_PIN_VAGA02,HIGH);
  distancia_02=duracao_02*0.017;
  //Serial.println("Distancia_02=");
  Serial.println(distancia_02);
  //Serial.println("cm");
  delay(500);


  if(distancia_02>8){    
    digitalWrite(LED_VAGA02,LOW);
    vaga_02="Vaga02Livre";
  }else{
    digitalWrite(LED_VAGA02,HIGH);
    vaga_02="Vaga02Ocupada";
  }

}