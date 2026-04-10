#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo myservo;
const int servoPin = 2;

const char* ssid     = "ccm666";
const char* password = "12345678";

ESP8266WebServer server(80);

int currentAngle = 0;
bool rotatingCW = false;
bool rotatingCCW = false;

void handleRoot() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    angle = constrain(angle, 0, 180);
    myservo.write(angle);
    currentAngle = angle;
    rotatingCW = false;
    rotatingCCW = false;
  }
  if (server.hasArg("cw")) {
    rotatingCW = true;
    rotatingCCW = false;
  }
  if (server.hasArg("ccw")) {
    rotatingCCW = true;
    rotatingCW = false;
  }
  if (server.hasArg("stop")) {
    rotatingCW = false;
    rotatingCCW = false;
  }

  String html = "<!DOCTYPE HTML><html>";
  html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP8266 舵机控制</title>";
  html += "<style>body{text-align:center;font-family:Arial;margin-top:50px;}h1{color:#2c3e50;}.angle{font-size:24px;margin:20px 0;color:#e74c3c;}button{font-size:20px;padding:15px 30px;margin:10px;background:#3498db;color:white;border:none;border-radius:8px;cursor:pointer;}button:hover{background:#2980b9;}.rotate{background:#e67e22;}</style>";
  html += "</head>";
  html += "<body>";
  html += "<h1>ESP8266 舵机控制器</h1>";
  html += "<div class='angle'>当前角度: " + String(currentAngle) + "°</div>";
  html += "<a href='/?angle=0'><button>0°</button></a>";
  html += "<a href='/?angle=90'><button>90°</button></a>";
  html += "<a href='/?angle=180'><button>180°</button></a><br><br>";
  
  html += "<button class='rotate' onmousedown='fetch(\"/?cw\")' onmouseup='fetch(\"/?stop\")' onmouseout='fetch(\"/?stop\")' ontouchstart='fetch(\"/?cw\")' ontouchend='fetch(\"/?stop\")'>🔄 按住正转</button>";
  html += "<button class='rotate' onmousedown='fetch(\"/?ccw\")' onmouseup='fetch(\"/?stop\")' onmouseout='fetch(\"/?stop\")' ontouchstart='fetch(\"/?ccw\")' ontouchend='fetch(\"/?stop\")'>🔄 按住反转</button>";
  
  html += "</body></html>";

  server.send(200, "text/html; charset=utf-8", html);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  myservo.attach(servoPin);
  myservo.write(0);
  currentAngle = 0;
  Serial.println("🔧 舵机初始化完成，复位到0°");

  WiFi.begin(ssid, password);
  Serial.print("🔄 正在连接WiFi: ");
  Serial.println(ssid);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(1000);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi连接成功！");
    Serial.print("📶 本地IP地址: ");
    Serial.println(WiFi.localIP());
    server.on("/", handleRoot);
    server.begin();
    Serial.println("🌐 服务器已启动，可通过IP访问");
  } else {
    Serial.println("\n❌ WiFi连接失败！请检查WiFi名称/密码/信号");
  }
}

void loop() {
  server.handleClient();

  if (rotatingCW && currentAngle < 180) {
    currentAngle++;
    myservo.write(currentAngle);
    delay(15);
  }
  if (rotatingCCW && currentAngle > 0) {
    currentAngle--;
    myservo.write(currentAngle);
    delay(15);
  }
}