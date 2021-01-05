#ifndef CLASS_controlled_switch_dom
#define CLASS_controlled_switch_dom

#ifndef NBR_MAX_CONTROLLED_SWITCH
  #define   NBR_MAX_CONTROLLED_SWITCH 64  //maximum number of AC dim connected 
#endif

#include "Class_log_message.h"
#include "class_pcf8574.h"
#include "class_connector_with_dom.h"
  
    /**
   *  This class allows to control a specifc switch, and it update value to domoticz,
   *  before using this class, you need to first create relay boards with class Class_74HC595 (need in constructor)
   */

  class Class_controlled_switch_dom 
  {
	  
  private :
	Class_pcf8574* 	pcf_extender;
	bool			value;
	String		IDX;
	byte 			            pos_on_extender;
	Class_connector_with_dom* 	Com_with_dom;

	
   
  public :
    /**
     *  \brief Brief Constuctor before use
     *  
     *  \param [in] pcf_extender : A extender to connect to relay
     *  \param [in] pos_on_extender : position on relay board in Class_74HC595
     *  \param [in] Com_with_dom : A class connector with domoticz, see class_connector_with_dom.h for more info
     *  \param [in] IDX : the id of virtual sensor in domoticz, see setup->devices in web page of domoticz
     *  \return Return description
     *  
     *  \details More details
     */
    Class_controlled_switch_dom(Class_pcf8574* pcf_extender, byte pos_on_extender, Class_connector_with_dom* Com_with_dom, int IDX) 
    {
    this->pcf_extender   		  = pcf_extender;
    this->Com_with_dom        = Com_with_dom;
	  this->IDX 		  		      = String(IDX, DEC); 
	  this->pos_on_extender 	= pos_on_extender;
    set_val(false);
    send_val_to_Domoticz();
    
   
	 
    };
	
	/**
	 *  \brief Brief Change state, if true then it will be false, if flase it will be true
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	void change_state(){
		set_val(!value);
	}
	
	/**
	 *  \brief Brief Set val to a specific test
	 *  
	 *  \param [in] val Description for val
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	void set_val(bool value){
		this->value = value;
		if ( pcf_extender->set_one_output(pos_on_extender, value) ) send_val_to_Domoticz();
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
    if(remain_url == "action=1" ){
       set_val(true);
    }
    else if (remain_url == "action=0" ){
       set_val(false);
    }
    else{
      Log::msg_minor_error(String("Error receive : ") + remain_url + " instead action=0 or action=1\n");
    }

  }

  
	/**
	 *  \brief Brief Construct url to send to domoticz and send it via Com_with_dom
	 *  
	 *  \return Return true com ok, false nok
	 *  
	 *  \details More details
	 */
	bool send_val_to_Domoticz(){
		String url = "GET /json.htm?type=command&param=switchlight&idx=" + IDX ;
		url +=  "&";
		if(value) url+= "switchcmd=On";
		else  url+= "switchcmd=Off";
		url+= " HTTP/1.1";
		return Com_with_dom->send_to_dom(url);
	}
	


};



  /**
   * this class contain a list of Class_controlled_switch_dom, which help to store Class_controlled_switch_dom with a maximum of 128 switch
   */
  class Class_list_controlled_switch_dom{
    private :
      Class_controlled_switch_dom* SWs[NBR_MAX_CONTROLLED_SWITCH];
      int IDXs[NBR_MAX_CONTROLLED_SWITCH];
      byte actual_size;

    public :
      void add(Class_controlled_switch_dom* sw_dom, int IDX){
        this->SWs[actual_size]=sw_dom;
        this->IDXs[actual_size]=IDX;
        actual_size++;
        
      }

      Class_controlled_switch_dom* get_sw( int IDX){
        for (int i =0; i<actual_size; i++){;
          if( IDX == this->IDXs[i]) return this->SWs[i];
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
          result_com = this->SWs[i]->send_val_to_Domoticz();
          i++;
        };
        
      }  
    
  };
#endif
