// Importação de bibliotecas necessárias
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Altere para a senha e o cadastro do wifi
const char* ssid = "SHARE-RESIDENTE";
const char* password = "Share@residente";

const int output = 2;

String sliderValue = "0";

// estabelecendo as propriedades PWM
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const char* PARAM_INPUT = "value";

// Cria um objeto AsyncWebServer na porta 80
AsyncWebServer server(80);

// Página HTML que será carregada
const char index_html[] PROGMEM = R"rawliteral(
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>JOGUINHO</title>
</head>
<body>
    <h1>ACERTE O NUMERO</h1>
    <h2>Digite um numero de 1 a 100</h2>
    <input type="number" id="numchut"><button onclick="xazam()" id="chutbutt">CHUTAR</button>
    <div id="hint"></div>
    <script>
        function xazam(){
            var number = 48;
            var chute = document.getElementById("numchut").value;

            if(chute == number){
                document.getElementById("hint").innerHTML = "Numero correto! Parabens!";
                console.log(number, chute);
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/slider?value="+1, true);
                xhr.send();
            }
            else if(chute < number){
                document.getElementById("hint").innerHTML = "Chute um numero maior!";
                console.log(number, chute);
                document.getElementById("numchut").value = "";
            }
            else if(chute > number){
                document.getElementById("hint").innerHTML = "Chute um numero menor!";
                console.log(number, chute);
                document.getElementById("numchut").value = "";
            }
        }
    </script>
</body>
</html>
)rawliteral";


void setup(){
  // porta serial para propósito de debug e configuração de led
  pinMode(16, OUTPUT);
  Serial.begin(115200);
  
  //configura as funcionalidades de LED PWM
  ledcSetup(ledChannel, freq, resolution);
  
  //anexa o canal ao GPIO a ser controlado
  ledcAttachPin(output, ledChannel);
  
  ledcWrite(ledChannel, sliderValue.toInt());

  // Conecta ao wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Printa o ip local do esp
  Serial.println(WiFi.localIP());

  // rota para root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Manda uma requisição GET para <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      if(inputMessage == "1"){
        digitalWrite(16, HIGH);
        digitalWrite(17, HIGH);                          
      }
      else {
        digitalWrite(16, LOW);        
      }

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // inicia o server
  server.begin();
}
  
void loop() {
  
}