#ifndef CLASS_Button
#define CLASS_Button

#define BUTTON_TYPE_PUSH true
#define BUTTON_TYPE_CLASSIC false
/**
 *  This class is used to implement a simple button (latch or push). The input receive 0 or 5V (without pullup).
 *  To use it you need to construct it in setup, and then you call update_state in loop
 */
class Class_Button
{
	private :
	int     button_Pin;
	bool    state = false;
	bool 	button_type;
	bool 	previous_val;
	bool    last_previous_val_undebounced;
	unsigned long      button_debounce_delay;
	unsigned long      lastDebounceTime;

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
	Class_Button(int button_Pin, bool button_type, unsigned long button_debounce_delay = 50 ) 
	{
		this->button_Pin = button_Pin;
		this->button_type = button_type;
		this->button_debounce_delay = button_debounce_delay;
		pinMode(button_Pin, INPUT);    
		//bouncing is avoid for init, by adding a long delay in main class init loop
		previous_val = digitalRead(button_Pin);

	};
	
	/**
	 *  \brief Brief Update value, must be call in loop
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	bool update_state(){
		if (button_type == BUTTON_TYPE_PUSH) update_state_push();
		else update_state_classic();
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
	}
	
	private :
	/**
	 *  \brief Brief Read the input when it is stable
	 *  
	 *  \return Return a stable value of input
	 *  
	 *  \details More details
	 */
	bool  read_with_debounce(){
		// read the state of the switch into a local variable:
		int reading = digitalRead(button_Pin);

		// check to see if you just pressed the button
		// (i.e. the input went from LOW to HIGH), and you've waited long enough
		// since the last press to ignore any noise:

		// If the switch changed, due to noise or pressing:
		if (reading != last_previous_val_undebounced) {
			// reset the debouncing timer
			lastDebounceTime = millis();
			last_previous_val_undebounced = reading;
		}


		if ((millis() - lastDebounceTime) > button_debounce_delay) {
			// whatever the reading is at, it's been there for longer than the debounce
			// delay, so take it as the actual current state:
			return reading;
		}
		else{
			//other wise we kept as it was
			return previous_val;
		}
		
	}
	
	/**
	 *  \brief Brief update state of classic button
	 *  
	 *  
	 *  \return Return State of switch : by default false, and then change if value change
	 *  
	 *  \details More details
	 */
	bool update_state_classic(){
		int  val_read = read_with_debounce();
		if(val_read!= previous_val){
			state = !state;
			previous_val   = val_read;
			to_do_if_val_changed();
		}
		return state;
	}

	/**
	*  \brief Brief update state of push button
	*  
	*  
	*  \return Return State of push_button : by default false, when pushbutton press, state change 
	*  
	*  \details More details
	*/
	bool update_state_push(){
		int  val_read = read_with_debounce();
		if(val_read == 0 and previous_val == 1){
			state = !state;
			to_do_if_val_changed();
		}
		previous_val = val_read;
		return state;
	}

};
#endif
