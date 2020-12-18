/**
 *  This class allow you to create a sensor which communicate with domoticz by ethernet
 */


#ifndef CLASS_DHT22_dom
#define CLASS_DHT22_dom

#include "Class_log_message.h"
#include "DHT.h"   //you need to first install dht22 library from arduino repo
#include "class_connector_with_dom.h"
#define DHTTYPE DHT22       // DHT type 22  (AM2302)
#define DELTA_UPDATE 0.2	//under 0.2 diff, we do not update value in domoticz
class Class_dht22_dom
{

	private :
	DHT* dht;
	unsigned long   refresh_time;
	unsigned long   last_refresh;
	float			humidity;
	float						temperature;
	Class_connector_with_dom* 	Com_with_dom;
	String						IDX;
	
	public :
	
	/**
	 *  \brief Brief Construct before use
	 *  
	 *  \param [in] Pin_to_read  :the pin where dht22 is connected
	 *  \param [in] Com_with_dom : A class connector with domoticz, see class_connector_with_dom.h for more info
	 *  \param [in] IDX : the id of virtual sensor in domoticz, see setup->devices in web page of domoticz
	 *  \param [in] refresh_time : time of requested refresh in ms, default 120secondes
	 *  \return Return Nothing
	 *  
	 *  \details More details
	 */
	Class_dht22_dom(int Pin_to_read, Class_connector_with_dom* Com_with_dom, int IDX, unsigned long refresh_time=120000)
	{
		this->refresh_time = refresh_time;
		this->Com_with_dom = Com_with_dom;
		this->last_refresh = millis();
		this->IDX 		  = String(IDX, DEC); 
		dht = new DHT(Pin_to_read, DHTTYPE);
    dht->begin();
    Log::msg_info(String("Temperature sensor IDX : ") + this->IDX + " is ready for use\n");
	};
	
	/**
	 *  \brief A function which update sensor, if refresh cycle is achieved, must be called in loop
	 *  
	 *  \return Return Nothing
	 *  
	 *  \details More details
	 */
	bool update_state(){
		if ( (millis() - last_refresh) > refresh_time){
			last_refresh = millis() ;
			float new_humidity 		  = dht->readHumidity();
			float new_temperature 	= dht->readTemperature();
      if (isnan(new_humidity) || isnan(new_temperature) ) {
        Log::msg_minor_error(String("Echec de lecture capteur with IDX : ") + IDX + " \n");
        return;
      }
			else if( abs(new_humidity - humidity)> DELTA_UPDATE || abs(new_temperature - temperature) > DELTA_UPDATE ){
				temperature = new_temperature;
				humidity = new_humidity;
				send_val_to_Domoticz();
			}
		}
	}

	private :
	/**
	 *  \brief Brief Construct url to send to domoticz and send it via Com_with_dom
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	void send_val_to_Domoticz(){
		
		String url = "GET /json.htm?type=command&param=udevice&idx=" + IDX ;
		url +=  "&nvalue=0&svalue=";
		url +=  String(temperature, 1);
		url +=  ";";
		url +=  String(humidity, 1);
		url +=  ";0";
		url+= " HTTP/1.1";
		Com_with_dom->send_to_dom(url);
		
	};
};
#endif
