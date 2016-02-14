out = ""
power = ""

leftBTN=3
gpio.mode(leftBTN, gpio.INT, gpio.PULLUP)


rightBTN=7                      
gpio.mode(rightBTN,gpio.INT,gpio.PULLUP)

function leftDown()
    print("Left button pressed")
    tmr.delay(50)                          -- time delay for switch debounce
    gpio.trig(leftBTN, "up", leftUp)  -- change trigger on falling edge
end

function leftUp()
    print("Left button released")
    tmr.delay(50)                          -- time delay for switch debounce
    gpio.trig(leftBTN, "down", leftDown)  -- change trigger on falling edge
end

function rightDown()
    print("Right button pressed")
    tmr.delay(50)                          -- time delay for switch debounce
    gpio.trig(rightBTN, "up", rightUp)  -- change trigger on falling edge
end

function rightUp()
    print("Right button released")
    tmr.delay(50)                          -- time delay for switch debounce
    gpio.trig(rightBTN, "down", rightDown)  -- change trigger on falling edge
end

-- Display layout
function drawOLED()
    disp:firstPage()
        repeat
            disp:drawLine(0, 11, 128, 11)
            disp:drawStr(0, 0, power.."W")
            disp:drawStr(100, 0, out.." C")
            disp:drawStr(0, 15, track)
            disp:drawStr(0, 30, artist)
            disp:drawStr(0, 54, "Play")
            disp:drawStr(100, 54, "Next")
    until disp:nextPage() == false
end

-- On incoming message, update display
m:on("message", function(conn, topic, data) 
  if topic == "/sonos/livingroom/track" then
    track = data
  end
  if topic == "/sonos/livingroom/artist" then
    artist = data
  end
  if topic == "/ivt/out" then
    out = data
    print("IVT Out:"..data)
  end
  if topic == "/power/watt" then
    power = data
    print("Power Watt:"..data)
  end
  if artist ~= nil and track  ~= nil then
  print(artist.." - "..track)
  drawOLED()
  end
end)

-- Connect to server and subscribe
m:connect(MQTT_HOST, MQTT_PORT, 0, function(conn) 
    print("connected") 
    m:publish("/esp/"..NODE_ID,"hello world from "..NODE_ID,0,0, function(conn) 
        print("sent") 
    end)
    m:subscribe("/sonos/livingroom/#",0, function(conn) 
        print("Subscribed to livingroom") 
    end)
    m:subscribe("/ivt/#",0, function(conn) 
        print("Subscribed to ivt") 
    end)
    m:subscribe("/power/watt",0, function(conn) 
        print("Subscribed to power") 
    end)
end)

init_OLED(5,6)
gpio.trig(leftBTN, "down", leftDown)
gpio.trig(rightBTN, "down", rightDown)