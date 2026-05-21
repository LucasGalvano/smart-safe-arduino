#include <LiquidCrystal.h>
#include <Servo.h>

// Definicao do LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, A1);

// Configuracao do Servo
Servo meuServo;
const int pinoServo = 9;
const int posicaoTrancado = 0;
const int posicaoAberto = 90;

// Pinos de Saida
const int ledVerde = 10;
const int ledVermelho = 13;
const int buzzer = 8;

// Sensor e Calibracao
const int pinoLDR = A0;
int luzAmbiente = 0; 
const int MARGEM_SEGURANCA = 50; 

// Pinos de Entrada
const int botao = 7;
const int botaoReset = 2;

// Configuracao da Senha
const int SENHA_MESTRE[3] = {2, 1, 3};
int senhaDigitada[3] = {0, 0, 0};
int indice = 0;
int tentativas = 0;

// Estados do Sistema
bool processandoValidacao = false;
bool bloqueado = false;
bool cofreAberto = false;

// Logica do Botao
int estadoBotao;
int ultimoEstadoBotao = HIGH;
unsigned long ultimoClique = 0;
int contadorCliques = 0;
const unsigned long debounce = 150;
const unsigned long timeout = 1500;

// Interrupcao
volatile bool resetar = false;

void resetSistema() {
  resetar = true;
}

// Funcoes de Audio
void somClique() { tone(buzzer, 1200, 80); }
void somReset() { tone(buzzer, 400, 200); }
void somErro() { tone(buzzer, 300, 200); delay(200); tone(buzzer, 200, 300); }
void somSucesso() { tone(buzzer, 800, 150); delay(200); tone(buzzer, 1200, 200); }

// Limpa apenas a segunda linha do LCD
void limparLinha2() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void mostrarMensagemInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COFRE TRANCADO");
  lcd.setCursor(0, 1);
  lcd.print("Insira a senha:");
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  meuServo.attach(pinoServo);
  meuServo.write(posicaoTrancado);

  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(pinoLDR, INPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(botaoReset, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(botaoReset), resetSistema, FALLING);

  lcd.print("Calibrando LDR...");
  delay(1000);
  luzAmbiente = analogRead(pinoLDR);
  
  mostrarMensagemInicial();
}

void loop() {
  int luzAtual = analogRead(pinoLDR);
  
  // Calcula o alvo e limita em 1023 (o máximo do Arduino)
  int alvoInvasao = luzAmbiente + MARGEM_SEGURANCA;
  if (alvoInvasao > 1023) {
      alvoInvasao = 1023; 
  }

  // Agora o print vai mostrar o valor limitado corretamente
  Serial.print("Luz atual: "); Serial.print(luzAtual);
  Serial.print(" | Alvo invasao: "); Serial.println(alvoInvasao);

  // A condicao de invasao agora usa o alvo limitado
  if (luzAtual >= alvoInvasao && !cofreAberto && !bloqueado && !processandoValidacao) {
    lcd.clear();
    lcd.print("INVASAO!");
    digitalWrite(ledVermelho, HIGH);
    somErro();
    bloqueado = true;
  }
  if (bloqueado) {
    digitalWrite(ledVermelho, (millis() % 400) < 200);
    if (!resetar) return; 
  }

  if (resetar) {
    somReset();
    indice = 0;
    contadorCliques = 0;
    ultimoClique = 0;
    ultimoEstadoBotao = HIGH;
    processandoValidacao = false;
    tentativas = 0;
    bloqueado = false;
    cofreAberto = false;
    for (int i = 0; i < 3; i++) senhaDigitada[i] = 0;

    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, LOW);
    meuServo.write(posicaoTrancado);

    lcd.clear();
    lcd.print("RECALIBRANDO...");
    delay(1000);
    luzAmbiente = analogRead(pinoLDR);

    resetar = false;
    mostrarMensagemInicial();
  }

  if (!processandoValidacao) {
    estadoBotao = digitalRead(botao);

    if (ultimoEstadoBotao == HIGH && estadoBotao == LOW) {
      if (millis() - ultimoClique > debounce) {
        // Se for o primeiro clique do digito, limpa a linha para tirar o "Insira a senha:"
        if (contadorCliques == 0) {
          limparLinha2();
        }
        
        contadorCliques++;
        ultimoClique = millis();
        somClique();
        
        lcd.setCursor(0, 1);
        lcd.print("Dig "); 
        lcd.print(indice + 1);
        lcd.print(": "); 
        lcd.print(contadorCliques);
      }
    }
    ultimoEstadoBotao = estadoBotao;

    if (contadorCliques > 0 && (millis() - ultimoClique > timeout)) {
      if (indice < 3) {
        senhaDigitada[indice] = contadorCliques;
        indice++;
        lcd.clear();
        lcd.print("DADO SALVO");
        delay(400);
        if (indice < 3) mostrarMensagemInicial();
      }
      contadorCliques = 0;
    }

    if (indice == 3) {
      processandoValidacao = true;
      validarSenha();
    }
  }
}

void validarSenha() {
  lcd.clear();
  lcd.print("Validando...");
  delay(800);

  bool senhaCorreta = true;
  for (int i = 0; i < 3; i++) {
    if (senhaDigitada[i] != SENHA_MESTRE[i]) {
      senhaCorreta = false;
      break;
    }
  }

  if (senhaCorreta) {
    somSucesso();
    lcd.clear();
    lcd.print("SENHA CORRETA");
    digitalWrite(ledVerde, HIGH);
    meuServo.write(posicaoAberto);
    cofreAberto = true;
    delay(4000);
    meuServo.write(posicaoTrancado);
    cofreAberto = false;
  } else {
    tentativas++;
    somErro();
    lcd.clear();
    lcd.print("SENHA ERRADA");
    lcd.setCursor(0, 1);
    lcd.print("Tenta: "); lcd.print(tentativas); lcd.print("/3");
    digitalWrite(ledVermelho, HIGH);
    delay(2000);

    if (tentativas >= 3) {
      lcd.clear();
      lcd.print("BLOQUEADO!");
      lcd.setCursor(0, 1);
      lcd.print("Aguarde 10s...");
      bloqueado = true;
      unsigned long inicioBloqueio = millis();
      while (millis() - inicioBloqueio < 10000) {
        digitalWrite(ledVermelho, (millis() % 400) < 200);
      }
      bloqueado = false;
      tentativas = 0;
    }
  }

  indice = 0;
  processandoValidacao = false;
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, LOW);
  if (!bloqueado) mostrarMensagemInicial();
}