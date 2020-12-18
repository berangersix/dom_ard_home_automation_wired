#ifndef CLASS_Button_connected
#define CLASS_Button_connected

#include "class_button.h"
#include "Class_controlled_switch_dom.h"
#include "class_dimmer_dom.h"
/**
*  This class is used to implement a simple button (latch or push) which is connected with dimmers or controlled switch
*/
class Class_Button_connected : public Class_Button
{
	private :
	Class_controlled_switch_dom* list_control_sw[NBR_MAX_CONTROLLED_SWITCH];
	Class_dimmer_dom*  list_dimmer[NBR_MAX_DIMMERS];
	byte nbr_conrolled_switch=0;
	byte nbr_dimmer=0;

	public :
	/**
	*  \brief The constructor of button
	*  
	*  \param [in] button_Pin  :  Pin were switch input is connected
	*  \param [in] button_type : a boolean, you must set BUTTON_TYPE_PUSH(true) or BUTTON_TYPE_CLASSIC(false)
	*  \param [in] button_debounce_delay : Debouncing time in ms (default 50). A input will be validate, only if it is stable for more than 50ms
	*  \return Return description
	*  
	*  \details More details
	*/
	Class_Button_connected(int button_Pin, bool button_type, unsigned long button_debounce_delay = 50) : Class_Button(button_Pin,button_type,button_debounce_delay)
	{
	}
	
	/**
	*  \brief Brief Attach a control switch to button
	*  
	*  \param [in] control_sw  :  a switch to connect with this button
	*  \return Return Nothing
	*  
	*  \details More details
	*/
	void attach_controlled_switch(Class_controlled_switch_dom* control_sw){
		list_control_sw[nbr_conrolled_switch]=control_sw;
		nbr_conrolled_switch++;
	}
	
	/**
	*  \brief Brief Attach a dimmer to Button
	*  
	*  \param [in] dimmer : a dimmer to connect to Button
	*  \return Return description
	*  
	*  \details More details
	*/
	void attach_dimmer(Class_dimmer_dom* dimmer){
		list_dimmer[nbr_dimmer]=dimmer;
		nbr_dimmer++;
	}
	
	
	protected :
	/**
	*  \brief Brief This method will be call if value changed
	*  
	*  \return Return description
	*  
	*  \details More details
	*/
	virtual  void to_do_if_val_changed() {
		for (int i = 0; i < nbr_conrolled_switch ; i++)	list_control_sw[i]->change_state();
		for (int i = 0; i < nbr_dimmer ; i++)	list_dimmer[i]->change_state();
	}
	
	private :
	
};
#endif
