#ifndef CLASS_dimmer_dom
#define CLASS_dimmer_dom

/**
 * This class allow to control an AC dimmer and update value to dom
 * Zc must be connected according following table (exemple d2 for Arduino mega)
 * If you need to redifined Zc please use do #define ZC_PIN 2 before include
 * by default dim is set to 50hZ network is need to redifined do  #define ELEC_FREQ 50  before include
 * 
 *  ---------------------- OUTPUT & INPUT Pin table ---------------------
 *  +---------------+-------------------------+-------------------------+
 *  |   Board       | INPUT Pin               | OUTPUT Pin              |
 *  |               | Zero-Cross              |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Lenardo       | D7 (NOT CHANGABLE)      | D0-D6, D8-D13           |
 *  +---------------+-------------------------+-------------------------+
 *  | Mega          | D2 (NOT CHANGABLE)      | D0-D1, D3-D70           |
 *  +---------------+-------------------------+-------------------------+
 *  | Uno           | D2 (NOT CHANGABLE)      | D0-D1, D3-D20           |
 *  +---------------+-------------------------+-------------------------+
 *  | ESP8266       | D1(IO5),    D2(IO4),    | D0(IO16),   D1(IO5),    |
 *  |               | D5(IO14),   D6(IO12),   | D2(IO4),    D5(IO14),   |
 *  |               | D7(IO13),   D8(IO15),   | D6(IO12),   D7(IO13),   |
 *  |               |                         | D8(IO15)                |
 *  +---------------+-------------------------+-------------------------+
 *  | ESP32         | 4(GPI36),   6(GPI34),   | 8(GPO32),   9(GP033),   |
 *  |               | 5(GPI39),   7(GPI35),   | 10(GPIO25), 11(GPIO26), |
 *  |               | 8(GPO32),   9(GP033),   | 12(GPIO27), 13(GPIO14), |
 *  |               | 10(GPI025), 11(GPIO26), | 14(GPIO12), 16(GPIO13), |
 *  |               | 12(GPIO27), 13(GPIO14), | 23(GPIO15), 24(GPIO2),  |
 *  |               | 14(GPIO12), 16(GPIO13), | 25(GPIO0),  26(GPIO4),  |
 *  |               | 21(GPIO7),  23(GPIO15), | 27(GPIO16), 28(GPIO17), |
 *  |               | 24(GPIO2),  25(GPIO0),  | 29(GPIO5),  30(GPIO18), |
 *  |               | 26(GPIO4),  27(GPIO16), | 31(GPIO19), 33(GPIO21), |
 *  |               | 28(GPIO17), 29(GPIO5),  | 34(GPIO3),  35(GPIO1),  |
 *  |               | 30(GPIO18), 31(GPIO19), | 36(GPIO22), 37(GPIO23), |
 *  |               | 33(GPIO21), 35(GPIO1),  |                         |
 *  |               | 36(GPIO22), 37(GPIO23), |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Arduino M0    | D7 (NOT CHANGABLE)      | D0-D6, D8-D13           |
 *  | Arduino Zero  |                         |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Arduino Due   | D0-D53                  | D0-D53                  |
 *  +---------------+-------------------------+-------------------------+
 *  | STM32         | PA0-PA15,PB0-PB15       | PA0-PA15,PB0-PB15       |
 *  | Black Pill    | PC13-PC15               | PC13-PC15               |
 *  | BluePill      |                         |                         |
 *  | Etc...        |                         |                         |
 *  +---------------+-------------------------+-------------------------+
 */

 #include "Class_log_message.h"
 #include "class_dimmer.h"
 #include "class_connector_with_dom.h"
/**
 *  This class is used to implement a robodyn dimme to control AC light connected with dom
 *  To use it you need to construct it in setup, and then you can update it with set value
 */
class Class_dimmer_dom : public Class_dimmer
{
	private :
	String		IDX;
	Class_connector_with_dom* 	Com_with_dom;
	
	
	public :
	/**
	 *  \brief Create a new PWM control AC light control with Robodyn at pos PWM_pin, connected with dom
	 *  Initialize it to 0V
	 * 
 	 *  \param [in] Com_with_dom : A class connector with domoticz, see class_connector_with_dom.h for more info
	 *  \param [in] IDX : the id of virtual sensor in domoticz, see setup->devices in web page of domoticz
	 *  \param [in] PWM_pin : Control pin
	 *  \return Return Nothing
	 *  
	 *  \details More details
	 */
	Class_dimmer_dom(Class_connector_with_dom* Com_with_dom,int IDX, int PWM_pin) : Class_dimmer(PWM_pin)
	{

		this->Com_with_dom	= Com_with_dom;
		this->IDX			= String(IDX, DEC); 
    set_power(0);
    send_val_to_Domoticz(0);

	};
  
	 /**
   *  \brief Brief Construct url to send to domoticz and send it via Com_with_dom
   *  
   *  \param [in] dim_level ; dimmer level
   *  \return Return true com ok, false nok
   *  
   *  \details More details
   */
  bool send_val_to_Domoticz(int dim_level){
    String url = "GET /json.htm?type=command&param=switchlight&idx=" + IDX ;
    url +=  "&";
    url += "switchcmd=Set%20Level&level=";
    url += String(dim_level, DEC); 
    url+= " HTTP/1.1";
    return Com_with_dom->send_to_dom(url);
  }
  
	 /**
   *  \brief Brief Call when receive a value from domoticz
   *  
   *  \param [in] remain_url : What is receive from domoticz, it should be "action=1" or "action=1"
   *  \return Return description
   *  
   *  \details More details
   */
  void receive_val_from_dom(String remain_url){

    if(remain_url.startsWith("action=") ){
       set_power(remain_url.substring(7).toInt());
    }
    else{
      Log::msg_minor_error(String("Error receive : ") + remain_url + "instead of action=value \n");
    }

  }
  
	protected :
	/**
	 *  \brief Brief This method will be call if value changed 
	 *  
	 *  \param [in] power : power set
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	virtual  void to_do_if_val_changed(int power) {
		
		send_val_to_Domoticz(power);
		
	}
	
	
	

};

  /**
   * this class contain a list of dimmer, which help to store Class_pwm_dimmer_dom with a maximum of NBR_MAX_DIMMERS switch
   */
  class Class_list_dimmer_dom{
    private :
      Class_dimmer_dom* dims[NBR_MAX_DIMMERS];
      int IDXs[NBR_MAX_DIMMERS];
      byte actual_size;

    public :
      void add(Class_dimmer_dom* dimmer_dom, int IDX){
        this->dims[actual_size]=dimmer_dom;
        this->IDXs[actual_size]=IDX;
        actual_size++;
        
      }

      Class_dimmer_dom* get_dim( int IDX){
        for (int i =0; i<actual_size; i++){
          if( IDX == this->IDXs[i]) return this->dims[i];
        }
        Log::msg_minor_error(String("Error Switch IDX : ") + String(IDX, DEC) + "not find in database \n");
        return nullptr;
      }

      /**
       * Send all value to domoticz
       */
      void update_all_val_to_dom(){
        int i = 0;
        bool result_com = true;
        
        while(i<actual_size && result_com == true)
        {
          result_com = this->dims[i]->send_val_to_Domoticz(this->dims[i]->get_power());
          i++;
        };
        
      }  
    
  };
#endif
