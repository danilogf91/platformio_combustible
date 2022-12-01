
#include <Arduino.h>
#include "funciones_super_usuario.hpp"

//--------------------------------------------------------------------INICIO VARIABLES DE USO GENERAL-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE TRABAJO PARA VALIDAR CEDULA-------------------------------------------------------------------------------------------------------

String            Datos_cedula[]            = {"0997101571","1715460653","1600154726","0503260689","1713904488","1104839483","1804281838","1002949889","0910345693"};
int               comparador_cedula         = 0;
char              *comparar[]               = {"0997101571","1715460653","1600154726","0503260689","1713904488","1104839483","1804281838","1002949889","0910345693"};  // DEBE SER EL MISMO DE Datos_cedula
int               contador_cedula           = 0;
String            cedula_fila               = "";  
char              cedula[10]                = {0,0,0,0,0,0,0,0,0,0};
int               bandera_cedula            = 0;
int               bandera_nombre_ced        = 0;

//--------------------------------------------------------------------FIN VARIABLES DE TRABAJO PARA VALIDAR CEDULA-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE TRABAJO BUZZER-------------------------------------------------------------------------------------------------------

#define           buzzer                    42        // PIN DEL BUZZER 
#define           Tiempo_buzzer             500      // TIEMPO DEL BUZZER

//--------------------------------------------------------------------FIN VARIABLES DE TRABAJO BUZZER-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE TRABAJO GALONES-------------------------------------------------------------------------------------------------------

float             Num_gal_valido            = 0;                  // NUMERO PARA VALIDAR SI LOS GALONES INGRESADOS SON VALIDOS 
char              galones[6]                = {0,0,0,0,0,0};      // ALMACENA LAS TECLAS INGRESAS POR USUARIO EN 6 POSISIONES TIPO CHAR
String            gal_fila                  = "";                 // RECIBE LOS DATOS CHAR EN UN SOLO STRING
#define           BOMBA                     37                    // PIN DE ENCENDIDO DE LA BOMBA
float             Contador_interrup         = 0;                  // CUENTA EL NUMERO DE PULSOS DEL ENCODER
int               Contador_final            = 0;                  // 
int               Contador_aux              = 0;                  
bool              Bandera_final_carga       = LOW;                // BANDERA DE CARGA DE LOS GALONES
int               contador_galones          = 0;     
char              aux_fin_carga             = 0;   
float             galones_cargados          = 0;
//--------------------------------------------------------------------FIN VARIABLES DE TRABAJO GALONES-------------------------------------------------------------------------------------------------------


int               contador_factura          = 0;                  // CUENTA EL NUMERO DE DIGITOS DE LA FACTURA 
char              factura[6]                = {0,0,0,0,0,0};      // ALMACENA LAS TECLAS INGRESAS POR USUARIO EN 6 POSISIONES TIPO CHAR
String            factura_fila                ;                   // RECIBE LOS DATOS CHAR DEL NUMERO DE FACTURA Y LOS CONVIERTE EN UN SOLO STRING 

//--------------------------------------------------------------------INICIO VARIABLES DE CONTROL PANTALLAS-------------------------------------------------------------------------------------------------------

int               pantalla                  = 1;                      // INDICA EL NUMERO DE PANTALLA QUE SE ESTA VISUALIZANDO 
int               num_estado                = 0;                      // INDICA EL ESTADO EN EL QUE SE ENCUENTRA LA EJECUCION
int               resetear                  = 0;                      // BANDERA PARA RESET COMPLETO

//--------------------------------------------------------------------FIN VARIABLES DE CONTROL PANTALLAS-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE CONTROL TIEMPO-------------------------------------------------------------------------------------------------------


unsigned long     tiempo_espera             = 0;                      // TEMPORIZADORES PARA RESET POR INACTIVIDAD
unsigned long     tiempo_espera_aux         = 0;                      // TEMPORIZADORES PARA RESET POR INACTIVIDAD AUXILIAR
unsigned long     tiempo_tanque             = 0;
unsigned long     tiempo_tanque_aux         = 0;
# define          interval                    300
# define          TEMPORIZADOR                60000                   // VALOR DE TIEMPO A ESPERAR HASTA EL REINICIO

//--------------------------------------------------------------------FIN VARIABLES DE CONTROL TIEMPO-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE CONTROL CODIGO DE VEHICULO-------------------------------------------------------------------------------------------------------

char              movil[2]                 = {0,0};                  // CODIGO DE DOS DIGITOS PARA EL VEHICULO
int               contador_movil           = 0;                      // CONTADOR DE DIGITOS DEL CODIGO
String            movil_fila                  ;                      // FILA DE VALORES DE CODIGO
String            datos_movil[]            = {"01","02","03","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19"};
char*             movil_puntero[]          = {"01","02","03","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19"};  // DEBE SER EL MISMO DE datos_movil
int               bandera_movil            = 0;                     // PARA REALIZAR UNA SOLA EJECUCION

//--------------------------------------------------------------------FIN VARIABLES DE CONTROL CODIGO DE VEHICULO-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE CONTROL CLAVE-------------------------------------------------------------------------------------------------------

char               cod_clave[4]             = {0,0,0,0};
char               cod_clave_ing[4]         = {0,0,0,0};
String             Datos_clave              = "";
String             Datos_clave_validar      = "";
char               clave[4]                 = {0,0,0,0};
int                contador_clave           = 0;
int                cont_intentos_clave      = 0;
int                clave_valida             = 0;

//--------------------------------------------------------------------FIN VARIABLES DE CONTROL CLAVE-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO VARIABLES DE CONTROL SUPER USUARIO-------------------------------------------------------------------------------------------------------

#define           tiempo_super_usuario 4000                 // TIEMPO EN MILISEGUNDOS DE ESPERA MIENTRAS SE PRESIONA LA TECLA * PARA EL SUPER USUARIO
unsigned long     time_1              = 0;                  // ALMACENA EL TIEMPO EN MILISEGUNDOS
int               cont_temp_tecla     = 0;                  // CONTADOR DE TECLA HOLD
int               cont_temp_tecla1    = 0;                  // CONTADOR DE TECLA HOLD
int               tem_ejecucion_super = 0;                  // BANDERA PARA LEER TIEMPO MILLIS()
int               tem_ejecucion_clave = 0;                  // BANDERA PARA LEER TIEMPO MILLIS()

char              dato_aux_super_usuario ;                  // VARIABLE PARA RESPALDAR EL VALOR DE dato dentro de la funcion leer_dato_super_usuario
char              dato_aux_super;
int               chofer              = 0;                  // VARIABLE PARA DESIGNAR AL CHOFER
String            chofer_lcd             ;
int               base_datos          = 0;


char              dato_aux_cambiar_clave ;                  // VARIABLE PARA RESPALDAR EL VALOR DE dato dentro de la funcion leer_dato_cambiar_clave
//--------------------------------------------------------------------FIN VARIABLES DE CONTROL SUPER USUARIO-------------------------------------------------------------------------------------------------------



char              dato; 
int               borrar_lcd          = 0;
String            Datos_nombre[]      = {"ARMIJOS_J._A.","BELTRAN_J.","CEVALLOS_L.","CRESPO_E.","DELGADO_G.","GRANDA_D.","MORALES_F.","MORENO_W.","TORRES_N."};


//--------------------------------------------------------------------FIN VARIABLES DE USO GENERAL-------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------INICIO SENSOR ULTRASONICO---------------------------------------------------------------------------------------------------------        

const int         EchoPin             = 16;             // PINES DEL SENSOR ULTRASONICO
const int         TriggerPin          = 17;             // SENSOR ULTRASONICO
float             duracion            = 0;              // REINICIAR EN BORRAR TODO????
float             distancia_Cm        = 0;              // REINICIAR EN BORRAR TODO????
float             aux                 = 0;              // acumula el valor del sensor
int               iteracion           = 1;              // cuenta las iteraciones

//--------------------------------------------------------------------FIN SESNOR ULTRASONICO---------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------INICIO CAMBIAR CLAVE-------------------------------------------------------------------------------------------------------------
int               erase_lcd           = 0;
int               cont_clave          = 0;
char              camb_clave[4]       = {0,0,0,0};
String            Camb_clave          = "";
//-------------------------------------------------------------------FIN CAMBIAR CLAVE------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------INICIO INGRESO NUEVA CLAVE-------------------------------------------------------------------------------------------------------------
int               nueva_erase_lcd      = 0;
int               nueva_cont_clave    = 0;
char              nueva_camb_clave[4] = {0,0,0,0};
String            nueva_Camb_clave    = "";

String            nueva_clave_1       ="";
String            nueva_clave_2       ="";

int               responsable_int     =0 ;                          // QUITAR

//-------------------------------------------------------------------FIN INGRESO NUEVA CLAVE------------------------------------------------------------------------------------------------------------


const int DataPin = 2;
const int IRQpin =  3;
int  bandera_datos      = 0;
int  bandera_datos_gas  = 0;



void mensaje_incio ();
void leer_dato_super_usuario (int estado_teclas);
void ENCODER_DATO ();
void PIN_DISPENSADOR ();
int ESTADO_1 ();
int ESTADO_2 ();
void ESTADO_3 ();
int ESTADO_4 ();
void ESTADO_5 ();
int ESTADO_6 ();
void ESTADO_7 ();
int ESTADO_8 ();
int ESTADO_9 ();
void ESTADO_10 ();
int ESTADO_11();
int ESTADO_12 ();
int ESTADO_13 ();


int ESTADO_CLAVE_41 ();
int ESTADO_CLAVE_42 ();
int ESTADO_CLAVE_43 ();
int ESTADO_CLAVE_44 ();
int ESTADO_CLAVE_45 ();
int ESTADO_CLAVE_46 ();
int ESTADO_CLAVE_47 ();
int ESTADO_CLAVE_48 ();
int ESTADO_CLAVE_49 ();

void cabecera_lcd ();
char TecladoPC (int num);
void mensaje_incio ();
void escribir_password ();
void lee_clave_epprom ();
void ingrese_nueva_clave (char clave, int nueva_clave);
void borrar_ingrese_nueva_clave ();
void antigua_clave (char clave, int num_clave);
void borrar ();


void ingreso_movil (char dato_movil, int num_estado_movil);
void comparacion_mov (int num_estado_mov, String fila_mov, int bandera_movil_aux);
void borrar_movil ();
void validar_cedula (char dato_ced, int num_estado_ced);
void leer_dato_cambiar_clave (int estado_teclas);
void borrar_galones ();
void borrar_cedula ();
void ordenar_parametros_super (int pantalla_ordenar_super_usuario);
void base_datos_protocolo (int CONDUCTOR_1, String factura_string, float galones_float);
void comparacion_ced (int num_estado_aux, String cedula_fila_aux, int bandera_cedula_aux);
void validar_clave (int num_estado_val_clave, int contador_clave_val_clave);
void num_factura (char dato_factura, int num_estado_factura);
void cargar_galones (int num_estado_gal);
void ingreso_clave (char dato_clave, int num_estado_clave);
void ingreso_galones_normal (char dato_galones, int num_estado_galones);
void borrar_clave ();
void base_datos_gas (String nombre, String cedula, float galones, String movil);
void ingreso_galones_super_usuario (char dato_galones_super, int num_estado_galones_super);