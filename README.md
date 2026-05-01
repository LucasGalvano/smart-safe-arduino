# 🔐 Cofre Inteligente com Arduino

## 📌 Descrição

Este projeto consiste em um sistema de cofre inteligente desenvolvido com Arduino, utilizando autenticação por sequência de cliques em botão.

O sistema integra sensores, atuadores e feedback visual e sonoro para simular um sistema real de segurança embarcado.

---

## ⚙️ Componentes Utilizados

* Arduino Uno
* Display LCD 16x2
* Servo Motor
* LED (verde e vermelho)
* Buzzer
* LDR (sensor de luminosidade)
* Botões (input e reset)
* Resistores
* Protoboard

---

## 🧠 Funcionamento

### 🔑 Entrada de senha

A senha é inserida através de múltiplos cliques em um botão, com timeout entre cada dígito.

### ✅ Validação

O sistema compara a sequência inserida com a senha pré-definida.

### 🔓 Acesso permitido

* LED verde acende
* Servo abre o cofre
* Som de confirmação é emitido

### ❌ Acesso negado

* LED vermelho acende
* Som de erro
* Contador de tentativas

### 🚨 Bloqueio

Após 3 tentativas incorretas, o sistema entra em modo bloqueado.

### 🌞 Sensor de invasão (LDR)

Caso seja detectada luz com o cofre fechado, o sistema interpreta como tentativa de invasão e ativa o bloqueio.

### 🔊 Feedback sonoro

* Clique de botão
* Reset
* Sucesso
* Erro

---

## 🔌 Diagrama de Ligações

<p align="center">
  <img src="imagens/Projeto IoT.png" width="700" height="650">
</p>

---

## ▶️ Como executar

1. Monte o circuito conforme especificado
2. Faça upload do código para o Arduino
3. Insira a senha através do botão
4. Observe o comportamento do sistema

---

## 📊 Conceitos Aplicados

* Interrupções
* Leitura de sensores analógicos
* Controle de atuadores
* Debounce de botão
* Lógica de estados

