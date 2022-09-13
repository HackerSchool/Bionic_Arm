/**
 * @file mao_e_pulso.ino
 * @author carmen e joão
 * @brief Um braço biónico que usa os pin's desde do número 6 até 10 para poder 
 *        controlar um braço biónico de um arduino nano. A maneira como ele 
 *        corre é através de interrupts, sendo que dado um sinal, idealmente 
 *        por um cartão, ele iria mover-se entre pedra, papel ou tesoura de 
 *        forma aleatória. Este projeto é muito usado em bancas, então 
 *        mantenção pode ser necessária para o seu operacionamento.
 * @version 0.1
 * @date long long time ago, in a nucleus far away
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Servo.h>
#include <Wire.h>

#define NUM_MOV 5 //número de movimentos conhecidos
#define ANGULO_MAX 60 //ângulo máximo de compressão do fio
#define ANGULO_MIN 0  //ângulo de à vontade
#define ANGULO_MAX_P 180 //ângulo máximo de compressão do fio
#define SERVO_POS_UM 0  //pin do primeiro servo, os outros vêm de seguida, logo, SERVO_POS_UM = [1, 9]
#define DELAY 0 //tempo de espera para rotacao dos servos
#define ENDERECO 8  //linha de informação de master-slave

//vetor de funções void
typedef void (*vect_funcoes_void)();

// estrutura dos servos da mão
typedef struct {
  int polegar;
  int indicador;
  int medio;
  int anelar;
  int mindinho;
  int pulso;
} MAO;

// polegar, indicador, medio, anelar, mindinho, pulso
int pwm_pins[6] = {3,5,6,9,10,11};

// declarações de variáveis globais
MAO mao;  //estrura que contem os dedos da mao
//vetor com funções que representão os movimentos conhecidos
vect_funcoes_void vect_movimentos_mao[NUM_MOV] = {&descanso, &pedra, &papel, &tesoura, &carmen}; 
Servo s_polegar, s_indicador, s_medio, s_anelar, s_mindinho, s_pulso;
int *movimento = 0;

// movimentos conhecidos
void pedra() {
  angulos_mao(ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, 0);
  Serial.println("pedra");
}
void papel() {
  angulos_mao(ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, 0);
  Serial.println("papel");
}
void tesoura() {
  angulos_mao(ANGULO_MAX, ANGULO_MIN, ANGULO_MIN, ANGULO_MAX, ANGULO_MAX, 0);
  Serial.println("tesoura");
}
void descanso() {
  angulos_mao(ANGULO_MAX / 2, ANGULO_MAX / 2, ANGULO_MAX / 2, ANGULO_MAX / 2, ANGULO_MAX / 2, 0);
  Serial.println("descnaso");
}
void carmen() {
  for (int j = 0; j < 5; j++) {
    angulos_mao(ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, 0);
    angulos_mao(ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, 0);
    Serial.println("carmen");
  }
}
/*void joao() {
  for (int j = 0; j < 5; j++) {
    angulos_mao(ANGULO_MAX, ANGULO_MIN, ANGULO_MIN, ANGULO_MAX, ANGULO_MIN);
    angulos_mao(ANGULO_MAX, ANGULO_MIN, ANGULO_MIN, ANGULO_MAX, ANGULO_MIN);
  }
  //angulos_mao(ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN, ANGULO_MIN);
  Serial.println("joao");
}*/


//funções auxiliares
void angulos_mao(int p, int i, int M, int a, int m , int P) {
  m += 10;
  int Maximo = max_value(p, i, M, a, m, P);
  for (int count = ANGULO_MIN; count < Maximo || count == ANGULO_MIN; count ++) {
    //recebe o movimento dependente se o angulo aumenta ou diminui, de cada sevo
    //o ângulo aumenta
    if (mao.polegar < p) mao.polegar++;
    //o ângulo diminui
    else if (mao.polegar > p)  mao.polegar--;
    if (mao.indicador < i) mao.indicador++;
    else if (mao.indicador > i)  mao.indicador--;
    if (mao.medio < M) mao.medio++;
    else if (mao.medio > M) mao.medio--;
    if (mao.anelar < a) mao.anelar++;
    else if (mao.anelar > a) mao.anelar--;
    if (mao.mindinho < m) mao.mindinho++;
    else if (mao.mindinho > m) mao.mindinho--;
    //if(mao.pulso < P) mao.pulso++;
    //else if(mao.pulso > P) mao.pulso--;
    mao.pulso = P;
    //info_mao();
    servos_mao();
  }
}
int max_value(int p, int i, int M, int a, int m, int P) {
  p = abs( mao.polegar - p);
  i = abs(mao.indicador - i);
  M = abs(mao.medio - M);
  a = abs(mao.anelar - a);
  m = abs(mao.mindinho - m);
  int aa = max(p, i);
  int bb = max(M, a);
  int cc = max(aa, bb);
  return max(cc, m);

}
void init_mao() {
  // papel, descanso, papel
  vect_movimentos_mao[2]();
  vect_movimentos_mao[0]();
  vect_movimentos_mao[2]();

}
void relax_mao() {
  int times = 1;
  for (int i = 0; i <  times * 5; i++) {
    angulos_mao(ANGULO_MAX * (((i + 5) % 5) == 0), ANGULO_MAX * (((i + 4) % 5) == 0), ANGULO_MAX * (((i + 3) % 5) == 0), ANGULO_MAX * (((i + 2) % 5) == 0), ANGULO_MAX * (((i + 1) % 5) == 0), ANGULO_MAX_P / 2);

  }
}
void info_mao() {
  Serial.print("Polegar: ");
  Serial.println(mao.polegar);
  Serial.print("Indicador: ");
  Serial.println(mao.indicador);
  Serial.print("Médio: ");
  Serial.println(mao.medio);
  Serial.print("Anelar: ");
  Serial.println(mao.anelar);
  Serial.print("Mindinho: ");
  Serial.println(mao.mindinho);
  Serial.print("Pulso: ");
  Serial.println(mao.pulso);
  Serial.println();
}

//atua quando a mão está pronta a ser utilizada
void servo_ready() {
  pinMode(13, OUTPUT);
  for (int i = 1; i <= NUM_MOV; i++) {
    digitalWrite(13, HIGH);
    delay(NUM_MOV / i);
    digitalWrite(13, LOW);
    delay(NUM_MOV / i);
  }
}


//funções relacionadas com os servos
void attach_servos() {
  /*s_polegar.attach(SERVO_POS_UM, ANGULO_MIN, ANGULO_MAX);
    s_indicador.attach(SERVO_POS_UM + 1, ANGULO_MIN, ANGULO_MAX);
    s_medio.attach(SERVO_POS_UM + 2, ANGULO_MIN, ANGULO_MAX);*/

  s_polegar.attach(pwm_pins[SERVO_POS_UM]);
  s_indicador.attach(pwm_pins[SERVO_POS_UM + 1]);
  s_medio.attach(pwm_pins[SERVO_POS_UM + 2]);
  s_anelar.attach(pwm_pins[SERVO_POS_UM + 3]);
  s_mindinho.attach(pwm_pins[SERVO_POS_UM + 4]);
  s_pulso.attach(pwm_pins[SERVO_POS_UM + 5]);
}
void detach_servos() {
  s_polegar.detach();
  s_indicador.detach();
  s_medio.detach();
  s_anelar.detach();
  s_mindinho.detach();
  s_pulso.detach();
}
void servos_mao() {
  if (s_polegar.read() != mao.polegar) {
    s_polegar.write(mao.polegar);
  }
  if (s_indicador.read() != mao.indicador) {
    s_indicador.write(mao.indicador);

  }
  if (s_medio.read() != mao.medio) {
    s_medio.write(mao.medio);
  }
  if (s_anelar.read() != mao.anelar) {
    s_anelar.write(mao.anelar);
  }
  if (s_mindinho.read() != mao.mindinho) {
    s_mindinho.write(mao.mindinho);
  }
  if (s_pulso.read() != mao.pulso) {
    s_pulso.write(mao.pulso);
  }
}

//interrupção para receber informação do master
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read();        // receive byte as a character
  }
  *movimento = (int)Wire.read();

  if (*movimento >= 0 && *movimento < NUM_MOV)
    vect_movimentos_mao[*movimento]();
}

void setup() {
  //Wire.begin(ENDERECO);
  //Wire.onReceive(receiveEvent);
  
  Serial.begin(9600);

  attach_servos();
  init_mao();
  servo_ready();
  //detach_servos();


}

void pulso_up(){
  angulos_mao(ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, 180);
  delay(15);
}

void pulso_down() {
  angulos_mao(ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, ANGULO_MAX, 0);
  delay(15);  
}

void loop() {
  if (Serial.available()>0)
  {
    int byteReceived = Serial.read();
    if (byteReceived == 0)
    {
      pulso_up(); delay(250); pulso_down(); delay(100);
      pulso_up(); delay(250); pulso_down(); delay(100);
      pulso_up(); delay(125);

      switch (random(3))
      {
        case 0: pedra(); break;
        case 1: papel(); break;
        case 2: tesoura(); break;
        default: break;
      }
    }
    

  }


}
