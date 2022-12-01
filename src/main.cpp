#include <Arduino.h>

#include  <EEPROM.h>
#include  <SPI.h>
#include  <PS2Keyboard.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>


//--------------------------------------------------------------------INICIO VARIABLES DEL PROGRAMA--------------------------------------------------------------------------------------------

#include "variables.hpp"

//--------------------------------------------------------------------FIN VARIABLES DEL PROGRAMA-------------------------------------------------------------------------------------------------------


//#include <avr/wdt.h>




//--------------------------------------------------------------------INICIO DEGUG DEL CODIGO--------------------------------------------------------------------------------------------

// PARA HACER DEBUG QUITE LOS COMENTARIOS DE LA VARIABLE, CUANDO YA ESTE LISTO EL PROGRAMA COMENTE LA VARIABLE Y SUBA EL CODIGO

#define DEBUG "Debug" 


//--------------------------------------------------------------------FIN DEGUG DEL CODIGO-------------------------------------------------------------------------------------------------------


int cantidad_galones_validacion = 1000;


//--------------------------------------------------------------------INICIALIZACIÓN PROTOCOLO TCP IP-------------------------------------------------------------------------------------------------------
#include  <Ethernet.h>

// Configuracion del Ethernet Shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// arduino
IPAddress ip(192,168,1,180);
IPAddress gateway(192, 168, 1, 100);
IPAddress subnet(255, 255, 255, 0);
IPAddress servidor_dns(200,107,10,100);

EthernetClient cliente;

// Direccion del servidor
byte serv[] = { 192,168,1,158 };

//--------------------------------------------------------------------FIN INICIALIZACIÓN PROTOCOLO UDP-------------------------------------------------------------------------------------------------------


PS2Keyboard keyboard;

//--------------------------------------------------------------------INICIALIZACIÓN TECLADO MATRICIAL 4X4-------------------------------------------------------------------------------------------------------

const byte  filas             = 4;
const byte  columnas          = 4;
byte        pinesC[columnas]   = {29,31,33,35};
byte        pinesF[filas]      = {39,23,25,27};
 
char teclas[filas][columnas]  = 
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'%','0','=','D'}
};
 
Keypad teclado = Keypad(makeKeymap(teclas), pinesF, pinesC, filas, columnas);

//--------------------------------------------------------------------FIN INICIALIZACIÓN TECLADO MATRICIAL 4X4-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIALIZACIÓN LCD 20X4-------------------------------------------------------------------------------------------------------

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

//--------------------------------------------------------------------FIN INICIALIZACIÓN LCD 20X4-------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------INICIO DE CONFIGURACION ARDUINO MEGA-------------------------------------------------------------------------------------------------------
void setup() 
{
    keyboard.begin(DataPin, IRQpin, PS2Keymap_Spanish);             // INICIALIZACION DE TECLADO DE PC
    Ethernet.begin(mac, ip);                                        // INICIALIZACION CONEXION ETHERNET
    cliente.connect(serv, 80);                                      // CLIENTE SE CONECTA AL SERVIDOR POR EL ´PUERTO 80
    
        
    Serial.begin(115200);                                           // INICIO DE CONFIGURACION SERIAL
    lcd.begin();                                                    // INICIALIZAR LCD 20X4 
    
 
    mensaje_incio();                                                // MOSTRAR MENSAJE DE INICIO
    
    attachInterrupt(5, ENCODER_DATO, FALLING);                      // CONFIGURAR INTERRUPCION PIN 18 ENCODER           --->> ENTRADA PIN 18
    attachInterrupt(4, PIN_DISPENSADOR, FALLING);                   // CONFIGURAR PIN 19 DEL DISPENSADOR DE COMBUSTIBLE --->> ENTRADA PIN 19
    pinMode(buzzer, OUTPUT);                                        // CONFIGURAR EL PIN BUZZER COMO SALIDA             --->> SALIDA PIN 50
    pinMode(BOMBA, OUTPUT);                                         // CONFIGURAR EL PIN BOMBA COMO SALIDA              --->> SALIDA PIN 52
    num_estado=ESTADO_1();                                          // LLAMA FUNCION ESTADO 1

    pinMode(TriggerPin, OUTPUT);                                    // PIN TRIGGER COMO SALIDA
    pinMode(EchoPin, INPUT);                                        // PIN ECO COMO ENTRADA

//    wdt_disable();                                                  // DESACTIVAR WATCHDOG TIMER
    delay(5000);                                                    // ESPERAR QUE SE INICIE LA COMUNICACION CON EL TECLADO
//    wdt_enable(WDTO_8S);                                            // CONFIGURAR WATCHDOG TIMER
}

//--------------------------------------------------------------------FIN DE CONFIGURACION ARDUINO MEGA-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO PROGRAMA ARDUINO MEGA-------------------------------------------------------------------------------------------------------

void loop() 
{
 
//wdt_reset();                                                          // REINICIAR WATVHDOG TIMER


//*********************************************************************************INICIO SUPER USUARIO**************************************************************************************************
if(num_estado != 23 || num_estado == 23 )
{
        if(iteracion >= 50 || num_estado != 23){
        aux = 0;
        iteracion = 1;
        }
}

//dato = teclado_matricial() ;                                            // LEER EL VALOR INGRESADO POR TECLADO MATRICIAL Y CARGAR EN VARIABLE dato

  dato = 0;
  
  if (keyboard.available()) 
  {
    char c = keyboard.read();
    int  num = c;
    #ifdef DEBUG 
        Serial.print(c);
        Serial.print(' ');
        Serial.print(num);
        Serial.print(' ');
    #endif    
    dato = TecladoPC(num);
  }
  
//  && num_estado !=2
if(num_estado !=12 )                                                    // FUNCIONA EN TODOS LOS ESTADOS EXCEPTO CUANDO ESTÁ CARGANDO GALONES
{


  leer_dato_super_usuario(dato);                            // LEER EL INGRESO DE SUPER USUARIO * POR MAS DE 5 SEGUNDOS



  leer_dato_cambiar_clave(dato);                            // LEER EL INGRESO DE CAMBIAR CLAVE 3 POR MAS DE 10 SEGUNDOS
}

  if(pantalla==20)                                                      // CONDICION 1) pantalla=20 ES LA BANDERA PARA INGRESAR A SUPER USUARIO
  {
  borrar();  
  pantalla=ESTADO_SUPER_1();                                            // LLAMA AL ESTADO_SUPER_1 MENU: 1._VER # GALONES
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if(dato=='B' && pantalla==21)                                         // CONDICION 2) 
  {
  pantalla=ESTADO_SUPER_2();                                            // LLAMA AL ESTADO_SUPER_1 MENU: 2._ NUEVA CARGA COMBUSTIBLE  
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if(dato=='A' && pantalla==22)                                         // CONDICION 3)                                       
  {
  pantalla=ESTADO_SUPER_1();                                            // LLAMA AL ESTADO_SUPER_1 MENU: 1._VER # GALONES
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if( (dato=='F' && ((pantalla >=21 && pantalla<=25) || (pantalla==27 || pantalla==28 || pantalla==29 || pantalla==30)) ))                                      // CONDICION 5), COLOCADA AQUI PARA QUE LEA ANTES DE LA CONDICION 4 Y FUNCIONES CORRECTAMENTE
  { 
  // RESETAR TODOS LOS VALORES UTILIZADOS EN LAS FUNCIONES SUPER USUARIO
  borrar();  
  pantalla=ESTADO_1();                                                  // MUESTRA EL ESTADO 1
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if( (dato=='C' && pantalla==21) || (pantalla==23) )                   // CONDICION 4) 
  {
  tiempo_tanque = millis();
  
  if( tiempo_tanque-tiempo_tanque_aux >= interval){
    
    pantalla=ESTADO_SUPER_3();                                          // MOSTRAR EL NUMERO DE GALONES MEDIDOS POR EL SENSOR ULTRASONICO
    tiempo_tanque_aux = tiempo_tanque;
  }
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if( (dato=='C' && pantalla==22))                                      // CONDICION 6)
  {
  pantalla=ESTADO_SUPER_4();                                            // LLAMA AL INGRESO DE # FACTURA
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( (dato=='C' && pantalla==24 && contador_factura>=6))               // CONDICION 7)
  {
  pantalla=ESTADO_SUPER_5();                                            // LLAMA AL INGRESO DE # GALONES
  num_estado=pantalla;                                                  // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if( (dato=='C' && pantalla==25 && (Num_gal_valido>=0.25 && Num_gal_valido <= cantidad_galones_validacion )))        // CONDICION 8)                                                                                                                                                             
  {
  pantalla=ESTADO_SUPER_7();                    // LLAMA AL INGRESO DE CHOFER #1
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( (dato=='C' && pantalla==25 && (Num_gal_valido<0.25 || Num_gal_valido > cantidad_galones_validacion )))        // CONDICION 9)                                                                                                                                                             
  {
  borrar_galones();                             // BORRAR VARIABLES DE FUNCION GALONES
  ESTADO_SUPER_6();                             // LLAMA AL MUESTRA ERROR EN INGRESO GALONES
  pantalla=ESTADO_SUPER_5();                    // LLAMA AL INGRESO DE # GALONES PARA QUE REINGRESE LOS DATOS
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( dato=='B' && pantalla==27)                // CONDICION 10)                                                                                                                                                             
  {
  pantalla=ESTADO_SUPER_8();                    // LLAMA AL INGRESO DE CHOFER #2
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( dato=='A' && pantalla==28)                // CONDICION 11)                                                                                                                                                             
  {
  pantalla=ESTADO_SUPER_7();                    // LLAMA AL INGRESO DE CHOFER #1
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( (dato=='C' && pantalla==28) || (dato=='C' && pantalla==27))                // CONDICION 12)   OPERACION OR    CONDICION 13)                                                                                                                                                       
  {
  ordenar_parametros_super(pantalla);
  pantalla=ESTADO_SUPER_9();                    // LLAMA FUNCION ELEGIR RESPONSABLE #1
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( dato=='B' && pantalla==29)                // CONDICION 14)                                                                                                                                                             
  {
  pantalla=ESTADO_SUPER_10();                   // LLAMA AL INGRESO DE RESPONSABLE #2
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( dato=='A' && pantalla==30)                // CONDICION 15)                                                                                                                                                             
  {
  pantalla=ESTADO_SUPER_9();                    // LLAMA AL INGRESO DE RESPONSABLE #1
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( (dato=='C' && pantalla==29)  || (dato=='C' && pantalla==30))                // CONDICION 16) Y CONDICION 17)                                                                                                                                                             
  {
    if(dato=='C' && pantalla==30)
    {
    responsable_int=1;    
    }    
  pantalla=ESTADO_SUPER_11();                    // LLAMA A FUNCION MOSTRAR DATOS EN LCD
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if( (base_datos==0 && pantalla==31))          // CONDICION 18)                                                                                                                                                       
  {
  borrar_cedula();
  ESTADO_SUPER_12();                            // LLAMA FUNCION ENVIAR A BASE DE DATOS
  base_datos=1;                                 // BANDERA BASE DE DATOS ESPERA USADA PARA HACER UNA SOLA EJECUCION
  bandera_datos=1;
  base_datos_protocolo(chofer,factura_fila,Num_gal_valido);               // AGREGAR RESPONSABLE
  borrar();
  pantalla=ESTADO_1();                          // LLAMA FUNCION ESTADO 1
  num_estado=pantalla;                          // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

//*********************************************************************************FIN SUPER USUARIO**************************************************************************************************


  if( (teclado.getState()!=2 && num_estado==1 && dato!=0))          // CONDICION 15)                                                                                                                                                       
  { 
  num_estado=ESTADO_2();                                            // LLAMA FUNCION ESTADO 2
  }

comparacion_ced(num_estado, cedula_fila, bandera_cedula);           // FUNCION RETORNA EL VALOR DE LA bandera_cedula SOLO SE EJECUTA CUANDO num_estado==2 Y contador_cedula>=10 y dato=='C'
  
  if( (bandera_cedula==0 && num_estado==2 && dato=='C' && contador_cedula>=10))          // CONDICION 16)                                                                                                                                                       
  { 
  ESTADO_3();                                                       // LLAMA FUNCION ESTADO 3
  borrar_cedula();                                                  // LLAMA FUNCION BORRAR CEDULA
  num_estado=ESTADO_2();                                            // LLAMA FUNCION ESTADO 2
  }

  if( (bandera_cedula==1 && num_estado==2 && dato=='C' && contador_cedula>=10))          // CONDICION 17)                                                                                                                                                       
  { 
  num_estado=ESTADO_4();                                            // LLAMA FUNCION ESTADO 2
  }

comparacion_mov(num_estado, movil_fila, bandera_movil);

  if( (bandera_movil==0 && num_estado==4 && dato=='C' && contador_movil>=2))          // CONDICION 18)                                                                                                                                                       
  { 
  ESTADO_5();                                                       // LLAMA FUNCION ESTADO 5
  borrar_movil();                                                   // LLAMA FUNCION BORRAR MOVIL
  num_estado=ESTADO_4();                                            // LLAMA FUNCION ESTADO 2
  }

  if( (bandera_movil==1 && num_estado==4 && dato=='C' && contador_movil>=2))          // CONDICION 19)                                                                                                                                                       
  { 
  num_estado=ESTADO_6();                                            // LLAMA FUNCION ESTADO 6
  }

  if( (Num_gal_valido<0.25 || Num_gal_valido>500) && num_estado==6 && dato=='C' )          // CONDICION 20)                                                                                                                                                       
  { 
  ESTADO_7();                                                       // LLAMA FUNCION ESTADO 7
  borrar_galones();                                                 // BORRAR VARIABLES DE FUNCION GALONES
  num_estado=ESTADO_6();                                            // LLAMA FUNCION ESTADO 6
  }

  if( (Num_gal_valido>=0.25 && Num_gal_valido<=500) && num_estado==6 && dato=='C' )          // CONDICION 21)                                                                                                                                                       
  { 
  num_estado=ESTADO_8();                                            // LLAMA FUNCION ESTADO 8
  lee_clave_epprom();
  }
  
  if(contador_clave>=4 && num_estado==8 && dato=='C' )             // CONDICION 22)                                                                                                                                                       
  { 
  
  validar_clave(num_estado, contador_clave);                       // FUNCION VALIDAR CLAVE SOLO SE EJECUTA CUANDO num_estado==8 Y contador_clave >= 4
       
      if(clave_valida == 0)                                        // CONDICION 23)                                                                                                                                                       
      { 
      num_estado=ESTADO_9();                                       // LLAMA FUNCION ESTADO 9
      }

          if( num_estado==9 && cont_intentos_clave < 3 )            // CONDICION 24)                                                                                                                                                       
          { 
          borrar_clave();
          delay(2000);
          num_estado=ESTADO_8();                                   // LLAMA FUNCION ESTADO 8
          }  

          if( num_estado==9 && cont_intentos_clave >= 3 )          // CONDICION 25)                                                                                                                                                       
          { 
          ESTADO_10();                                             // LLAMA FUNCION ESTADO 10            
          delay(2000);
          borrar();
          num_estado=ESTADO_1();                                   // LLAMA FUNCION ESTADO 1
          }  
          
          if(clave_valida == 1)                                    // CONDICION 26)                                                                                                                                                       
          { 
          num_estado=ESTADO_11();                                // LLAMA FUNCION ESTADO 11   QUITAR PRECARGAR          
          }
  }    

    if(num_estado==11)                                       // CONDICION 27)                                                                                                                                                       
    {
    //delay(1000);                                              // evita falso flanco
    num_estado=ESTADO_12();                                  // LLAMA FUNCION ESTADO 12
    }

    if( (num_estado==12 && Bandera_final_carga == LOW && galones_cargados >= Num_gal_valido) || (num_estado==12 && Bandera_final_carga == LOW && dato=='C') || (num_estado==12 && Bandera_final_carga == HIGH) )          // CONDICION 28)                                                                                                                                                       
    {
    digitalWrite(BOMBA, LOW);                   
    num_estado=ESTADO_13();                                  // LLAMA FUNCION ESTADO 13 
    bandera_datos_gas=1;

#ifdef DEBUG 
    Serial.println("bandera datos gas");                      // bandera gas 
    Serial.println(bandera_datos_gas);                        // bandera gas 
#endif

    if(galones_cargados > 0)                                 // SOLO ENVIA DATOS SI EL NUMERO DE GALONES ES MAYOR A CERO    
    {
    base_datos_gas(Datos_nombre[bandera_nombre_ced], cedula_fila, galones_cargados, movil_fila);      // LLAMA A LA FUNCION ENVIAR DATOS A TABLA DE DESPACHO
    }
    borrar();
    num_estado=ESTADO_1();                                   // LLAMA FUNCION ESTADO 1                
    }

  

validar_cedula(dato, num_estado);                                       // FUNCION INGRESAR NUMERO DE CEDULA SOLO SE EJECUTA CUANDO num_estado==2 Y SE REGISTRA UN VALOR EN dato
ingreso_movil(dato, num_estado);                                        // FUNCION INGRESAR NUMERO DE MOVIL SOLO SE EJECUTA CUANDO num_estado==4 Y SE REGISTRA UN VALOR EN dato
ingreso_galones_normal(dato, num_estado);                               // FUNCION INGRESAR NUMERO DE GALONES SOLO SE EJECUTA CUANDO num_estado==6 Y SE REGISTRA UN VALOR EN dato
ingreso_clave(dato, num_estado);                                        // FUNCION INGRESAR CLAVE SOLO SE EJECUTA CUANDO num_estado==8 Y SE REGISTRA UN VALOR EN dato 
cargar_galones(num_estado);                                             // FUNCION INGRESAR GALONES SOLO SE EJECUTA CUANDO num_estado==12 Y galones_cargados HA CAMBIADO   
num_factura(dato, num_estado);                                          // FUNCION INGRESAR NUMERO DE FACTURA SOLO SE EJECUTA CUANDO num_estado==24 Y SE REGISTRA UN VALOR EN dato
ingreso_galones_super_usuario(dato, num_estado);                        // FUNCION INGRESAR NUMERO DE FACTURA SOLO SE EJECUTA CUANDO num_estado==24 Y SE REGISTRA UN VALOR EN dato                                           

    if(dato!=0)                                              // CONDICION 50)                                                                                                                                                       
    {
#ifdef DEBUG 
    Serial.println("CARGANDO TEMPORIZADOR"); 
#endif      
    tiempo_espera=millis();                                  // ACTUALIZA TEMPORIZADOR
    }
    
    if((num_estado!=1 && num_estado!=23 && num_estado!=12) && (millis()-tiempo_espera > 30000))      // CONDICION 51)                                                                                                                                                       
    {
#ifdef DEBUG 
    Serial.println("BORRANDO RESET INACTIVIDAD");            // BORRAR  
#endif       

    borrar();   
    num_estado=ESTADO_1();                                   // LLAMA FUNCION ESTADO 1   
    tiempo_espera=millis();                                  // ACTUALIZA TEMPORIZADOR
    }

//**************************************************************************************CAMBIO DE CLAVE****************************************************************************************  

  if(pantalla==40)                                          // CONDICION X) pantalla=40 ES LA BANDERA PARA INGRESAR A CAMBIAR CLAVE
  {
  borrar();  
#ifdef DEBUG 
  Serial.println("pantalla cambiar clave");
#endif    

  pantalla=ESTADO_CLAVE_41();                               // LLAMA A PANTALLA MOSTRAR ITEM 1
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if(pantalla==41 && dato=='B')                             // CONDICION 41) 
  {
  pantalla=ESTADO_CLAVE_42();                               // LLAMA A PANTALLA MOSTRAR ITEM 2
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if(pantalla==42 && dato=='A')                             // CONDICION 42) 
  {
  pantalla=ESTADO_CLAVE_41();                                // LLAMA A PANTALLA MOSTRAR ITEM 1
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if(pantalla==42 && dato=='C')                             // CONDICION 43) 
  {
  borrar();                                                 // BORRAR TODAS LAS VARIABLES  
  pantalla=ESTADO_1();                                      // LLAMA A PANTALLA INICIO
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if(pantalla==41 && dato=='C')                             // CONDICION 44)
  {  
  lee_clave_epprom();                                       // LEE LA CLAVE ESCRITA EN MEMORIA EPPROM
  pantalla=ESTADO_CLAVE_43();                               // LLAMA A PANTALLA INGRESE CLAVE
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if(Camb_clave==Datos_clave_validar && cont_clave>= 4 && pantalla==43 && dato=='C')                            // CONDICION 45)
  {
  pantalla=ESTADO_CLAVE_44();                               // LLAMA A PANTALLA INGRESE NUEVA CLAVE
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }

  if(Camb_clave!=Datos_clave_validar && cont_clave>= 4 && pantalla==43 && dato=='C')                            // CONDICION 46)
  {
  pantalla=ESTADO_CLAVE_45();                               // LLAMA A PANTALLA OPERACIÓN FALLIDA
  borrar();                                                 // BORRAR TODAS LAS VARIABLES  
  pantalla=ESTADO_1();                                      // LLAMA A PANTALLA INICIO
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }  

  if(nueva_cont_clave>= 4 && pantalla==44 && dato=='C')                            // CONDICION 47)
  {
  pantalla=ESTADO_CLAVE_46();                               // LLAMA A PANTALLA INGRESE NUEVA CLAVE
  nueva_clave_1=nueva_Camb_clave;
  borrar_ingrese_nueva_clave();
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  } 

  if(nueva_cont_clave>= 4 && pantalla==46 && dato=='C')                            // CONDICION 48)
  {
  pantalla=ESTADO_CLAVE_47();                               // LLAMA A PANTALLA COMPARAR CLAVES
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  } 

  if(nueva_clave_1 == nueva_clave_2 && pantalla==47)                  // CONDICION 49)
  {
  pantalla=ESTADO_CLAVE_48();                               // LLAMA A PANTALLA CLAVE SE CAMBIO CORRECTAMENTE
  escribir_password();
  delay(2000);
  borrar();
  pantalla=ESTADO_1();                                      // LLAMA A PANTALLA INICIO
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  }
  
  if(nueva_clave_1 != nueva_clave_2 && pantalla==47)                  // CONDICION 50)
  {
  pantalla=ESTADO_CLAVE_49();                               // LLAMA A PANTALLA OPERACIÓN FALLIDA
  delay(2000);
  borrar();
  pantalla=ESTADO_1();                                      // LLAMA A PANTALLA INICIO
  num_estado=pantalla;                                      // GUARDA EL VALOR DE pantalla EN num_estado PARA SABER EN QUE PANTALLA ESTA
  } 

antigua_clave(dato, num_estado);  
ingrese_nueva_clave(dato, num_estado);
}


//*********************************************************************************FIN SUPER USUARIO**************************************************************************************************


//--------------------------------------------------------------------INICIO FUNCIÓN CARGAR GALONES-------------------------------------------------------------------------------------------------------

void cargar_galones(int num_estado_gal)
{
      if((Contador_final >= Contador_aux+0.1 ) && num_estado_gal==12)     
      {                  
      //galones_cargados=((Contador_interrup)/160);                     //cambiar aqui-----------------------------  
      (Contador_interrup<=2) ? galones_cargados=0 : galones_cargados=0.006315*Contador_interrup-0.004779;                         // comparison that avoid two incorrect values  
      //galones_cargados=0.006315*Contador_interrup-0.004779;             // NUEVA ECUACION
      
      Contador_final=Contador_aux;
      delay(10);                                                        // RETARDO PARA OBSERVAR CAMBIOS  
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("CARGANDO GALONES"));
      lcd.setCursor(0, 1);
      lcd.print(Num_gal_valido);
      
          if(Num_gal_valido<10)
          {   
          lcd.setCursor(4, 1);
          lcd.print(F("="));
          lcd.setCursor(5, 1);      
          lcd.print(galones_cargados); 
          }
              if(Num_gal_valido>=10 && Num_gal_valido<100)
              {   
              lcd.setCursor(5, 1);
              lcd.print(F("="));
              lcd.setCursor(6, 1);      
              lcd.print(galones_cargados);
              }
              if(Num_gal_valido>=100 && Num_gal_valido<=500)
              {
              lcd.setCursor(6, 1); 
              lcd.print(F("="));                    
              lcd.setCursor(7, 1);      
              lcd.print(galones_cargados);                
              }

      }
}    
//--------------------------------------------------------------------INICIO FUNCIÓN CARGAR GALONES-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCIÓN INGRESAR CLAVE-------------------------------------------------------------------------------------------------------

void ingreso_clave(char dato_clave, int num_estado_clave)
{
    if((dato_clave=='1' || dato_clave=='2' || dato_clave=='3' || dato_clave=='4' || dato_clave=='5' || dato_clave=='6'|| dato_clave=='7' || dato_clave=='8' || dato_clave=='9' || dato_clave=='0'|| dato_clave=='=') && num_estado_clave==8)                           // PRUEBA VALIDACION CEDULA 
    {  
        if((dato_clave=='=' && (contador_clave>0 && contador_clave<=4)))
        {
        cod_clave_ing[contador_clave-1]=0;  
        contador_clave--;
        borrar_lcd=1;
        }
           
        if((dato_clave=='=' && contador_clave==5 ))
        {
        cod_clave_ing[contador_clave-2]=0;
        contador_clave=3;
        borrar_lcd=1;   
        }
    
        if(dato_clave!='=' && (contador_clave>=0 && contador_clave<=3))
        {
        cod_clave_ing[contador_clave]=dato_clave;
        contador_clave++;
        borrar_lcd=0;  
        }

       Datos_clave=String(cod_clave_ing[0])+String(cod_clave_ing[1])+String(cod_clave_ing[2])+String(cod_clave_ing[3]);  
       Datos_clave_validar=String(cod_clave[0])+String(cod_clave[1])+String(cod_clave[2])+String(cod_clave[3]);

        if(contador_clave >= 0 && contador_clave < 5)
        {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(Datos_clave); 
        }
        
        if(contador_clave==1 && borrar_lcd==0)
        {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("*"));       
        }

        if(contador_clave==1 && borrar_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("*"));               
        }
        
        if(contador_clave==2  && borrar_lcd==0)
        {
        lcd.setCursor(0, 2);
        lcd.print(F("*"));           
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("**"));       
        }
        
        if(contador_clave==2 && borrar_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("**"));               
        }
        
        if(contador_clave==3  && borrar_lcd==0)
        {
        lcd.setCursor(0, 2);
        lcd.print(F("**"));            
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("***"));       
        }

        if(contador_clave==3 && borrar_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("***"));               
        }        
        
        if(contador_clave>=4  && borrar_lcd==0)
        {
        lcd.setCursor(0, 2);
        lcd.print(F("***"));              
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
        lcd.setCursor(0, 1);
        lcd.print(Datos_nombre[bandera_nombre_ced]); 
        lcd.setCursor(0, 2);
        lcd.print(F("****"));       
        }

        if(contador_clave>=4)
        {
        contador_clave=5;
        }
    }
}    
//--------------------------------------------------------------------INICIO FUNCIÓN INGRESAR CLAVE-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN VALIRAR CLAVE Y NUMERO DE INTENTOS-------------------------------------------------------------------------------------------------------
void validar_clave(int num_estado_val_clave, int contador_clave_val_clave)
{  
    if((num_estado_val_clave == 8) && (Datos_clave != Datos_clave_validar) )
    {
      cont_intentos_clave++;
      clave_valida=0;
    }
    
    if(Datos_clave == Datos_clave_validar)
    {
    clave_valida=1;  
    }    
}
//--------------------------------------------------------------------FIN FUNCIÓN VALIRAR CLAVE Y NUMERO DE INTENTOS-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN BORRAR CLAVE-------------------------------------------------------------------------------------------------------
void borrar_clave()
{  

#ifdef DEBUG 
    Serial.println("borrar_clave");                                        // BORRAR  
#endif   
  Datos_clave[0]           = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION borrar_clave****
  Datos_clave[1]           = 0;                                          //**********************************************************
  Datos_clave[2]           = 0;                                          //**********************************************************
  Datos_clave[3]           = 0;                                          //**********************************************************
  Datos_clave_validar[0]   = 0;                                          //**********************************************************
  Datos_clave_validar[1]   = 0;                                          //**********************************************************
  Datos_clave_validar[2]   = 0;                                          //**********************************************************
  Datos_clave_validar[3]   = 0;                                          //**********************************************************
  cod_clave_ing[0]         = 0;                                          //**********************************************************
  cod_clave_ing[1]         = 0;                                          //**********************************************************
  cod_clave_ing[2]         = 0;                                          //**********************************************************
  cod_clave_ing[3]         = 0;                                          //**********************************************************     
  borrar_lcd               = 0;                                          //**********************************************************
  clave_valida             = 0;                                          //**********************************************************
  contador_clave           = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION borrar_clave******** 
}
//--------------------------------------------------------------------FIN FUNCIÓN BORRAR CLAVE-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCIÓN VALIDAR GALONES USUARIO NORMAL-------------------------------------------------------------------------------------------------------

void ingreso_galones_normal(char dato_galones, int num_estado_galones)
{  
    if( ((dato_galones=='1' || dato_galones=='2' || dato_galones=='3' || dato_galones=='4' || dato_galones=='5' || dato_galones=='6'|| dato_galones=='7' || dato_galones=='8' || dato_galones=='9' || dato_galones=='0'|| dato_galones=='=' || dato_galones=='%') && num_estado_galones==6))                           // PRUEBA VALIDACION GALONES USUARIO NORMAL 
    {  

        if((dato_galones=='=' && (contador_galones>0 && contador_galones<7)) || (dato_galones=='=' && contador_galones==7 ))
        {
        galones[contador_galones-1]=0;  
        contador_galones--;

            if((dato_galones=='=' && contador_galones==6 ))
            {
            galones[contador_galones-1]=0;
            contador_galones=5;   
            }
        }
    
                if(dato_galones!='=' && (contador_galones>=0 && contador_galones<=6))
                {
                    if(dato_galones=='%')
                    {
                    dato_galones='.';      
                    }
                galones[contador_galones]=dato_galones;
                contador_galones++;  
                }

    gal_fila=String(galones[0])+String(galones[1])+String(galones[2])+String(galones[3])+String(galones[4])+String(galones[5]);  
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE GALONES:"));
    lcd.setCursor(0, 3); 
    lcd.print(gal_fila);
    Num_gal_valido=gal_fila.toFloat(); 

                          if(contador_galones>=6)
                          {
                          contador_galones=7;
                          }
    }
}
//--------------------------------------------------------------------FIN FUNCIÓN VALIDAR GALONES USUARIO NORMAL-------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------INICIO FUNCIÓN VALIDAR MOVIL-------------------------------------------------------------------------------------------------------

void ingreso_movil(char dato_movil, int num_estado_movil)
{
    if((dato_movil=='1' || dato_movil=='2' || dato_movil=='3' || dato_movil=='4' || dato_movil=='5' || dato_movil=='6'|| dato_movil=='7' || dato_movil=='8' || dato_movil=='9' || dato_movil=='0'|| dato_movil=='=') && num_estado_movil==4)                           // PRUEBA VALIDACION CEDULA 
    {  
        if((dato_movil=='=' && (contador_movil>0 && contador_movil<2)))
        {
        movil[contador_movil-1]=0;  
        contador_movil--;
        }
            
        if((dato_movil=='=' && contador_movil==3 ))
        {
        movil[contador_movil-2]=0;
        contador_movil=1;   
        }
    
        if(dato_movil!='=' && (contador_movil>=0 && contador_movil<=1))
        {
        movil[contador_movil]=dato;
        contador_movil++;  
        }

    movil_fila=String(movil[0])+String(movil[1]);  
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE MOVIL: "));
    lcd.setCursor(0, 3); 
    lcd.print(movil_fila); 
#ifdef DEBUG 
       Serial.print("movil =");
       Serial.println(movil_fila.toFloat()); 
#endif     

    

        if(contador_movil>=2)
        {
        contador_movil=3;
        }
    }
}    
//--------------------------------------------------------------------INICIO FUNCIÓN VALIDAR MOVIL-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN COMPARAR CEDULAS-------------------------------------------------------------------------------------------------------

void comparacion_mov(int num_estado_mov, String fila_mov, int bandera_movil_aux)
{

  if(num_estado_mov==4 && contador_movil>=2 && bandera_movil_aux!=1 && dato=='C')
  {
            for (int i=0; i <=sizeof(movil_puntero)/2; i++)
            {
               if(fila_mov==movil_puntero[i])
               {
               bandera_movil=1;
#ifdef DEBUG 
               Serial.println("Movil correcto"); 
#endif               
               }               
            } 
  }
  if(contador_movil<2)
  {
   bandera_movil=0; 
  }  
}
//--------------------------------------------------------------------FIN FUNCIÓN COMPARAR CEDULAS-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN BORRAR MOVIL-------------------------------------------------------------------------------------------------------
void borrar_movil()
{  
#ifdef DEBUG 
  Serial.println("borrar_movil");                                        // BORRAR  
#endif  
  movil[0]                 = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION validar_movil*****
  movil[1]                 = 0;                                          //**********************************************************
  movil_fila               = "";                                         //**********************************************************
  contador_movil           = 0;                                          //**********************************************************
  bandera_movil            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION validar_movil********
}
//--------------------------------------------------------------------FIN FUNCIÓN BORRAR MOVIL-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN VALIDAR CEDULA-------------------------------------------------------------------------------------------------------

void validar_cedula(char dato_ced, int num_estado_ced)      // FUNCION VALIDAR CEDULA CON DOS DATOS DE ENTRADA dato, num_estado 
{
                                                                              // CONDICION PARA INGRESAR SOLO NUMEROS Y PODER BORRAR, DESDE EL ESTADO 2 
    if(((dato_ced=='1' || dato_ced=='2' || dato_ced=='3' || dato_ced=='4' || dato_ced=='5' || dato_ced=='6'|| dato_ced=='7' || dato_ced=='8' || dato_ced=='9' || dato_ced=='0'|| dato_ced=='=') && num_estado_ced==2))                           // PRUEBA VALIDACION CEDULA 
    {  
                                                                              // CONDICION PARA INGRESAR A BORRAR NUMEROS DE CEDULA Y MANEJAR INDEXADO  
        if(((dato=='=' && (contador_cedula>0 && contador_cedula<11)) || (dato=='=' && contador_cedula==11 )))
        {
        cedula[contador_cedula-1]=0;  
        contador_cedula--;
                                                                              // CONDICION PARA BORRAR NUMEROS DESDE EL ULTIMO DIGITO 
              if(dato=='=' && contador_cedula==10)
              {
              cedula[contador_cedula-1]=0;
              contador_cedula=9;   
              }
         }
                                                                              // CONDICION PARA INGRESAR NUMEROS Y MANEJAR INDEXADO 
                    if((dato!='=' && (contador_cedula>=0 && contador_cedula<=10)))
                    {
                    cedula[contador_cedula]=dato;
                    contador_cedula++;
                    }
                                                                              // MOSTRAR NUMERO INGRESADOS EN LCD 
    cedula_fila=String(cedula[0])+String(cedula[1])+String(cedula[2])+String(cedula[3])+String(cedula[4])+String(cedula[5])+String(cedula[6])+String(cedula[7])+String(cedula[8])+String(cedula[9]); 
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE # DE CEDULA"));
    lcd.setCursor(0, 3);
    lcd.print(cedula_fila); 
                                                                              // CONDICION PARA GENERAR EL FIN DEL INGRESO DE CEDULA, BANDERA 
                          if(contador_cedula>=10)
                          {
                          contador_cedula=11;
                          }
    }

}
//--------------------------------------------------------------------FIN FUNCIÓN VALIDAR CEDULA-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN COMPARAR CEDULAS-------------------------------------------------------------------------------------------------------

void comparacion_ced(int num_estado_aux, String cedula_fila_aux, int bandera_cedula_aux)
{

  if(num_estado_aux==2 && contador_cedula>=10 && bandera_cedula_aux!=1 && dato=='C')
  {
            for (int i=0; i <=sizeof(comparar)/2; i++)
            {
               String myString = String(comparar[i]);
#ifdef DEBUG 
               Serial.print("i = "); 
               Serial.println(i); 
               Serial.print("Cedula aux = "); 
               Serial.println(cedula_fila_aux); 
               
               Serial.print("Cedula comparada = "); 
               Serial.println(myString); 
#endif              

               if(cedula_fila_aux == myString)
               {
               bandera_cedula     = 1;
               bandera_nombre_ced = i;
#ifdef DEBUG 
               Serial.println("Cedula correcta"); 
#endif               
               }               
            } 
  }
  if(contador_cedula<10)
  {
   bandera_cedula=0; 
  }  
}
//--------------------------------------------------------------------FIN FUNCIÓN COMPARAR CEDULAS-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCIÓN BORRAR CÉDULA-------------------------------------------------------------------------------------------------------
void borrar_cedula()
{  
#ifdef DEBUG 
  Serial.println("borrar_cedula");                                       // BORRAR
#endif  
  cedula[0]                = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION validar_cedula****
  cedula[1]                = 0;                                          //*******************************************************
  cedula[2]                = 0;                                          //*******************************************************
  cedula[3]                = 0;                                          //*******************************************************
  cedula[4]                = 0;                                          //*******************************************************
  cedula[5]                = 0;                                          //*******************************************************
  cedula[6]                = 0;                                          //*******************************************************
  cedula[7]                = 0;                                          //*******************************************************
  cedula[8]                = 0;                                          //*******************************************************
  cedula[9]                = 0;                                          //*******************************************************  
  cedula_fila              = "";                                         //*******************************************************
  bandera_cedula           = 0;                                          //*******************************************************
  bandera_nombre_ced       = 0;                                          //*******************************************************
  contador_cedula          = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION validar_cedula****
}
//--------------------------------------------------------------------FIN FUNCIÓN BORRAR CÉDULA-------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------INICIO FUNCIÓN ORDERNAR PARAMETROS-------------------------------------------------------------------------------------------------------

void ordenar_parametros_super(int pantalla_ordenar_super_usuario)
{
  if(pantalla==28)
  {
    chofer=1;
    chofer_lcd="MORENO W.";
  }
  else
  {
    chofer=0;
    chofer_lcd="CEVALLOS E.";
  }
}

//--------------------------------------------------------------------FIN FUNCIÓN ORDERNAR PARAMETROS-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN VALIDAR GALONES-------------------------------------------------------------------------------------------------------

void ingreso_galones_super_usuario(char dato_galones_super, int num_estado_galones_super)
{  
    if( ((dato_galones_super=='1' || dato_galones_super=='2' || dato_galones_super=='3' || dato_galones_super=='4' || dato_galones_super=='5' || dato_galones_super=='6'|| dato_galones_super=='7' || dato_galones_super=='8' || dato_galones_super=='9' || dato_galones_super=='0'|| dato_galones_super=='=' || dato_galones_super=='%') && num_estado_galones_super==25) || num_estado_galones_super==50)                           // PRUEBA VALIDACION CEDULA 
    {  

        if((dato_galones_super=='=' && (contador_galones>0 && contador_galones<7)) || (dato_galones_super=='=' && contador_galones==7 ))
        {
        galones[contador_galones-1]=0;  
        contador_galones--;

            if((dato_galones_super=='=' && contador_galones==6 ))
            {
            galones[contador_galones-1]=0;
            contador_galones=5;   
            }
        }
    
                if(dato_galones_super!='=' && (contador_galones>=0 && contador_galones<=6))
                {
                    if(dato_galones_super=='%')
                    {
                    dato_galones_super='.';      
                    }
                galones[contador_galones]=dato_galones_super;
                contador_galones++;  
                }

    gal_fila=String(galones[0])+String(galones[1])+String(galones[2])+String(galones[3])+String(galones[4])+String(galones[5]);  
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE GALONES:"));
    lcd.setCursor(0, 3); 
    lcd.print(gal_fila);
    Num_gal_valido=gal_fila.toFloat(); 

                          if(contador_galones>=6)
                          {
                          contador_galones=7;
                          }
    }
}
//--------------------------------------------------------------------FIN FUNCIÓN VALIDAR GALONES-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN BORRAR GALONES-------------------------------------------------------------------------------------------------------
void borrar_galones()
{
#ifdef DEBUG 
  Serial.println("borrar_galones");                                       // BORRAR
#endif

  contador_galones          = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
  galones[0]                = 0;                                          //*******************************************************
  galones[1]                = 0;                                          //*******************************************************
  galones[2]                = 0;                                          //*******************************************************
  galones[3]                = 0;                                          //*******************************************************
  galones[4]                = 0;                                          //*******************************************************
  galones[5]                = 0;                                          //*******************************************************
  gal_fila                  = "";                                         //*******************************************************
  Num_gal_valido            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
}
//--------------------------------------------------------------------FIN FUNCIÓN BORRAR GALONES-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN INGRESO NUMERO FACTURA-------------------------------------------------------------------------------------------------------

void num_factura(char dato_factura, int num_estado_factura)
{
    if((dato_factura=='1' || dato_factura=='2' || dato_factura=='3' || dato_factura=='4' || dato_factura=='5' || dato_factura=='6'|| dato_factura=='7' || dato_factura=='8' || dato_factura=='9' || dato_factura=='0'|| dato_factura=='=') && num_estado_factura==24)                           // PRUEBA VALIDACION CEDULA 
    {  

        if((dato_factura=='=' && (contador_factura>0 && contador_factura<7)) || (dato_factura=='=' && contador_factura==7 ))
        {
        factura[contador_factura-1]=0;  
        contador_factura--;

            if((dato_factura=='=' && contador_factura==6 ))
            {
            factura[contador_factura-1]=0;
            contador_factura=5;   
            }
        }
    
                if(dato_factura!='=' && (contador_factura>=0 && contador_factura<=6))
                {
                factura[contador_factura]=dato_factura;
                contador_factura++;  
                }

    factura_fila=String(factura[0])+String(factura[1])+String(factura[2])+String(factura[3])+String(factura[4])+String(factura[5]);  
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE FACTURA")); 
    lcd.setCursor(0, 3); 
    lcd.print(factura_fila);
    
#ifdef DEBUG 
    Serial.print(factura_fila);
    Serial.print(' ');
    Serial.println(contador_factura);
#endif  

                          if(contador_factura>=6)
                          {
                          contador_factura=7;
                          }
    }
}
//--------------------------------------------------------------------FIN FUNCIÓN INGRESO NUMERO FACTURA-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCIÓN TECLADO MATRICIAL-------------------------------------------------------------------------------------------------------
char teclado_matricial() 
{
char tecla;                                                                         // VARIABLE UTILIZADA SOLO EN ESTA FUNCION 
    
    tecla= teclado.getKey();                                                        // GUARDA EL VALOR DE LA TECLA PRESIONADA EN LA VARIABLE tecla DE TIPO char 
     
    if (tecla != 0)                                                                 // SI EXISTE DATO INGRESA A LA CONDICION
    {
    return tecla;                                                                   // RETORNA EL VALOR DE LA tecla Y TERMINA LA FUNCION 
    }
    else return 0;                                                                  // RETORNA UN VALOR CERO SI LA FUNCION NO RECIBE NADA
}

//--------------------------------------------------------------------FIN FUNCIÓN TECLADO MATRICIAL-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCIÓN TECLADO MATRICIAL-------------------------------------------------------------------------------------------------------
void resetear_val_super_usuario() 
{
#ifdef DEBUG 
  Serial.println("resetear_val_super_usuario");                           // BORRAR
#endif  

  tem_ejecucion_super       = 0;                                          // REINICIO DE BANDERAS FUNCION leer_dato_super_usuario
  cont_temp_tecla           = 0;                                          // REINICIO DE BANDERAS FUNCION leer_dato_super_usuario
  dato_aux_super_usuario    = 0;                                          // GUARDA EL VALOR DE dato EN LA FUNCION leer_dato_super_usuario
  
  contador_factura          = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION num_factura****
  factura[0]                = 0;                                          //*******************************************************
  factura[1]                = 0;                                          //*******************************************************
  factura[2]                = 0;                                          //*******************************************************
  factura[3]                = 0;                                          //*******************************************************
  factura[4]                = 0;                                          //*******************************************************
  factura[5]                = 0;                                          //*******************************************************
  factura_fila              = "";                                         //*****FIN RESETEAR VALORES DE FUNCION num_factura*******

  contador_galones          = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
  galones[0]                = 0;                                          //*******************************************************
  galones[1]                = 0;                                          //*******************************************************
  galones[2]                = 0;                                          //*******************************************************
  galones[3]                = 0;                                          //*******************************************************
  galones[4]                = 0;                                          //*******************************************************
  galones[5]                = 0;                                          //*******************************************************
  gal_fila                  = "";                                         //*******************************************************
  galones_cargados          = 0;
  Num_gal_valido            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
  chofer                    = 0;
  chofer_lcd                = "";
  base_datos                = 0;
}

//--------------------------------------------------------------------FIN FUNCIÓN TECLADO MATRICIAL-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO FUNCION DETECTAR PRESION * SUPER USUARIO------------------------------------------------------------------------------------------------------

void leer_dato_super_usuario(int estado_teclas)
{ 


//if(dato!=0)
if(dato == 'D')
{
dato_aux_super_usuario=dato;                                                        // VARIABLE PARA RESPALDAR EL VALOR DE dato dentro de la funcion leer_dato_super_usuario
#ifdef DEBUG 
Serial.print("Valor de dato_aux_super_usuario= ");                                  // DEBUG
Serial.println(dato_aux_super_usuario);                                             // DEBUG
#endif


cont_temp_tecla=1;                                                                  // PARA QUE INGRESE DIRECTAMENTE CON EL NUEVO TECLADO
}     
    if(estado_teclas ==2 && cont_temp_tecla==0 && dato_aux_super_usuario=='D')      // COMPARA SI LA TECLA SE MANTIENE PRESIONADA OPCION HOLD Y TECLA %
    {
      #ifdef DEBUG 
          Serial.println("Condicion HOLD ok");                                            // DEBUG
          Serial.print("Valor de dato HOLD= ");                                           // DEBUG
          Serial.println(dato_aux_super_usuario);                                         // DEBUG 
      #endif
    
      time_1=millis();                                                                // CARGA EL VALOR DEL TIEMPO A LA VARIABLE time_1
   // cont_temp_tecla=1;                                                              // BANDERA cont_temp_tecla=1 PARA QUE INGRESE UNA VEZ AL BUCLE    

    }
    
        if(cont_temp_tecla==1 && dato_aux_super_usuario=='D')                        // SI MANTIENE PRECIONADA LA TECLA * ='%' INGRESA A LA CONDICION 
        {  
                     
            if( (millis()-time_1) > 5000 && tem_ejecucion_super==0)                 // SI EL TIEMPO QUE SE MANTIENE PRESIONADA LA TECLA ES MAYOR A 5 SEGUNDOS INGRESA UNA SOLA VEZ 
            { 
              #ifdef DEBUG 
                Serial.println("Condicion super usuario correcta");                     // DEBUG                   
              #endif
            pantalla=20;                                                            // CARGA EL VALOR DE LA PANTALLA PARA LA CONDICION DE SUPER USUARIO   
            tem_ejecucion_super=1;                                                  // BANDERA tem_ejecucion_super=1 PARA QUE INGRESE UNA VEZ AL BUCLE
            }
       
        }
        
        if(cont_temp_tecla==1 && estado_teclas ==0)                                 // CONDICION PARA EVITAR FALSO HOLD POR 5 SEGUNDOS
        {
        cont_temp_tecla=0;                                                          // BANDERA cont_temp_tecla=1 PARA QUE INGRESE UNA VEZ AL PRIMER BUCLE 
        time_1=millis();                                                            // VOLVER A CARGAR EL VALOR DEL TEMPORIZADOR EN CASO DE FALSO HOLD 
        }                
}

//--------------------------------------------------------------------FIN FUNCION DETECTAR PRESION * SUPER USUARIO-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCION DETECTAR PRESION NUMERO 3 CAMBIAR CLAVE------------------------------------------------------------------------------------------------------

void leer_dato_cambiar_clave(int estado_teclas)
{ 
int valor = 0;


if(dato=='*')
{
  valor = 2;
dato_aux_cambiar_clave=dato;                                                        // VARIABLE PARA RESPALDAR EL VALOR DE dato dentro de la funcion leer_dato_super_usuario
#ifdef DEBUG 
Serial.print("Valor de dato_aux_cambiar_clave= ");                                  // DEBUG
Serial.println(dato_aux_cambiar_clave);                                             // DEBUG
#endif

//cont_temp_tecla1=1; 
}     
    if(valor ==2 && cont_temp_tecla1==0 && dato_aux_cambiar_clave=='*')      // COMPARA SI LA TECLA SE MANTIENE PRESIONADA OPCION HOLD Y TECLA %
    {
#ifdef DEBUG 
    Serial.println("Condicion HOLD ok");                                            // DEBUG
    Serial.print("Valor de dato HOLD= ");                                           // DEBUG
    Serial.println(dato_aux_cambiar_clave);                                         // DEBUG 
#endif      

    time_1=millis();                                                                // CARGA EL VALOR DEL TIEMPO A LA VARIABLE time_1
    cont_temp_tecla1=1;                                                              // BANDERA cont_temp_tecla1=1 PARA QUE INGRESE UNA VEZ AL BUCLE    
    }
    
        if(cont_temp_tecla1==1 && dato_aux_cambiar_clave=='*')                        // SI MANTIENE PRECIONADA LA TECLA * ='%' INGRESA A LA CONDICION 
        {  
                     
           // if( (millis()-time_1) > 10000 && tem_ejecucion_clave==0)                // SI EL TIEMPO QUE SE MANTIENE PRESIONADA LA TECLA ES MAYOR A 5 SEGUNDOS INGRESA UNA SOLA VEZ 
           // { 
#ifdef DEBUG 
            Serial.println("Condicion cambiar clave correcta");                     // DEBUG  
#endif               
            pantalla=40;                                                            // CARGA EL VALOR DE LA PANTALLA PARA LA CONDICION DE SUPER USUARIO   
            tem_ejecucion_clave=1;                                                  // BANDERA tem_ejecucion_clave=1 PARA QUE INGRESE UNA VEZ AL BUCLE
            //}            
        }
        
        if(cont_temp_tecla1==1 && estado_teclas ==0)                                 // CONDICION PARA EVITAR FALSO HOLD POR 5 SEGUNDOS
        {
        cont_temp_tecla1=0;                                                          // BANDERA cont_temp_tecla1=1 PARA QUE INGRESE UNA VEZ AL PRIMER BUCLE 
        time_1=millis();                                                            // VOLVER A CARGAR EL VALOR DEL TEMPORIZADOR EN CASO DE FALSO HOLD 
        }                
}

//--------------------------------------------------------------------FIN FUNCION DETECTAR PRESION NUMERO 3 CAMBIAR CLAVE-------------------------------------------------------------------------------------------------------



int ESTADO_1()                                    // EJECUCION DEL ESTADO 1 PANTALLA DE INICIO 
{      
    //digitalWrite(BOMBA, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("   HIDROELECTRICA"));
    lcd.setCursor(0, 1);
    lcd.print(F(" PALMIRA - NANEGAL"));
    lcd.setCursor(0, 3);  
    lcd.print(F("BOMBA DE COMBUSTIBLE"));
#ifdef DEBUG 
    Serial.println(F("pantalla 1 -> "));          // BORRAR
#endif    
    pantalla=1;
    dato=0;
    return pantalla;
}

int ESTADO_2()                                    // EJECUCION DEL ESTADO 1 PANTALLA DE INGRESO CEDULA 
{
    tiempo_espera=millis();                                  // ACTUALIZA TEMPORIZADOR    
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE # DE CEDULA"));
#ifdef DEBUG 
    Serial.println(F("pantalla 2 -> "));          // BORRAR
#endif    
    pantalla=2;
    dato=0;
    return pantalla;
}

void ESTADO_3()                                   // EJECUCION INGRESO INCORRECTO CEDULA     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("CEDULA INCORRECTA!!!"));       
    lcd.setCursor(0, 3);
    lcd.print(F("INGRESE NUEVAMENTE"));       
#ifdef DEBUG 
    Serial.println(F("pantalla 3 -> "));          // BORRAR   
#endif
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=0;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA    
}

int ESTADO_4()                                   // EJECUCION INGRESO MÓVIL     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE MOVIL: "));       
#ifdef DEBUG 
    Serial.println(F("pantalla 4 -> "));          // BORRAR   
#endif
    pantalla=4;
    dato=0;
    return pantalla;  
}

void ESTADO_5()                                   // EJECUCION INGRESO INCORRECTO CEDULA     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("MOVIL INCORRECTO!!!")); 
    lcd.setCursor(0, 3);
    lcd.print(F("INGRESE NUEVAMENTE"));       
#ifdef DEBUG 
    Serial.println(F("pantalla 5 -> "));          // BORRAR   
#endif
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=0;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA    
}

int ESTADO_6()                                    // EJECUCION DEL ESTADO 6 PANTALLA DE INGRESO GALONES 
{      
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE GALONES:"));
#ifdef DEBUG 
  Serial.println(F("pantalla 6 -> "));          // BORRAR
#endif
    pantalla=6;
    dato=0;
    return pantalla;
}

void ESTADO_7()                                   // EJECUCION INGRESO INCORRECTO GALONES     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("# GALONES INCORRECTO"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 7 -> "));          // BORRAR   
#endif         
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=0;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA    
}

int ESTADO_8()                                    // EJECUCION DEL ESTADO 8 PANTALLA DE INGRESO CLAVE 
{      
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INGRESE SU CLAVE");  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
    lcd.setCursor(0, 1);
    lcd.print(Datos_nombre[bandera_nombre_ced]);  // IMPRIME EL NOMBRE DEL USUARIO EN EL LCD
#ifdef DEBUG 
    Serial.println(F("pantalla 8 -> "));          // BORRAR
#endif    
    pantalla=8;
    dato=0;
    return pantalla;
}

int ESTADO_9()                                   // EJECUCION INGRESO INCORRECTO CLAVE     
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("CLAVE INCORRECTA!!!"));
    lcd.setCursor(0, 1);
    lcd.print(F("INGRESE NUEVAMENTE"));    
#ifdef DEBUG 
    Serial.println(F("pantalla 9 -> "));          // BORRAR   
#endif           
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=9;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA         
    return pantalla;   
}

void ESTADO_10()                                  // EJECUCION INGRESO USUARIO BLOQUEAD     
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("USUARIO BLOQUEADO")); 
#ifdef DEBUG 
    Serial.println(F("pantalla 10 -> "));         // BORRAR   
#endif          
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=0;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA    
}

int ESTADO_11()                                   // EJECUCION INGRESO INCORRECTO CLAVE     
{
#ifdef DEBUG 
    Serial.println(F("pantalla 11 -> "));         // BORRAR   
#endif

    digitalWrite(BOMBA, HIGH);
    
    pantalla=11;                                  // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA         
    return pantalla;   
}

int ESTADO_12()                                   // EJECUCION CARGANDO GALONES     
{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("CARGANDO GALONES"));
      lcd.setCursor(0, 1);
      lcd.print(Num_gal_valido);
      
          if(Num_gal_valido<10)
          {   
          lcd.setCursor(4, 1);
          lcd.print(F("="));
          lcd.setCursor(5, 1);      
          lcd.print(galones_cargados); 
          }
              if(Num_gal_valido>=10 && Num_gal_valido<100)
              {   
              lcd.setCursor(5, 1);
              lcd.print(F("="));
              lcd.setCursor(6, 1);      
              lcd.print(galones_cargados);
              }
              if(Num_gal_valido>=100 && Num_gal_valido<=500)
              {
              lcd.setCursor(6, 1); 
              lcd.print(F("="));                    
              lcd.setCursor(7, 1);      
              lcd.print(galones_cargados);                
              }
#ifdef DEBUG 
    Serial.println(F("pantalla 12 -> "));          // BORRAR   
#endif                         
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    pantalla=12;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA         
    return pantalla;   
}


int ESTADO_13()                                   // EJECUCION CARGANDO GALONES     
{
 
    //wdt_reset(); 

    digitalWrite(buzzer, HIGH);
    delay(Tiempo_buzzer);                         // RETARDO PARA VIZUALIZAR EL MENSAJE
    
    digitalWrite(buzzer, LOW);
    delay(Tiempo_buzzer);                         // RETARDO PARA VIZUALIZAR EL MENSAJE
    
    digitalWrite(buzzer, HIGH);
    delay(Tiempo_buzzer);                         // RETARDO PARA VIZUALIZAR EL MENSAJE
    
    digitalWrite(buzzer, LOW);
    delay(Tiempo_buzzer);                         // RETARDO PARA VIZUALIZAR EL MENSAJE
    
    digitalWrite(buzzer, HIGH);
    delay(Tiempo_buzzer);                         // RETARDO PARA VIZUALIZAR EL MENSAJE
    
    digitalWrite(buzzer, LOW);
    delay(Tiempo_buzzer);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("CEDULA"));  
    lcd.setCursor(0, 1);
    lcd.print((cedula_fila));
    delay(3000);
 
//wdt_reset(); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("GALONES"));  
    lcd.setCursor(0, 1);
    lcd.print((galones_cargados));
    delay(3000);    
 
//wdt_reset(); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("MOVIL"));  
    lcd.setCursor(0, 1);
    lcd.print((movil_fila));
    delay(3000);

#ifdef DEBUG 
    Serial.println(F("pantalla 13 -> "));          // BORRAR    
#endif    
    dato=0;                                        // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO        
    pantalla=13;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA         
    return pantalla;
}      


int ESTADO_SUPER_1()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO VER # DE GALONES 
{   
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("--> NUMERO GALONES"));  
    lcd.setCursor(0, 3);
    lcd.print(F("    NUEVA CARGA   "));    

#ifdef DEBUG 
    Serial.println(F("pantalla 21 -> "));         // BORRAR
#endif      

    pantalla=21;
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO
    return pantalla;
}

int ESTADO_SUPER_2()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO NUEVA CARGA  
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("    NUMERO GALONES"));  
    lcd.setCursor(0, 3);
    lcd.print(F("--> NUEVA CARGA   "));    

#ifdef DEBUG 
    Serial.println(F("pantalla 22 -> "));         // BORRAR    
#endif      
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=22;
    return pantalla;
}

int ESTADO_SUPER_3()                              // EJECUCION CICLICA LECTURA DEL SENSOR ULTRASONICO     
{
    cabecera_lcd(); 
    lcd.setCursor(0, 2);                          // COLOCAR EL CURSOR EN LA LINEA UNO
    lcd.print(F("NUMERO GALONES"));               // IMPRIMIR EN LCD
    lcd.setCursor(0, 3);                          // COLOCAR EL CURSOR EN LA LINEA DOS
    
        digitalWrite(TriggerPin, LOW);                // GENERAR UN PULSO EN BAJO DURANTE 4us
        delayMicroseconds(2);                         // RETARDO 2us
        digitalWrite(TriggerPin, HIGH);               // GENERAR UN PULSO EN ALTO DURANTE 10us
        delayMicroseconds(10);                        // RETARDO 10us
        digitalWrite(TriggerPin, LOW);                // DEJAR EL PIN EN BAJO
   
        duracion = pulseIn(EchoPin, HIGH);            // MEDIR DURACION DEL TIEMPO ENTRE PULSOS EN MICROSEGUNDOS
        distancia_Cm = float((duracion/2)*0.0343);
        aux+=distancia_Cm;
        float d=aux/iteracion;  

#ifdef DEBUG 
        Serial.print(F("Distancia "));                                          // BORRAR  
        Serial.println(distancia_Cm);                                           // BORRAR  
        Serial.print(F("Valor promedio: "));                                    // BORRAR         
        Serial.println(d);                                                      // BORRAR          
        Serial.print(F("Numero de iteraciones: "));                             // BORRAR  
        Serial.println(iteracion);                                              // BORRAR          
        Serial.print(F("Valor aux: "));                                         // BORRAR  
        Serial.println(aux);                                                    // BORRAR   
#endif
        
            
        iteracion++;
    
    lcd.print(d);                      // IMPRIME EN EL LCD EL NUMERO DE GALONES

#ifdef DEBUG 
    Serial.println(F("pantalla 23 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO 
    
    //delay(300);                                   // RETARDO 100us   
    
    pantalla=23;    
    return pantalla;
}

int ESTADO_SUPER_4()                              // EJECUCION INGRESO DATOS FACTURA     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE FACTURA"));      
#ifdef DEBUG 
    Serial.println(F("pantalla 24 -> "));         // BORRAR    
#endif     
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    pantalla=24;
    return pantalla;
}

int ESTADO_SUPER_5()                              // EJECUCION INGRESO DATOS GALONES     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("INGRESE GALONES:"));       
#ifdef DEBUG 
    Serial.println(F("pantalla 25 -> "));         // BORRAR   
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    pantalla=25;
    return pantalla;
}

void ESTADO_SUPER_6()                              // EJECUCION INGRESO INCORRECTO GALONES     
{
    cabecera_lcd();
    lcd.setCursor(0, 2);
    lcd.print(F("# GAL INCORRECTO!!!"));       
    lcd.setCursor(0, 3);
    lcd.print(F("INGRESE NUEVAMENTE"));      
#ifdef DEBUG 
    Serial.println(F("pantalla 26 -> "));         // BORRAR   
#endif     
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO    
    delay(2000);                                  // RETARDO PARA VIZUALIZAR EL MENSAJE
    pantalla=0;                                   // CARGA EL VALOR DE 0 PORQUE NO IMPORTA EN ESTA ETAPA    
}


int ESTADO_SUPER_7()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO CHOFER # 1  
{   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("--> CEVALLOS E."));  
    lcd.setCursor(0, 1);
    lcd.print(F("    MORENO W."));      
#ifdef DEBUG 
    Serial.println(F("pantalla 27 -> "));         // BORRAR
#endif    
    pantalla=27;
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO
    return pantalla;
}

int ESTADO_SUPER_8()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO CHOFER # 2 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("    CEVALLOS E."));  
    lcd.setCursor(0, 1);
    lcd.print(F("--> MORENO W."));      
#ifdef DEBUG 
    Serial.println(F("pantalla 28 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=28;
    return pantalla;
}

int ESTADO_SUPER_9()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO RESPONSABLE # 1  
{   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("--> BELTRAN J."));  
    lcd.setCursor(0, 1);
    lcd.print(F("    MORALES F."));      
#ifdef DEBUG 
    Serial.println(F("pantalla 29 -> "));         // BORRAR
#endif    
    pantalla=29;
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO
    return pantalla;
}

int ESTADO_SUPER_10()                              // EJECUCION MENU SUPER USUARIO ITEM SELECCIONADO RESPONSABLE # 2 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("    BELTRAN J."));  
    lcd.setCursor(0, 1);
    lcd.print(F("--> MORALES F."));      
#ifdef DEBUG 
    Serial.println(F("pantalla 30 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=30;
    return pantalla;
}

int ESTADO_SUPER_11()                              // MOSTRAR LOS PARAMETROS EN LCD CEDULA, GALONES, FACTURA, CHOFER
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("FACTURA"));  
    lcd.setCursor(0, 1);
    lcd.print((factura_fila));
    delay(3000);
 
//wdt_reset(); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("GALONES"));  
    lcd.setCursor(0, 1);
    lcd.print((gal_fila));
    delay(3000);    
 
//wdt_reset(); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("CONDUCTOR"));  
    lcd.setCursor(0, 1);
    lcd.print((chofer_lcd));
    delay(3000);

//wdt_reset(); 

#ifdef DEBUG 
    Serial.println(F("pantalla 31 -> "));         // BORRAR    
#endif
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=31;
    return pantalla;
}

int ESTADO_SUPER_12()                             // ESCRIBE EN LA BASE DE DATOS
{ 
#ifdef DEBUG 
    Serial.println(F("pantalla 32 -> "));         // BORRAR    
#endif         
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=32;
    return pantalla;
}

int ESTADO_CLAVE_41()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("DESEA CAMBIAR CLAVE"));  
    lcd.setCursor(0, 1);
    lcd.print(F("--> SI"));
    lcd.setCursor(0, 2);
    lcd.print(F("    NO"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 41 -> "));         // BORRAR
#endif        
    pantalla=41;
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO
    return pantalla;
}

int ESTADO_CLAVE_42()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("DESEA CAMBIAR CLAVE"));  
    lcd.setCursor(0, 1);
    lcd.print(F("    SI"));
    lcd.setCursor(0, 2);
    lcd.print(F("--> NO"));     
#ifdef DEBUG 
    Serial.println(F("pantalla 42 -> "));         // BORRAR    
#endif        
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=42;
    return pantalla;
}

int ESTADO_CLAVE_43()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("INGRESE CLAVE"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 43 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=43;
    return pantalla;
}

int ESTADO_CLAVE_44()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("INGRESE NUEVA CLAVE"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 44 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=44;
    return pantalla;
}

int ESTADO_CLAVE_45()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("OPERACION FALLIDA"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 45 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=45;
    delay(2000);
    return pantalla;
}

int ESTADO_CLAVE_46()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("INGRESE NUEVA CLAVE"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 46 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=46;
    return pantalla;
}

int ESTADO_CLAVE_47()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("VALIDANDO CLAVES"));  
#ifdef DEBUG 
    Serial.println(F("pantalla 47 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    nueva_clave_2=nueva_Camb_clave;
    delay(2000);                                  // RETARDO PARA VER EL MENSAJE EN LCD
    pantalla=47;
    return pantalla;
}

int ESTADO_CLAVE_48()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("CLAVE CAMBIADA"));  

    lcd.setCursor(0, 1);
    lcd.print(F("NUEVA CLAVE")); 
    
    lcd.setCursor(0, 2);
    lcd.print(String(nueva_camb_clave[0])+String(nueva_camb_clave[1])+String(nueva_camb_clave[2])+String(nueva_camb_clave[3])); 
#ifdef DEBUG 
    Serial.println(F("pantalla 48 -> "));         // BORRAR    
#endif     
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=48;
    return pantalla;
}

int ESTADO_CLAVE_49()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("OPERACION CANCELADA"));
    lcd.setCursor(0, 1);
    lcd.print(F("CLAVES DIFERENTES"));
#ifdef DEBUG 
    Serial.println(F("pantalla 49 -> "));         // BORRAR    
#endif    
    dato=0;                                       // DATO SE CARGA CON CERO PARA QUE NO INGRESE DE NUEVO   
    pantalla=49;
    return pantalla;
}
//--------------------------------------------------------------------INICIO ENVIO DATOS CARGA DE TANQUE-------------------------------------------------------------------------------------------------------

void base_datos_protocolo(int CONDUCTOR_1, String factura_string, float galones_float ) 
{
    String value;
    
    if(CONDUCTOR_1==1)
    {
    value="MORENO_W.";
    }
    else
    {
    value="CEVALLOS_L."; 
    }
    
    if ((cliente.connect(serv, 80)))                                      // Connecting at the IP address and port we saved before
    { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("OPERACION CORRECTA"));
    lcd.setCursor(0, 1);
    lcd.print(F("CLIENTE CONECTADO"));
    lcd.setCursor(0, 2);
    lcd.print(F("DATOS ENVIADOS A"));
    lcd.setCursor(0, 3);
    lcd.print(F("BASE DE DATOS"));
    delay(4000);
            
        if(bandera_datos==1)                                              // BANDERA PARA UNA SOLA EJECUCION
        {
          #ifdef DEBUG 
            Serial.println("");
            Serial.println("****** BASE DE DATOS CARGAR TANQUE ENVIADO OK ******");
            Serial.println("");
          #endif
        cliente.print("GET /ethernet/conexion_base_tanque/data.php?");                         // Connecting and Sending values to database
        
        cliente.print("GALONES=");
        cliente.print(galones_float);
        
        cliente.print("&FACTURA=");
        cliente.print(factura_string);
        
        cliente.print("&CONDUCTOR=");
        cliente.print(value);        

        String responsable;                                               // QUITAR 

        if(responsable_int == 1)                                          // QUITAR
        {
        responsable="MORALES_F.";                                         // QUITAR
        }
        else
        responsable="BELTRAN_J.";                                         // QUITAR
        
        cliente.print("&RESPONSABLE=");                                   // QUITAR
        cliente.println(responsable);                                     // QUITAR
        
        cliente.stop();                                                   // Closing the connection
        bandera_datos    =0;
        } 
    }
    else
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR DE CONEXION"));
    lcd.setCursor(0, 1);
    lcd.print(F("CLIENTE DESCONECTAD0"));
    lcd.setCursor(0, 2);
    lcd.print(F("REVISE SU CONEXION"));
    lcd.setCursor(0, 3);
    lcd.print(F("DE INTERNET"));
        
    #ifdef DEBUG 
      Serial.println("");
      Serial.println("****** ERROR DE CONEXION A LA BASE DE DATOS ******");
      Serial.println("");
    #endif
    delay(4000); 
    }
}

//--------------------------------------------------------------------FIN ENVIO DATOS CARGA DE TANQUE-------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------INICIO ENVIO DATOS DESPACHO COMBUSTIBLE-------------------------------------------------------------------------------------------------------

void base_datos_gas(String nombre, String cedula, float galones, String movil) 
{
    float combustible=0,aux1=0;
#ifdef DEBUG 
  Serial.println("");
  Serial.println("****** ENVIO DATO ******");
  Serial.println("");    
#endif    
    if (cliente.connect(serv, 80))                                      // Connecting at the IP address and port we saved before
    {
    lcd.clear();
    lcd.print(F("OPERACION CORRECTA"));
    lcd.setCursor(0, 1);
    lcd.print(F("CLIENTE CONECTADO"));
    lcd.setCursor(0, 2);
    lcd.print(F("DATOS ENVIADOS A"));
    lcd.setCursor(0, 3);
    lcd.print(F("BASE DE DATOS"));    
    delay(4000);
    
// MEDICION DE NIVEL AÑADIDA

        digitalWrite(TriggerPin, LOW);                              // QUITAR
        delayMicroseconds(4);                                       // QUITAR
        digitalWrite(TriggerPin, HIGH);                             // QUITAR
        delayMicroseconds(10);                                      // QUITAR
        digitalWrite(TriggerPin, LOW);                              // QUITAR
   
        duracion = pulseIn(EchoPin, HIGH);                          // QUITAR
        //combustible = float(82-(duracion * 10 / 292/ 2))*((123.5*207.5)/3785.412);  // QUITAR       
        combustible = float(duracion*10/292/2);                     // QUITAR
        //combustible = float(duracion*10/292/2);                   // QUITAR
        //combustible=distancia_Cm;                                 // QUITAR

#ifdef DEBUG 
       Serial.println("");
       Serial.println("****** SERVIDOR CLIENTE ******");
       Serial.println("");
#endif
        if(bandera_datos_gas == 1)                                          // BANDERA PARA UNA SOLA EJECUCION
        {
#ifdef DEBUG     
        Serial.println("");
        Serial.println("****** ENVIO A BASE DATOS CARGAR COMBUSTIBLE ******");// BORRAR
        Serial.println("");        
#endif                  
        cliente.print("GET /ethernet/conexion_base_despacho/data_gas.php?");                     // Connecting and Sending values to database
        
        cliente.print("NOMBRE_ARDUINO=");
        cliente.print(nombre);
        
        cliente.print("&CEDULA_ARDUINO=");
        cliente.print(cedula);
        
        cliente.print("&GALONES_ARDUINO=");
        cliente.print(galones); 

        cliente.print("&EQUIPO_ARDUINO=");

            int aux=movil.toInt();
            switch (aux) 
            {            
                case 1:
                movil="CAMIONETA_CHEVROLET";
                break;                               
                
                case 2:
                movil="CAMIONETA_GREAT_WALL";
                break;
                 
                case 3:
                movil="EXCAVADORA";
                break;

                case 4:
                movil="MOTONIVELADORA";
                break;
                      
                case 5:
                movil="CARGADORA";
                break;
                
                case 6:
                movil="GALLINETA";
                break;
                
                case 7:
                movil="TRACTOR";                
                break;

                case 8:
                movil="VOLQUETA";
                break;

                case 9:
                movil="GENERADOR_CASA_DE_MAQUINAS";
                break;
                              
                case 10:
                movil="GENERADOR_PLANTA_DE_HORMIGON";
                break;
                
                case 11:
                movil="GENERADOR_PORTATIL";
                break;

                case 12:
                movil="GENERADOR_KIPOR";
                break;
                
                case 13:
                movil="MANT_WKV";
                break;
                
                case 14:
                movil="MANT_TALLERES";
                break;
                
                case 15:
                movil="VARIOS";
                break;
                
                case 16:
                movil="BOMBA_AGUA_PEQUE";
                break; 

                case 17:
                movil="POLICIA_NACIONAL";
                break;
                
                case 18:
                movil="CAMION_HINO";
                break;

                case 19:
                movil="CABEZAL_NISSAN";
                break; 
            }        
        cliente.print(movil);
        
        cliente.print("&NIVEL_ARDUINO=");                                 // QUITAR
        cliente.println(combustible);                                     // QUITAR
        cliente.stop();                                                   // Closing the connection
        bandera_datos_gas    =0;
        } 
    }else 
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR DE CONEXION"));
    lcd.setCursor(0, 1);
    lcd.print(F("CLIENTE DESCONECTAD0"));
    lcd.setCursor(0, 2);
    lcd.print(F("REVISE SU CONEXION"));
    lcd.setCursor(0, 3);
    lcd.print(F("DE INTERNET"));
    
#ifdef DEBUG 
    Serial.println("");
    Serial.println("****** CLIENTE DESCONECTADO ******");
    Serial.println("");
#endif
    delay(4000);
    }
}

//--------------------------------------------------------------------FIN ENVIO DATOS DESPACHO COMBUSTIBLE-------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------INICIO ENCODER_DATO CUENTA GALONES-------------------------------------------------------------------------------------------------------

void ENCODER_DATO()                                                             // INTERRUPCION ENCODER, LEE UN DATO DE UN PIN
{
if(Bandera_final_carga != HIGH && num_estado==12)                               // BANDERA DE CARGA Y ESTADO 12
{
      if((galones_cargados < Num_gal_valido) && Bandera_final_carga ==LOW)      // CONTAR SOLO SI EL NUMERO DE GALONES ES MENOR AL NUMERO VALIDO Y LA BANDERA ESTA EN BAJO
      {
      Contador_interrup++;                                                      // CONTADOR DE PULSOS DE ENCODER
      Contador_final++;                                                         // CONTADOR DE PULSOS DE ENCODER AUXILIAR
      Bandera_final_carga=LOW;                                                  // CARGAR LOW EN LA BANDERA DE CARGA COMBUSTIBLE
#ifdef DEBUG 
      Serial.print("PULSOS = ");                                                // MENSAJE DE PRUEBA SENSOR HALL
      Serial.println(Contador_interrup);
#endif      
      }
      else 
      {
#ifdef DEBUG 
      Serial.println("");
      Serial.println("****** OPERACION FINALIZADA galones_cargados = Num_gal_valido ******");// MENSAJE DE OPERACION FINALIZADA
      Serial.println("");      
#endif      
      Bandera_final_carga=HIGH;                                                 // CARGAR HIGH EN LA BANDERA DE CARGA COMBUSTIBLE
      }
}                
}
//--------------------------------------------------------------------FIN ENCODER_DATO CUENTA GALONES-------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------INICIO INTERRUPCION PIN DISPENSADOR-------------------------------------------------------------------------------------------------------

void PIN_DISPENSADOR()                                                          // INTERRUPCION PIN DISPENSADOR
{
//if(galones_cargados <= Num_gal_valido && num_estado==12)                        // SOLO FUNCIONA CUANDO NUMERO DE GALONES ES MENOR O IGUAL AL NUMERO DE GALONES VALIDO Y ESTÁ EN EL ESTADO 12
//{      
//      Serial.println("Operacion finalizada PIN_DISPENSADOR");                   // MUESTRA MENSAJE DE FIN OPERACIÓN
//      Bandera_final_carga=HIGH;                                                 // CARGA HIGH EN LA BANDERA DE CARGA COMBUSTIBLE
//}                

if(galones_cargados <= Num_gal_valido && num_estado==12)                        // SOLO FUNCIONA CUANDO NUMERO DE GALONES ES MENOR O IGUAL AL NUMERO DE GALONES VALIDO Y ESTÁ EN EL ESTADO 12
{     

#ifdef DEBUG 
  Serial.println("");
  Serial.println("****** OPERACION FINALIZADA PIN_DISPENSADOR ******");         // MUESTRA MENSAJE DE FIN OPERACIÓN
  Serial.println("");      
#endif
      Bandera_final_carga=LOW;                                                 // CARGA HIGH EN LA BANDERA DE CARGA COMBUSTIBLE
}                


}
//--------------------------------------------------------------------FIN INTERRUPCION PIN DISPENSADOR-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO FUNCIÓN RESET MAESTRO-------------------------------------------------------------------------------------------------------
void borrar()
{
#ifdef DEBUG 
  Serial.println("");
  Serial.println("****** BORRAR TODOS LOS DATOS ******");
  Serial.println("");
//  Serial.println("DATOS LEIDOS DESDE LA EEPROM");                       // BORRAR
//  Serial.println("");                                                   // BORRAR
//  Serial.println(EEPROM.read(0));                                       // DATOS DE LA EEPROM
//  Serial.println(EEPROM.read(1));
//  Serial.println(EEPROM.read(2));
//  Serial.println(EEPROM.read(3));
//  Serial.println("");                                                   // BORRAR
#endif  
  

    
cont_intentos_clave= 0;                                                  // REINICIA EL CONTADOR DE INTENTOS EN CLAVE UNICO REINICIO

  Datos_clave[0]           = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION borrar_clave****
  Datos_clave[1]           = 0;                                          //**********************************************************
  Datos_clave[2]           = 0;                                          //**********************************************************
  Datos_clave[3]           = 0;                                          //**********************************************************
  Datos_clave_validar[0]   = 0;                                          //**********************************************************
  Datos_clave_validar[1]   = 0;                                          //**********************************************************
  Datos_clave_validar[2]   = 0;                                          //**********************************************************
  Datos_clave_validar[3]   = 0;                                          //**********************************************************
  cod_clave_ing[0]         = 0;                                          //**********************************************************
  cod_clave_ing[1]         = 0;                                          //**********************************************************
  cod_clave_ing[2]         = 0;                                          //**********************************************************
  cod_clave_ing[3]         = 0;                                          //**********************************************************     
  borrar_lcd               = 0;                                          //**********************************************************
  clave_valida             = 0;                                          //**********************************************************
  contador_clave           = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION borrar_clave******** 

  movil[0]                 = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION validar_movil****
  movil[1]                 = 0;                                          //**********************************************************
  movil_fila               = "";                                         //**********************************************************
  contador_movil           = 0;                                          //**********************************************************
  bandera_movil            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION validar_movil********

  cedula[0]                = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION validar_cedula****
  cedula[1]                = 0;                                          //*******************************************************
  cedula[2]                = 0;                                          //*******************************************************
  cedula[3]                = 0;                                          //*******************************************************
  cedula[4]                = 0;                                          //*******************************************************
  cedula[5]                = 0;                                          //*******************************************************
  cedula[6]                = 0;                                          //*******************************************************
  cedula[7]                = 0;                                          //*******************************************************
  cedula[8]                = 0;                                          //*******************************************************
  cedula[9]                = 0;                                          //*******************************************************  
  cedula_fila              = "";                                         //*******************************************************
  bandera_cedula           = 0;                                          //*******************************************************
  bandera_nombre_ced       = 0;                                          //*******************************************************
  contador_cedula          = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION validar_cedula****

  Contador_interrup         = 0;                                         // REINICIA CONTADOR INTERRUPCIÓN       
  Contador_final            = 0;                                         // REINICIA CONTADOR AUXILIAR DE INTERRUPCIÓN
  Contador_aux              = 0;                                          
  Num_gal_valido            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****

  tem_ejecucion_super       = 0;                                          // REINICIO DE BANDERAS FUNCION leer_dato_super_usuario
  tem_ejecucion_clave       = 0;                                          // REINICIO DE BANDERAS FUNCION leer_dato_cambiar_clave
  cont_temp_tecla           = 0;                                          // REINICIO DE BANDERAS FUNCION leer_dato_super_usuario
  cont_temp_tecla1           = 0;                                         // REINICIO DE BANDERAS FUNCION leer_dato_cambiar_clave  
  dato_aux_super_usuario    = 0;                                          // GUARDA EL VALOR DE dato EN LA FUNCION leer_dato_super_usuario
  dato_aux_cambiar_clave    = 0;                                          // GUARDA EL VALOR DE dato EN LA FUNCION leer_dato_cambiar_clave
  
  contador_factura          = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION num_factura****
  factura[0]                = 0;                                          //*******************************************************
  factura[1]                = 0;                                          //*******************************************************
  factura[2]                = 0;                                          //*******************************************************
  factura[3]                = 0;                                          //*******************************************************
  factura[4]                = 0;                                          //*******************************************************
  factura[5]                = 0;                                          //*******************************************************
  factura_fila              = "";                                         //*****FIN RESETEAR VALORES DE FUNCION num_factura*******

  contador_galones          = 0;                                          //*****INICIO RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
  galones[0]                = 0;                                          //*******************************************************
  galones[1]                = 0;                                          //*******************************************************
  galones[2]                = 0;                                          //*******************************************************
  galones[3]                = 0;                                          //*******************************************************
  galones[4]                = 0;                                          //*******************************************************
  galones[5]                = 0;                                          //*******************************************************
  gal_fila                  = "";                                         //*******************************************************
  galones_cargados          = 0;                                          //*******************************************************
  Num_gal_valido            = 0;                                          //*****FIN RESETEAR VALORES DE FUNCION ingreso_galones_super_usuario****
  
  chofer                    = 0;                                          // REINICIA LOS VALORES DEL CHOFER
  chofer_lcd                = "";                                         // REINCIA LOS VALORES DEL CHOFER
  base_datos                = 0;                                          // BANDERA PARA ENVIAR DATOS 

  Bandera_final_carga=LOW;                                                // BANDERA DE CARGA INTERRUCIÓN CARGAR GALONES
  bandera_datos_gas=0;                                                    // BANDERA DE BASE DE DATOS


camb_clave[0]=0;
camb_clave[1]=0;
camb_clave[2]=0;
camb_clave[3]=0;
erase_lcd=0;
cont_clave=0;
Camb_clave="";

nueva_camb_clave[0]=0;
nueva_camb_clave[1]=0;
nueva_camb_clave[2]=0;
nueva_camb_clave[3]=0;
nueva_erase_lcd=0;
nueva_cont_clave=0;
nueva_Camb_clave=""; 

nueva_clave_1       ="";
nueva_clave_2       ="";

responsable_int=0;
borrar_galones();
dato = 0;
}
//--------------------------------------------------------------------FIN FUNCIÓN RESET MAESTRO-------------------------------------------------------------------------------------------------------

void antigua_clave(char clave, int num_clave)
{

    if((clave=='1' || clave=='2' || clave=='3' || clave=='4' || clave=='5' || clave=='6'|| clave=='7' || clave=='8' || clave=='9' || clave=='0'|| clave=='=') && num_clave==43)                           // PRUEBA VALIDACION CEDULA 
    {  
        if((clave=='=' && (cont_clave>0 && cont_clave<=4)))             // cont_clave=contador_clave
        {
        camb_clave[cont_clave-1]=0;  
        cont_clave--;
        erase_lcd=1;                                                   // erase_lcd=borrar_lcd
        }
           
        if((clave=='=' && cont_clave==5 ))
        {
        camb_clave[cont_clave-2]=0;
        cont_clave=3;
        erase_lcd=1;   
        }
    
        if(clave!='=' && (cont_clave>=0 && cont_clave<=3))
        {
        camb_clave[cont_clave]=clave;
        cont_clave++;
        erase_lcd=0;  
        }

       Camb_clave=String(camb_clave[0])+String(camb_clave[1])+String(camb_clave[2])+String(camb_clave[3]);  

        if(cont_clave >= 0 && cont_clave < 5)
        {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));        
        lcd.setCursor(0, 1);
        lcd.print(Camb_clave); 
        }
        
        if(cont_clave==1 && erase_lcd==0)
        {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));         
        lcd.setCursor(0, 1);
        lcd.print(F("*"));       
        }

        if(cont_clave==1 && erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));         
        lcd.setCursor(0, 1);
        lcd.print(F("*"));               
        }
        
        if(cont_clave==2  && erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("*"));           
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));    
        lcd.setCursor(0, 1);
        lcd.print(F("**"));       
        }
        
        if(cont_clave==2 && erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));    
        lcd.setCursor(0, 1);
        lcd.print(F("**"));               
        }
        
        if(cont_clave==3  && erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("**"));            
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));       
        lcd.setCursor(0, 1);        
        lcd.print(F("***"));       
        }

        if(cont_clave==3 && erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));      
        lcd.setCursor(0, 1);
        lcd.print(F("***"));               
        }        
        
        if(cont_clave>=4  && erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("***"));              
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE CLAVE"));     
        lcd.setCursor(0, 1);        
        lcd.print(F("****"));       
        }

        if(cont_clave>=4)
        {
        cont_clave=5;
        }
    }
}

void borrar_ingrese_nueva_clave()
{
nueva_camb_clave[0]=0;
nueva_camb_clave[1]=0;
nueva_camb_clave[2]=0;
nueva_camb_clave[3]=0;
nueva_erase_lcd=0;
nueva_cont_clave=0;
nueva_Camb_clave=""; 
}

void ingrese_nueva_clave(char clave, int nueva_clave)
{

    if((clave=='1' || clave=='2' || clave=='3' || clave=='4' || clave=='5' || clave=='6'|| clave=='7' || clave=='8' || clave=='9' || clave=='0'|| clave=='=') && (nueva_clave==44 || nueva_clave==46))                           // PRUEBA VALIDACION CEDULA 
    {  
        if((clave=='=' && (nueva_cont_clave>0 && nueva_cont_clave<=4)))             // nueva_cont_clave=contador_clave
        {
        nueva_camb_clave[nueva_cont_clave-1]=0;  
        nueva_cont_clave--;
        nueva_erase_lcd=1;                                                   // nueva_erase_lcd=borrar_lcd
        }
           
        if((clave=='=' && nueva_cont_clave==5 ))
        {
        nueva_camb_clave[nueva_cont_clave-2]=0;
        nueva_cont_clave=3;
        nueva_erase_lcd=1;   
        }
    
        if(clave!='=' && (nueva_cont_clave>=0 && nueva_cont_clave<=3))
        {
        nueva_camb_clave[nueva_cont_clave]=clave;
        nueva_cont_clave++;
        nueva_erase_lcd=0;  
        }

       nueva_Camb_clave=String(nueva_camb_clave[0])+String(nueva_camb_clave[1])+String(nueva_camb_clave[2])+String(nueva_camb_clave[3]);  

        if(nueva_cont_clave >= 0 && nueva_cont_clave < 5)
        {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));        
        lcd.setCursor(0, 1);
        lcd.print(nueva_Camb_clave); 
        }
        
        if(nueva_cont_clave==1 && nueva_erase_lcd==0)
        {
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));         
        lcd.setCursor(0, 1);
        lcd.print(F("*"));       
        }

        if(nueva_cont_clave==1 && nueva_erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));         
        lcd.setCursor(0, 1);
        lcd.print(F("*"));               
        }
        
        if(nueva_cont_clave==2  && nueva_erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("*"));           
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));    
        lcd.setCursor(0, 1);
        lcd.print(F("**"));       
        }
        
        if(nueva_cont_clave==2 && nueva_erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));    
        lcd.setCursor(0, 1);
        lcd.print(F("**"));               
        }
        
        if(nueva_cont_clave==3  && nueva_erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("**"));            
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));       
        lcd.setCursor(0, 1);        
        lcd.print(F("***"));       
        }

        if(nueva_cont_clave==3 && nueva_erase_lcd==1)
        {       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));      
        lcd.setCursor(0, 1);
        lcd.print(F("***"));               
        }        
        
        if(nueva_cont_clave>=4  && nueva_erase_lcd==0)
        {
        lcd.setCursor(0, 1);
        lcd.print(F("***"));              
        delay(200);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("INGRESE NUEVA CLAVE"));     
        lcd.setCursor(0, 1);        
        lcd.print(F("****"));       
        }

        if(nueva_cont_clave>=4)
        {
        nueva_cont_clave=5;
        }
    }
}


void lee_clave_epprom()
{

int num_clave=0;

for(int i=0; i<=3;i++)
{
  
num_clave=EEPROM.read(i)-48;
   
switch (num_clave) 
            {            
                case 0:
                cod_clave[i]='0';
                break;

                case 1:
                cod_clave[i]='1';
                break;

                case 2:
                cod_clave[i]='2';
                break;

                case 3:
                cod_clave[i]='3';
                break;

                case 4:
                cod_clave[i]='4';
                break;

                case 5:
                cod_clave[i]='5';
                break;

                case 6:
                cod_clave[i]='6';
                break;

                case 7:
                cod_clave[i]='7';
                break;

                case 8:
                cod_clave[i]='8';
                break;

                case 9:
                cod_clave[i]='9';
                break;
            }
}

Datos_clave_validar=String(cod_clave[0])+String(cod_clave[1])+String(cod_clave[2])+String(cod_clave[3]);      // borrar debug
#ifdef DEBUG 
  Serial.println("");
  Serial.println("****** MEMORIA EEPROM DEBUG ******");
  Serial.println("LEER MEMORIA EEPROM");
  Serial.print("CLAVE LEIDA DE MEMORIA EPPROM ");
  Serial.println(Datos_clave_validar);
  Serial.println("");
#endif           
}

void escribir_password()
{


EEPROM.write(0, nueva_camb_clave[0]);
EEPROM.write(1, nueva_camb_clave[1]);
EEPROM.write(2, nueva_camb_clave[2]);
EEPROM.write(3, nueva_camb_clave[3]);

  String prueba=String(nueva_camb_clave[0])+String(nueva_camb_clave[1])+String(nueva_camb_clave[2])+String(nueva_camb_clave[3]); 

#ifdef DEBUG 
  Serial.println("");
  Serial.println("****** MEMORIA EEPROM DEBUG ******");
  Serial.println("ESCRIBIR EN MEMORIA EEPROM");
  Serial.print("CLAVE INGRESADA EN MEMORIA EPPROM ");
  Serial.println(prueba);
  Serial.println("");
#endif           
  delay(2000);
}

void mensaje_incio(){
    lcd.clear();                                                    // BORRAR PANTALLA DE LCD
    lcd.setCursor(0, 0);                                            // POSICIONA EL CURSOR EL LA PRIMERA FILA DEL LCD
    lcd.print(F("   HIDROELECTRICA"));                              // IMPRIME MENSAJE
    lcd.setCursor(0, 1);                                            // POSICIONA EL CURSOR EL LA SEGUNDA FILA DEL LCD
    lcd.print(F("  PALMIRA - NANEGAL"));                            // IMPRIME MENSAJE
    
    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO.");                                        // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS
    
    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO..");                                       // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS
    
    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO...");                                      // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS
    
    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO....");                                     // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS
    
    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO.....");                                    // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS

    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO......");                                   // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS

    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO.......");                                  // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS

    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO........");                                 // IMPRIME MENSAJE
    delay(200);                                                     // RETARDO ESPERAR 500 MILISEGUNDOS

    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO.........");                                // IMPRIME MENSAJE
    delay(200); 

    lcd.setCursor(0, 3);                                            // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO..........");                               // IMPRIME MENSAJE    
    delay(200); 

    lcd.setCursor(0, 3);                                             // POSICIONA EL CURSOR EN LA TERCERA FILA DEL LCD
    lcd.print("INICIANDO...........");                               // IMPRIME MENSAJE    
    delay(200); 
}

void cabecera_lcd(){
    lcd.clear();                                                    // BORRAR PANTALLA DE LCD
    lcd.setCursor(0, 0);                                            // POSICIONA EL CURSOR EL LA PRIMERA FILA DEL LCD
    lcd.print(F("   HIDROELECTRICA"));                              // IMPRIME MENSAJE
    lcd.setCursor(0, 1);                                            // POSICIONA EL CURSOR EL LA SEGUNDA FILA DEL LCD
    lcd.print(F("  PALMIRA - NANEGAL"));                            // IMPRIME MENSAJE
 }

char TecladoPC(int num){

  switch (num) 
  {
    case 8:
      return 'i';                       // Izquierda
      break;
    case 10:
      return 'B';                       // BAJAR
      break;
    case 11:
      return 'A';                       // SUBIR
      break;
    case 13:
      return 'C';                       // ENTRAR
      break;
    case 21:
      return 'd';
      break;
      
    case 25:
      return 'F';                      // RePag
      break;

    case 26:
      return 'D';                      // AvPag
      break;  
        
    case 42:
      return '*';
      break;
    case 43:
      return '+';
      break;
    case 45:
      return '-';
      break;         

    case 46:
      return '%';         // PUNTO DECIMAL
      break;
    case 47:
      return '/';
      break;    
    case 48:
      return '0';
      break;
    case 49:
      return '1';
      break;
    case 50:
      return '2';
      break;
    case 51:
      return '3';
      break;
    case 52:
      return '4';
      break;
    case 53:
      return '5';
      break; 
    case 54:
      return '6';
      break;
    case 55:
      return '7';
      break;
    case 56:
      return '8';
      break;
    case 57:
      return '9';
      break;   
    case 127:
      return '=';           // BORRAR
      break;           
    default:
      return 'x';           // UNA TECLA CUALQUIERA
      break;
  } 
}
