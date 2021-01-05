/**
 *  This class allows to control output of one or multiple shift register module 74HC595 connected in series
 *  The control is achieved by 3 digital output Pin on your Arduino
 *  A Pinout_table must be set, this table link the position of 74HC595 outputs ( A to  H) with you desired control position
 *  For example if you have 2 74HC595 ; name a and b connected together. Output of these 2 : 74HC595 control Pin number from 1 to 16 as below.
You must set Pinout_table = {1,3,5,7,9,11,13,15,2,4,6,8,10,12,14,16}. Then when use methode set(5,HIGH), you will set Pin 5 (75HC595a/C) to 5volt.


				      ------------------------		   ------------------------	
Pin_data  ->	|SER_IN			    SER_out	| ->	|SER_IN			    SER_out	|
Pin_LClock->	|L_Clock		    L_Clock	| ->	|L_Clock		    L_Clock	| 
Pin_Clock ->	|Clock			      Clock	| ->	|Clock			      Clock	| 
				      |	   75HC595a			      | 		|	   75HC595b		      	|
				      | A  B  C  D  E  F  G  H| 		| A  B  C  D  E  F  G  H|
				      -----------------------		     ------------------------	
				        |  |  |  |  |  |  |  |		    |  |  |  |  |  |  |  |	
Pinout			    1  3  5  7  9 11 13  15		    2  4  6  8 10  12 14 16	
 (exemple relay, diode ...)
 */

#ifndef CLASS_74HC595
#define CLASS_74HC595
#include <SPI.h>

//change if you want more 74HC595 in series (not recommanded)
#ifndef MAX_NBR_74HC595_PER_LINE
    #define MAX_NBR_74HC595_PER_LINE 4
#endif
  class Class_74HC595
  {
  private :
	byte   register_values[MAX_NBR_74HC595_PER_LINE];
  byte    nbr_of_74HC595;
  byte   local_register_pos[MAX_NBR_74HC595_PER_LINE*8 + 1];
  byte   local_associated_register[MAX_NBR_74HC595_PER_LINE*8 + 1];
	int     Clock_Pin;
	int     LClock_Pin;
	int     data_Pin;
   
  public :
    /**
     *  \brief Class must be contruct with this method
     *  
     *  \param [in] Pin_Clock   : Arduino Pin position connected to Clock Pin on 74HC595 
     *  \param [in] Pin_LClock  : Arduino Pin position connected to L_Clock Pin on 74HC595 
     *  \param [in] Pin_data    : Arduino Pin position connected to data Pin on 74HC595 (also name SER_IN pin)
     *  \param [in] Pinout_table: A table  that link the position of 74HC595 outputs ( A to  H) with you desired control position 
     see commentary in top of file for more info
     *  
     *  \return Return Nothing
     */
    Class_74HC595(int Pin_Clock, int Pin_LClock, int Pin_data, byte number_of_74HC595,byte* Pinout_table = nullptr) 
    {
      
    Clock_Pin      = Pin_Clock;
	  LClock_Pin     = Pin_LClock;
	  data_Pin       = Pin_data;
    nbr_of_74HC595 = number_of_74HC595;
	  pinMode(Clock_Pin, OUTPUT);    
    pinMode(LClock_Pin, OUTPUT);  
    pinMode(data_Pin, OUTPUT);  
    digitalWrite(Clock_Pin, LOW);
    digitalWrite(LClock_Pin, LOW);
    digitalWrite(data_Pin, LOW);


    //initialise by to 0 and update
    for (byte i=0; i< (number_of_74HC595); i++){
      register_values[i] = 255;
     }
     update_value();
     
    //first initialise to dafault value
     for (byte i=0; i< (number_of_74HC595*8); i++){
          //by default we set in simpliest order 0,1,2,3,...
          local_register_pos[i]    = i%8;
          local_associated_register[i]  = i/8;
     }
    //reorder Pinout_table for direct access
	  if ( Pinout_table != nullptr){
		      //reorder position for simplier futur use
          for(byte i=0; i< (nbr_of_74HC595*8); i++){
            byte pos                   = Pinout_table[i];
            local_register_pos[pos]    = i%8;
            local_associated_register[pos]  = i/8;
          }
      
	  }
   
	 
    };
	/**
	 *  \brief set a output of register to specified value
	 *  
	 *  \param [in] position  :  Pinout position
	 *  \param [in] value     :  Value requested true or false 
	 *  \return Return A bool :  True value set was different than previous, false value requested is identical as stored one, so nothing was done
	 *  
	 *  \details More details
	 */
	bool set_one_output(byte pos, bool value){

    value = !value;//true was off, false on , so we invert
		if ( bitRead(register_values[ local_associated_register[pos] ], local_register_pos[pos]) != value){
		  bitWrite(register_values[ local_associated_register[pos] ], local_register_pos[pos], value);
		  update_value();
		  return true;
		}
		else {
		 return false;
		}
	}
	
	/**
	 *  \brief get a output of register to specified value
	 *  
	 *  \param [in] position Pinout position
	 *  \return Return value set
	 *  
	 *  \details More details
	 */
	bool get_one_output(byte pos){

		return bitRead(register_values[ local_associated_register[pos] ], local_register_pos[pos]);
		
	}
	private :
 /**
   *  \brief Rewrite shifout to be MBfirst by default, add delay, and debug
    *  
    *  \return Nothing
    *  
    *  \details More details
    */ 
  void own_shift_out(int data_Pin,int Clock_Pin, byte value) {

        for (int i = 0; i < 8; i++)  { 
           digitalWrite(data_Pin, !!(value & (1 << (7 - i))));
           delayMicroseconds(5);
           digitalWrite(Clock_Pin, HIGH);
           delayMicroseconds(5);
           digitalWrite(Clock_Pin, LOW);
           delayMicroseconds(5);            
        }
    }
    public :
	 /**
	  *  \brief update register, and activate output
	  *  
	  *  \return Nothing
	  *  
	  *  \details More details
	  */
	 void update_value(){
		digitalWrite(LClock_Pin, LOW);
		 //prepare data for input, output last 
		for (byte i=0; i<nbr_of_74HC595; i++){
		   own_shift_out(data_Pin, Clock_Pin , register_values[nbr_of_74HC595-1-i]);
		}
		//output data 
		digitalWrite(LClock_Pin, HIGH);
	 }

};
#endif
