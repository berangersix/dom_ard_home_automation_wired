/**
 * This class allow to control an AC dimmer 
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

#ifndef CLASS_dimmer
#define CLASS_dimmer

#ifndef ZC_PIN 
//default pin is 2 on mega, could be defined for something else in main class
  #define ZC_PIN 2
#endif

#ifndef ELEC_FREQ 
//default pin is 50HZ in France
  #define ELEC_FREQ 50
#endif

#ifndef NBR_MAX_DIMMERS 
  #define   NBR_MAX_DIMMERS 54  //maximum number of AC dim connected 
#endif

#ifndef LEVEL_DIM_LOW 
  #define LEVEL_DIM_LOW 20  //below this power (from 0 to 100) light is off, this will rescale power
#endif
#ifndef LEVEL_DIM_HIGH
  #define LEVEL_DIM_HIGH 100  //above this power (from 0 to 100) light is on, this will rescale power
#endif

#define	  MAX_LEVEL_DIM 128   // Dimming level (0-128)  0 = on, 128 = 0ff
#include  <TimerOne.h>       // Avaiable from <a href="http://www.arduino.cc/playground/Code/Timer1" rel="nofollow">
static volatile int 	        counter_interrupt_zc  = 0;      			// Variable to use as a counter
static volatile int 			    nbr_dimmer = 0;								// number of dimmer
static volatile int         	requested_dimmer_level[NBR_MAX_DIMMERS];// Dimming level (0-MAX_LEVEL_DIM)  0 = on, 128 = Off
static volatile int 	        dimmer_output_pin[NBR_MAX_DIMMERS];  // Dimming pin
static volatile bool          dim_status[NBR_MAX_DIMMERS];//previous state :false low, true HIGH 
static volatile bool 			    dimmer_zc_started = false;
static volatile int 			    freqStep;//frequence in microseconds


/**
 *  \brief Brief Interuption execute when triac trigger zero cross
 *  
 *  \return Return description
 *  
 *  \details More details
 */
static void zero_cross_detect() {    
	counter_interrupt_zc=0;
	for( int i=0; i< nbr_dimmer ; i++) {
		if(dim_status[i]){
			digitalWrite(dimmer_output_pin[i], LOW);       // turn off TRIAC (and AC)
			dim_status[i] = false;
		}
	}
}                                 

/**
 *  \brief Brief Interuption execute by timer, it is sampling execution to reproduce sinwave
 *  
 *  \return Return description
 *  
 *  \details More details
 */
static void dim_check() {        

	for( int i=0; i< nbr_dimmer ; i++){     
		if(counter_interrupt_zc>=requested_dimmer_level[i]) {                 
			if(!dim_status[i]){
				digitalWrite(dimmer_output_pin[i], HIGH);       // turn off TRIAC (and AC)
				dim_status[i] = true;
			}     
		}  
	}
	counter_interrupt_zc++; // increment time step counter  

}                    
/**
 *  \brief Brief Add a dimmer
 *  
 *  \param [in] AC_pin : pin where dimmer is connected
 *  \return Return position in table
 *  
 *  \details More details
 */
static int add_dimmer(int AC_pin) {  
	int pos =  nbr_dimmer;


	if (dimmer_zc_started){
		Timer1.detachInterrupt();
		detachInterrupt(digitalPinToInterrupt(ZC_PIN));
		counter_interrupt_zc = 0;
		
	}
	requested_dimmer_level[pos]=2*MAX_LEVEL_DIM+1;//be sure never happen
	dimmer_output_pin[pos] = AC_pin;
	pinMode(AC_pin, OUTPUT);  
	nbr_dimmer++;
	freqStep = (1000000.0/(ELEC_FREQ*2))  /MAX_LEVEL_DIM; 	//freq halfwave /max level , it is recreation sinwave frequency in microsecond
	dimmer_zc_started = true;										
	attachInterrupt(digitalPinToInterrupt(ZC_PIN), zero_cross_detect, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
	Timer1.initialize(freqStep);                      							// Initialize TimerOne library for the freq we need
	Timer1.attachInterrupt(dim_check, freqStep);    
	return pos;
}

/**
*  This class is used to implement a dimmer to control AC light. 
*  To use it you need to construct it in setup, and then you can update it with set value
*/
class Class_dimmer
{

private:
	byte  pos;     	 	  // where dimmer is on dimmer table
	int   power;  		  // power request
	float convert_factor =  (LEVEL_DIM_HIGH-LEVEL_DIM_LOW)/(100.0);
	float y_zero         =  LEVEL_DIM_LOW;
public:
	/**
	*  \brief Create a new PWM control AC light control using triac(as robodyn ac dimmer) at pos PWM_pin
	*  Initialize it to 0V
	*  
	*  \param [in] PWM_pin : Control pin
	*  \return Return Nothing
	*  
	*  \details More details
	*/
	Class_dimmer(int PWM_pin) 
	{
		pos = add_dimmer(PWM_pin);
		power = 0;
		set_power(0);
		
	};


	/**
	*  \brief Brief Change current state of light, if power >0 set light to 0, else set to 100
	*  
	*  \return Return Nothing
	*  
	*  \details More details
	*/
	void change_state(){
		if ( get_power() >1) {
			set_power(0);
		}
		else {
			set_power(100);
		}
		
	}
	
	/**
	*  \brief Brief Set to a specific power state
	*  
	*  \param [in] power_requested : power requested in percent (from 0 to 100)
	*  \return Return description
	*  
	*  \details More details
	*/
	void set_power(int power_requested){
		if (power_requested != power){
			//maximum is 100%, min is 0 (but we decide under 10 is 0, above 90 is 100)
			if ( power_requested > 90){
				power = 100;
				requested_dimmer_level[pos]=0;//full on
			}
			else if ( power_requested < 10){
				power = 0;
				requested_dimmer_level[pos]=MAX_LEVEL_DIM*2 +1; //to be sure to never have this condition
			}
			else{
				power = power_requested;
				requested_dimmer_level[pos]= MAX_LEVEL_DIM - ( y_zero+( convert_factor*power_requested));
			}
			to_do_if_val_changed( power );
		}  
	}


	/**
	*  \brief Brief get actual power
	*  
	*  \return Return actual power
	*  
	*  \details More details
	*/
	byte get_power(){
		return power;
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
		
	}
};
#endif
