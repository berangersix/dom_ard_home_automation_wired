
//maximum number of dimmers use in your project card (try to set nbr of elem+1) to reduce size at minimum
#define   NBR_MAX_DIMMERS 9
#define   NBR_MAX_CONTROLLED_SWITCH 33

#define USE_SERIAL
#define BAUD_RATE 115200
#define RED_LED_PIN 27
#define YELLOW_LED_PIN 29
#define GREEN_LED_PIN 25
#define BAUD_RATE 115200


#include "Class_log_message.h"
#include "class_connector_with_dom.h"
#include "class_dht22_dom.h"
#include "class_controlled_switch_dom.h"
#include "class_button_connected.h"
#include "class_dimmer_dom.h"


//ETh connector
Class_connector_with_dom* connector_with_dom;
IPAddress domoticz_server_ip(192, 168, 0, 13);
byte arduino_mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x80, 0xB7 }; // arduino shield mac
IPAddress ip_arduino(192, 168, 0, 10); //Fixed adress for debug, to fix in dhcp and delete it
#define DOM_PORT 8080
#define ARDUINO_PORT 4200
#define FORCE_REFRESH_TIME 15000 //force to refresh every 5min(300000ms) TODO change val
#define FORCE_REFRESH_RELAY 1000
unsigned long last_relay_refresh;
unsigned long last_forced_refresh;
struct receive_dom_change_struct receive_dom_obj;

//temperature sensor
Class_dht22_dom* dht1;
#define dht1_pin 24
#define dht1_IDX 1

//relay board
#define CLK_PIN 22
#define L_CLK_PIN 24
#define SER_PIN 26
Class_74HC595* relay_board;
#define NBR74HC595 1
#define PINOUT_TABLE {1,3,5,7,9,11,13,15}//,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,17,19,21,23,25,27,29,31}

//controlled switch
Class_list_controlled_switch_dom list_controlled_sw;
Class_controlled_switch_dom* lchambre; 
#define IDXlchambre 2
Class_controlled_switch_dom* lsalon; 
#define IDXlsalon 3


//dimmer
Class_list_dimmer_dom list_dim;
Class_dimmer_dom* ldim;
#define IDXldim 4
#define ldim_pin 6

//button
Class_Button_connected* bp_chambre;
#define PIN_BP_CHAMBRE 23
Class_Button_connected* bl_salon;
#define PIN_BL_SALON 40


void setup() {

  //set error pin management
  Log::init();

  
  connector_with_dom = new Class_connector_with_dom( arduino_mac, domoticz_server_ip, DOM_PORT, ARDUINO_PORT, ip_arduino);
  dht1 = new Class_dht22_dom(dht1_pin, connector_with_dom, dht1_IDX);

   //set pinout order
  byte Pinout_table_relay[32] = PINOUT_TABLE;
  relay_board = new Class_74HC595(CLK_PIN, L_CLK_PIN, SER_PIN, NBR74HC595, Pinout_table_relay );
  //set all light with their ID
  lchambre =new  Class_controlled_switch_dom(relay_board,connector_with_dom ,IDXlchambre , 1  );
  lsalon =new  Class_controlled_switch_dom(relay_board,connector_with_dom ,IDXlsalon , 3  );
  list_controlled_sw.add(lchambre,IDXlchambre); 
  list_controlled_sw.add(lsalon,IDXlsalon);



  //set a dimmer
  ldim = new Class_dimmer_dom(connector_with_dom,IDXldim,ldim_pin);
  list_dim.add(ldim,IDXldim);
  
  //create button
  bp_chambre = new Class_Button_connected(PIN_BP_CHAMBRE,BUTTON_TYPE_PUSH);
  bl_salon= new Class_Button_connected(PIN_BL_SALON,BUTTON_TYPE_CLASSIC);

  //connect button chambre allume chambre et salon
  bp_chambre->attach_controlled_switch(lsalon);
  bp_chambre->attach_controlled_switch(lchambre);
  bp_chambre->attach_dimmer(ldim);

  //salon allume salon
  bl_salon->attach_controlled_switch(lsalon);

  //set refresh to 0
  last_forced_refresh = millis();
  last_relay_refresh = millis();
  //to print memory in serial
  Log::print_mem();
}
void loop() {
   //update error led
   Log::update_state();
  //update button and sensor
  dht1->update_state();
  bl_salon->update_state();
  bp_chambre->update_state();

  //update server, do not change
  receive_dom_obj = connector_with_dom->update_server();
  if (receive_dom_obj.valid){
    if(receive_dom_obj.type_obj == dim){
      list_dim.get_dim(receive_dom_obj.IDX)->receive_val_from_dom(receive_dom_obj.remaining_url);
    }
    else if (receive_dom_obj.type_obj == sw) {
      list_controlled_sw.get_sw(receive_dom_obj.IDX)->receive_val_from_dom(receive_dom_obj.remaining_url);
    }
  } 

  //sometimes send data to domoticz
  if( (millis()-last_forced_refresh) > FORCE_REFRESH_TIME ){
     last_forced_refresh = millis();
     list_dim.update_all_val_to_dom();
     list_controlled_sw.update_all_val_to_dom();
     Log::print_mem();
  }
}
