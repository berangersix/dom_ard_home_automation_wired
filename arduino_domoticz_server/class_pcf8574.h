
#ifndef CLASS_pcf8574
#define CLASS_pcf8574


#include "Wire.h"
#include "Class_log_message.h"
static bool is_begin=false;

/**
 *  This class permit to send date to a pcf8574 for writing (reading is not yet implement
 */
class Class_pcf8574
{
private:
  uint8_t  adress;     	 	  //the adress without rw code thanks to jumper
  uint8_t  writing_adress;
  uint8_t  register_val=0xFF;
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
	Class_pcf8574(uint8_t adress) 
	{
      this->adress = adress;
      writing_adress = 0x40 + adress;
      if(!is_begin) Wire.begin();
      set_output_register();//set all to 1
  
      
	};

    /**
     *  \brief Brief Set val to the position pos 
     *  
     *  \param [in] pos the position in pcf8574 (from 0 to 7 corresponding to p0 to p7)
     *  \param [in] value 0 or 1
     *  \return Return true if val has been changed
     *  
     *  \details More details
     */
    bool set_one_output(byte pos, bool value)
  {
        //inverted logic
       value = !value;
       if (bitRead(register_val, pos) != value){
          bitWrite(register_val, pos, value);
          set_output_register();
          return true;
       }
       return false;
  }
  
 
private:
  /**
   *  \brief Brief update value to pcf output
   *  
   *  \return Return description
   *  
   *  \details More details
   */
  void set_output_register()
  {

    Wire.beginTransmission(adress);
    Wire.write(writing_adress);
    Wire.write(register_val);
    int err = Wire.endTransmission();
    if(err != 0){
      Log::msg_minor_error("error transmission code : " + String(err,DEC) + "for device adress : " + String(err,HEX));
    }
  }

};
#endif
