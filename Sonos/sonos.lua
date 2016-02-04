-- Display layout
function drawOLED()
    disp:firstPage()
        repeat
            disp:drawLine(0, 18, 128, 18)
            disp:drawStr(0, 0, "2000W")
            disp:drawStr(100, 0, "10 C")
            disp:drawStr(0, 20, track)
            disp:drawStr(0, 40, artist)
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
  if topic == "/ivy/out" then
    out = data
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
end)

init_OLED(5,6)