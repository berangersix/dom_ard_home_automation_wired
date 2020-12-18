/**
*	A connector which manage ethernet communication with domoticz  
*/


#ifndef CLASS_connector_with_dom
#define CLASS_connector_with_dom

#define Ethernet_timeout 150 //client time out in ms

#include "Class_log_message.h"
#include <Ethernet.h>

enum control_obj_type { sw, dim };

struct receive_dom_change_struct {
  bool valid = false;
	int IDX;
	control_obj_type type_obj;
	String remaining_url;
};

class Class_connector_with_dom
{

	private :
	IPAddress domoticz_ip;
	int domoticz_port;
	EthernetClient 	Eclient_to_send;
	EthernetClient 	Eclient_for_receive;
	EthernetServer* E_arduino_serveur;

	public :
	
	/**
	*  \brief Brief Construct before use, must be done in setup
	*  
	*  \param [in] mac : mac adress of ethernet controler of arduino
	*  \param [in] domoticz_ip : Ip of domoticz server
	*  \param [in] domoticz_port : Domoticz listening port (it is set at 8080 by default in domoticz)
	*  \param [in] arduino_listening_port : port where arduino listen (set 4200 if you don't know what to set)
*  \param [in] arduino_ip : optionnal set a fixed ip to arduino board)
	*  \return Return Nothing
	*  
	*  \details More details
	*/
	Class_connector_with_dom( byte* mac, IPAddress domoticz_ip, int domoticz_port, int arduino_listening_port, IPAddress arduino_ip)
	{
		this->domoticz_ip = domoticz_ip;
		this->domoticz_port = domoticz_port;
    Eclient_to_send.setConnectionTimeout(Ethernet_timeout); 
    Eclient_for_receive.setConnectionTimeout(Ethernet_timeout); 
		Ethernet.begin(mac,arduino_ip);

    String LocalIP = String() + Ethernet.localIP()[0] + "." + Ethernet.localIP()[1] + "." + Ethernet.localIP()[2] + "." +
    Ethernet.localIP()[3];
		Log::msg_network_OK(String("IP is ") + LocalIP + "\n" );
		delay(10000);
		
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware) {
			Log::msg_major_error("Ethernet shield was not found.  Sorry, can't run without hardware. :( \n");
			while (true) {
				delay(1); // do nothing, no point running without Ethernet hardware
			}
		}
		 for(int i= 0; Ethernet.linkStatus() == LinkOFF && i<5; i++) {
			Log::msg_network_NOK("Ethernet cable is not connected.\n");
			delay(5000);
		}
   
		E_arduino_serveur = new EthernetServer(arduino_listening_port);
		E_arduino_serveur->begin();
    delay(10000);
		Log::msg_network_OK("Ethernet Server Ready !\n");
	};


	/**
	*  \brief Brief Send a url via json format to domoticz, for updating a dummy device
	*  
	*  \param [in] url : url to send
	*  \return Return true: com ok, false nok
	*  
	*  \details More details
	*/
	bool send_to_dom(String url){

		if (Eclient_to_send.connect(domoticz_ip, domoticz_port)) 
		{
			Log::msg_network_OK(String("Send to Domoticz : ") + url + "\n");

			Eclient_to_send.println((url));
			Eclient_to_send.println(); 
			Eclient_to_send.stop();
      return true;
		} else {
			Log::msg_network_NOK("Connection with domoticz failed\n");
      return false;
      Eclient_to_send.stop();
		}
	}

    /**
     *  \brief Brief Must be call once in loop, to receive data from domoticz
     *  
     *  \return Return a structure containing info for update
     *  
     *  \details More details
     */
    struct receive_dom_change_struct  update_server(){
    struct receive_dom_change_struct receive_data;
    
		String url = receive_client();
    if (  url != ""){
      receive_data = deserialize_url(url);
      return receive_data;
    }
    else{
      return receive_data;
    }
    
		
	}

	private :
	
	/**
	 *  \brief Brief Deserialize url receive from rasbp
	 *  
	 *  \param [in] url Description for url
	 *  \return Return a structure containing info
	 *  
	 *  \details More details
	 */
	struct receive_dom_change_struct  deserialize_url(String url){
		 struct receive_dom_change_struct my_struct;
		 //format type "GET /type=switch&idx=2&action=0 HTTP/1.1"
		 Log::msg_network_OK(url + "\n");
		 int pos_esperluette = url.indexOf('&', 0);
		 int pos_equal = url.indexOf('=', 0);
		 String type_s = url.substring(pos_equal+1, pos_esperluette);
     if (type_s == "switch"){
        my_struct.type_obj = sw;
     }
     else if(type_s == "dimmer"){
        my_struct.type_obj = dim;
     }
     else{
      Log::msg_minor_error(String("Type : ") + type_s + " non reconnu\n" );
      return my_struct;
     }
     
		 pos_esperluette = url.indexOf('&', pos_esperluette+1);
		 pos_equal = url.indexOf('=', pos_equal+1);
		 my_struct.IDX = url.substring(pos_equal+1, pos_esperluette).toInt();
     if ( my_struct.IDX <1){
      Log::msg_minor_error("IDX : " +  url.substring(pos_equal+1, pos_esperluette) + " non reconnu\n");
      return my_struct;
     }
     
		 my_struct.remaining_url = url.substring(pos_esperluette+1, url.indexOf(' ', pos_esperluette));
		 my_struct.valid = true;
		 return my_struct;
		 
	}

	/**
	 *  \brief Brief Listening data await in server, and get back url send to arduino
	 *  
	 *  \return Return url send to arduino
	 *  
	 *  \details More details
	 */
	String receive_client(){
    String url_receive="";
		// Regarde si un client est connecté et attend une réponse
		Eclient_for_receive = E_arduino_serveur->available();
		if (Eclient_for_receive) { // Un client est là ?
			int index = 0;
			while(Eclient_for_receive.connected()) { // Tant que le client est connecté
				if(Eclient_for_receive.available()) { // A-t-il des choses à dire ?
					// traitement des infos du client
					char carlu = Eclient_for_receive.read(); //on lit ce qu'il raconte
					if(carlu != '\n') { // On est en fin de chaîne ?
						// non ! alors on stocke le caractère
						url_receive += carlu;
						index++;
					} else {
						// on a fini de lire ce qui nous intéresse
						// Ferme la connexion avec le client
            // Tout d'abord le code de réponse 200 = réussite
            Eclient_for_receive.println("HTTP/1.1 200 OK");
            // Puis le type mime du contenu renvoyé, du json
            Eclient_for_receive.println("Content-Type: application/json");
            // Et c'est tout !
            // On envoie une ligne vide pour signaler la fin du header
            Eclient_for_receive.println();
						Eclient_for_receive.stop();
						break;
					}
				}
			}
		}
	  return url_receive;
	}


};
#endif
