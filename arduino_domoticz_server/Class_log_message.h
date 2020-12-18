
#ifndef CLASS_log
#define CLASS_log



#ifdef USE_SERIAL
	#ifndef BAUD_RATE 
	  #error BAUD_RATE must be specified before include in main file
	#endif
#endif

//manage meory
#include <MemoryUsage.h>
//sd management
#include <SPI.h>
#include <SD.h>


#define SD_PIN 4
#define MAX_NBR_LINES 2000
#define MAX_NBR_FILES 100
//sd card info, we create a maximum of 100 files 2000 (environ 100j), after we go back to first
static int current_file_number=-1;
static int nbr_lines;
static bool SD_avalaible=false;
File LogFile;

#include "Class_error_led.h"

/**
 *  This class will set info in sd card
 *  it will also display info in serial if a baudrate set
 *  and it will save also info in SD card if it is ok
 */
  
class Log
{


public :
    /**
     *  \brief Brief Brief Must be called before used
     *  
     *  \return Return description
     *  
     *  \details More details
     */
    static void init(){
		    Error_led::init();
		
        #ifdef USE_SERIAL
          Serial.begin(BAUD_RATE);
        #endif    
         
         
         //test SD and create file
         if (!SD.begin(SD_PIN)) {
            #ifdef USE_SERIAL
              Log::msg_minor_error("initialization SD failed!\n");
            #endif
         }
         else{
              Log::msg_info("Initializing SD card...\n");
              for (int i = MAX_NBR_FILES-1; i>=0; --i){
                  if (SD.exists("index" + String(i,DEC) )){
                    current_file_number = i;
                    i=-1;//(we break loop)
                  }
              }
              //now create a new file
              create_new_file();
            
         }
        
	}
 
    /**
     *  \brief Brief Print memory info in serial link
     *  
     *  \return Return description
     *  
     *  \details More details
     */
		static void print_mem(){
    #ifdef USE_SERIAL
      Serial.println(F("Memory info:"));
      Serial.println();
      
      MEMORY_PRINT_START
      MEMORY_PRINT_HEAPSTART
      MEMORY_PRINT_HEAPEND
      MEMORY_PRINT_STACKSTART
      MEMORY_PRINT_END
      MEMORY_PRINT_HEAPSIZE
  
      Serial.println();
    #endif    
}

    /**
	 *  \brief Brief Call this to display a simple message in txt in file or serial if activate
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void msg_info(String txt = ""){
		txt = "Message : " + txt;
		internal_print(txt);
	}
	
	/**
	 *  \brief Brief Call if network is ok, and print a txt in file or serial if activate
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void msg_network_OK(String txt = ""){
		Error_led::network_OK();
		txt = "Network : " + txt;
		internal_print(txt);
	}
	
			
	/**
	 *  \brief Brief Call if network is Nok, and print a txt in file or serial if activate
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void msg_network_NOK(String txt = ""){
		Error_led::network_NOK();
		txt = "Network error : " + txt;
		internal_print(txt);
	}

			
	/**
	 *  \brief Brief Call if there is a minor error, and print a txt in file or serial if activate
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void msg_minor_error(String txt = ""){
		Error_led::minor_error();
		txt = "Minor Error : " + txt;
		internal_print(txt);
	}

			
	/**
	 *  \brief Brief Call if network is major error, and print a txt in file or serial if activate
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	static void msg_major_error(String txt = ""){
		Error_led::major_error();
		txt = "Major Error : " + txt;
		internal_print(txt);
	}

   /**
    *  \brief Brief to be called in main loop, to update led
    *  
    *  \return Return description
    *  
    *  \details More details
    */
   static void update_state(){
	   Error_led::update_state();
   }

private :
	/**
	 *  \brief Brief Thi class could not be instanciated
	 *  
	 *  \return Return description
	 *  
	 *  \details More details
	 */
	Log(){
	}
	
	/**
	 *  \brief Brief print in serial and file if request
	 *  
	 *  \param [in] txt to print
	 *  \return Return description
	 *  
	 *  \details More details
	 */
static void internal_print(String txt){
      #ifdef USE_SERIAL
        Serial.print(txt);
      #endif
      
      if(SD_avalaible){
          if (LogFile) {
            LogFile.print(txt);
            LogFile.flush();
            nbr_lines++;
            if(nbr_lines>MAX_NBR_LINES){
              SD_avalaible = false;
              LogFile.close();
              create_new_file();
            }
          }
          else{
            Log::msg_minor_error("Error log" +  String(current_file_number,DEC) );
            SD_avalaible=false;
          }
        }
   }


  /**
   *  \brief create a new file, to print info and create a file to tell which is the last file
   *  
   *  \return Return description
   *  
   *  \details More details
   */
static void create_new_file(){
      SD.remove("index" + String(current_file_number,DEC));
      current_file_number++;

      //remove last index
      SD.remove("log" + String(current_file_number,DEC));
      
      //create new index
      LogFile = SD.open("index" + String(current_file_number,DEC), FILE_WRITE);
      // if the file is available, write to it:
      if (LogFile) {
        LogFile.print(String(current_file_number,DEC) + "\n");
        LogFile.flush();
        LogFile.close();
      }
      // if the file isn't open, pop up an error:
      else {
        Log::msg_minor_error("Error opening index" +  String(current_file_number,DEC) );
      }
      

      //create log
      LogFile = SD.open("log" + String(current_file_number,DEC), FILE_WRITE);
      nbr_lines = 0;
      SD_avalaible=true;
      Log::msg_info("log " + String(current_file_number,DEC)+ " is ready\n");
    }

};
#endif
