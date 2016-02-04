dofile("config.lua")

wifi.setmode(wifi.STATION)


-- Debug info
print('\n\nSTATION Mode:',    'mode='..wifi.getmode())
print('MAC Address: ',      wifi.sta.getmac())
print('Chip ID: ',          node.chipid())
print('Heap Size: ',        node.heap(),'\n')


-- Start the connection attempt
wifi.sta.config(WIFI_SSID, WIFI_PASS)


-- Count how many times you tried to connect to the network
local wifi_counter = 0


-- Create an alarm to poll the wifi.sta.getip() function once a second
-- If the device hasn't connected yet, blink through the LED colors. If it 
-- has, turn the LED white
tmr.alarm(0, 1000, 1, function()
    if wifi.sta.getip() == nil then
        print("Connecting to AP...\n")
        
        -- Rotate through RGB colors while waiting
        wifi_counter = wifi_counter + 1;
    else
        ip, nm, gw = wifi.sta.getip()
        
        -- Debug info
        print("\n\nIP Info: \nIP Address: ",ip)
        print("Netmask: ",nm)
        print("Gateway Addr: ",gw,'\n')
        
        tmr.stop(0)     -- Stop the polling loop
        
        -- Continue to main function after network connection
        dofile("main.lua")
    end
end)
