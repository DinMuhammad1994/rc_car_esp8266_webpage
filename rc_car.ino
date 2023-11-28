#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>



#define in1 D1
#define in2 D2
#define in3 D3
#define in4 D4

const int trigPin = D6;
const int echoPin = D5;
int f = 0;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
int distanceCm;
int distanceInch;


int Left = 0;
int Right = 0;
int Stop = 0;
int forward = 0;
int backwward = 0;


// WiFi settings
#define WIFI_MODE           2                     // 1 = AP mode, 2 = STA mode
#define SSID_AP             "RC CAR"    // for AP mode
#define PASSWORD_AP         "abcd1234"            // for AP mode
#define SSID_STA            "C4-Projexels"      // for STA mode
#define PASSWORD_STA        "321ytrewq"  // for STA mode

IPAddress local_ip(192, 168, 4, 1); //IP for AP mode
IPAddress gateway(192, 168, 4, 1); //IP for AP mode
IPAddress subnet(255, 255, 255, 0); //IP for AP mode
ESP8266WebServer server(80);
int robot_mode = 0; // Robot Drive Mode initially 0 (Stop)

// initialize
void setup() {
  Serial.begin(115200);
  Serial.println("RC CAR");


  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  robot_control(); // Stop Robot


  if (WIFI_MODE == 1) { // AP mode
    WiFi.softAP(SSID_AP, PASSWORD_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
  } else { // STA mode
    WiFi.begin(SSID_STA, PASSWORD_STA);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP()); //the IP is needed for connection in STA mode
  }




  // setup web server to handle specific HTTP requests
  server.on("/", HTTP_GET, handle_OnConnect);


  server.on("/forward", HTTP_GET, handle_forward);
  server.on("/backward", HTTP_GET, handle_backward);

  server.on("/left", HTTP_GET, handle_left);
  server.on("/right", HTTP_GET, handle_right);

  server.on("/stop", HTTP_GET, handle_stop);
  server.onNotFound(handle_NotFound);

  //start server
  server.begin();
  Serial.println("ROBOT WEBSERVER STARTED");
}

// handle HTTP requests and control car
void loop() {
  server.handleClient();
  robot_control();
  ultra();
}

// HTTP request: on connect
void handle_OnConnect() {
  robot_mode = 0;
  Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: stop car
void handle_stop() {
  robot_mode = 0;
  Serial.println("Stopped");
  server.send(200, "text/html", SendHTML());
}



// HTTP request: turn left
void handle_left() {
  robot_mode = 1;
  Serial.println("Move left...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn right
void handle_right() {
  robot_mode = 2;
  Serial.println("Move right...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn forward
void handle_forward() {
  robot_mode = 3;
  Serial.println("Move Forward...");
  server.send(200, "text/html", SendHTML());
}


// HTTP request: turn Backward
void handle_backward() {
  robot_mode = 4;
  Serial.println("Move Backward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: other
void handle_NotFound() {
  robot_mode = 0;
  Serial.println("Page error");
  server.send(404, "text/plain", "Not found");
}

// control car movement
void robot_control() {

  if  (distanceCm < 20 && f == 1 )
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    // Set Motor B backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
      
        f = 0;
  }

  switch (robot_mode) {
      Serial.print("robot mode:"); Serial.println(robot_mode);
    case 0: // stop robot

      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      // Set Motor B backward
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      break;


    case 1: // Robot move left

      Serial.println("left");
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);

      break;
    case 2: // Robot move right
      Serial.println("right");
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      break;


    case 3: // Robot move Forward

      Serial.println("Forward");

      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      f = 1;

      break;


    case 4: // Robot move backward
      Serial.println("back");

      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);

      break;


  }
}

// output HTML web page for user
String SendHTML() {
  String html = "<!DOCTYPE html>\n";
  html += "<html>\n";
  html += "<head>\n";
  html += "<title>Floor Cleaning Robot</title>\n";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<div align=\"center\">\n";
  html += "<h1 style=\"text-align: center;\"><u><strong>Floor Cleaning Robot</strong> </u></h1>\n";
  html += "<h3 style=\"text-align: center;\">Powered by MUET-KHP</h3>";
  html += "<br>\n";
  html += "<form method=\"GET\">\n";

  html += "<input type=\"button\" value=\"MOVE FORWARD\" onclick=\"window.location.href='/forward'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"MOVE BACKWARD\" onclick=\"window.location.href='/backward'\">\n";
  html += "<br><br>\n";

  html += "<input type=\"button\" value=\"MOVE LEFT\" onclick=\"window.location.href='/left'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"MOVE RIGHT\" onclick=\"window.location.href='/right'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"STOP\" onclick=\"window.location.href='/stop'\">\n";
  html += "</form>\n";
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";
  return html;
}

void ultra()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  delay(500);
}
