function drawOLED()
    disp:firstPage()
        repeat
            disp:drawStr(0, 0, track)
            disp:drawStr(0, 10, artist)
    until disp:nextPage() == false
end

-- on publish message receive event
m:on("message", function(conn, topic, data) 
  --print(topic .. ":" ) 
  --if data ~= nil then
    --print(data)
  --end
  if topic == "/sonos/livingroom/track" then
    track = data
  end
  if topic == "/sonos/livingroom/artist" then
    artist = data
  end
  if artist ~= nil and track  ~= nil then
  print(artist.." - "..track)
  drawOLED()
  end
end)

m:connect(MQTT_HOST, MQTT_PORT, 0, function(conn) 
    print("connected") 
    m:publish("/esp/"..NODE_ID,"hello world from "..NODE_ID,0,0, function(conn) 
        print("sent") 
    end)
    m:subscribe("/sonos/livingroom/#",0, function(conn) 
        print("Subscribed to livingroom") 
    end)
end)

init_OLED(5,6)