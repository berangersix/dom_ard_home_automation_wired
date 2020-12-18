#ifndef CLASS_error_led
#define CLASS_error_led

#ifndef RED_LED_PIN 
  #error RED_LED_PIN must be specified before include in main file
#endif

#ifndef YELLOW_LED_PIN 
  #error YELLOW_LED_PIN must be specified before include in main file
#endif

#ifndef GREEN_LED_PIN 
  #error GREEN_LED_PIN must be specified before include in main file
#endif

#define LED_BLINK_CYCLE_ERROR 1000
//global variable
  static bool green_state=false;    
  static bool yellow_state=false;   
  static unsigned long last_green_pin_change_ms;  
  static bool minor_error_state=false;
  static bool network_error_state=false;
  
class Error_led
{


	

public :
    /**
     *  \brief Brief Must be called before used
     *  
     *  \return Return description
     *  
     *  \details More details
     */
    static void init(){
		pinMode(RED_LED_PIN, OUTPUT);
		pinMode(YELLOW_LED_PIN, OUTPUT);
	  pinMode(GREEN_LED_PIN, OUTPUT);
    last_green_pin_change_ms=millis();
    
	}
	
	/**
	 *  \brief Brief Call if network is ok
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void network_OK(){
		network_error_state = false;
	}
	
		
	/**
	 *  \brief Brief Call if network is NOK
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void network_NOK(){
		network_error_state = true;
		yellow_state = false;
		digitalWrite(YELLOW_LED_PIN, yellow_state); 
		
	}

	/**
	 *  \brief Brief Call if network is minor error
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void minor_error(){
		minor_error_state = true;
	}
	
	/**
	 *  \brief Brief Call if network is major error
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void major_error(){
	    digitalWrite(RED_LED_PIN, HIGH); 
	}

   /**
    *  \brief Brief to be called in main loop, to update led
    *  
    *  \return Return description
    *  
    *  \details More details
    */
   static void update_state(){
     unsigned long elpase_ms=  millis()-last_green_pin_change_ms;
     if (elpase_ms>LED_BLINK_CYCLE_ERROR) last_green_pin_change_ms = last_green_pin_change_ms + elpase_ms;
     
	   if (minor_error_state){
        if(elpase_ms > LED_BLINK_CYCLE_ERROR){
           //blink green small error
          green_state = !green_state;
          digitalWrite(GREEN_LED_PIN, green_state); 
        }

	   }
	   else{
		   //full green no error
		   digitalWrite(GREEN_LED_PIN, HIGH); 
	   }
	   
	   if (!network_error_state){
          //blink network ok
           if(elpase_ms > LED_BLINK_CYCLE_ERROR){
            yellow_state = !yellow_state;
            digitalWrite(YELLOW_LED_PIN, yellow_state); 
           }

	   }
	   
   }

private :
	/**
	 *  \brief Brief Thi class could not be instanciated
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	Error_led(){
	}

};
#endif
