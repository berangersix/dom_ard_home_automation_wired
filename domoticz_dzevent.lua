return {
	on = {
		devices = {
			'*'
		},

	},

	execute = function(domoticz, device) 
		-- you must first for each idx, crate a global var with -1 default value
        global_var_name = 'IDX' ..  device.idx .. '_lastval' -- this global var need to be create in user var with -1 default value
       	lastValue = domoticz.variables(global_var_name).value
	    ip_arduino      = '169.254.174.10'  --the fixed ip of arduino
	    port_arduino    = '4200'            --the fixed port of arduino server
	    
	    
	    header_url  = 'http://' .. ip_arduino .. ':' ..  port_arduino  .. '/'
	    url         =  header_url .. 'type=' 
	    
	    if (device.switchType == 'On/Off') then
	        if ( lastValue ~= device.nValue) then --send only if change
	            url = url  ..       'switch'
	            url = url  ..       '&idx='      .. device.idx
	            url = url  ..       '&action='   .. device.nValue
	            domoticz.variables(global_var_name).set(device.nValue)
	            domoticz.openURL(url)
	            domoticz.log(url, domoticz.LOG_INFO)
	        end
        elseif(device.switchType == 'Dimmer') then
            
            dlevel = 0
            if (device.state == 'Off') then--off is equivalent to 0
                dlevel = 0
            else
                dlevel = device.level
            end
            
            if ( lastValue ~= dlevel) then--send only if change
                url = url  ..       'dimmer'
                url = url  ..       '&idx='      .. device.idx
                url = url  ..   '&action='   .. dlevel
                domoticz.variables(global_var_name).set(dlevel)
	            domoticz.openURL(url)
	            domoticz.log(url, domoticz.LOG_INFO)
            end

            
            
            
        end
        

	end
}