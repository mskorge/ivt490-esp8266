out = ""
power = ""

leftBTN=3
gpio.mode(leftBTN, gpio.INT, gpio.PULLUP)


rightBTN=7                      
gpio.mode(rightBTN,gpio.INT,gpio.PULLUP)

function volDown()
    if(gpio.read(3) == 0) then
            print("volDown")
            tmr.alarm(1, 500, 0, volDown)
    end
end

function volUp()
    if(gpio.read(7) == 0) then
            print("volUp")
            tmr.alarm(1, 500, 0, volUp)
    end
end

function leftDown()
    print("Left button pressed")
    tmr.delay(50)
    tmr.alarm(0, 1000, 0, volDown)
    gpio.trig(leftBTN, "up", leftUp)
end

function leftUp()
    print("Left button released")
    tmr.delay(50)
    gpio.trig(leftBTN, "down", leftDown)
end

function rightDown()
    print("Right button pressed")
    tmr.delay(50)
    tmr.alarm(0, 1000, 0, volUp)
    gpio.trig(rightBTN, "up", rightUp)
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