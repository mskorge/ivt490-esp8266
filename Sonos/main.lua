m = mqtt.Client(MQTT_CLIENTID, 60, "", "") -- Living dangerously. No password!

-- Set up Last Will and Testament (optional)
-- Broker will publish a message with qos = 0, retain = 0, data = "offline"
-- to topic "/lwt" if client don't send keepalive packet
-- m:lwt("/lwt", "Oh noes! Plz! I don't wanna die!", 0, 0)

m:on("connect", function(m) 
    -- Serial status message
    print ("\n\n", MQTT_CLIENTID, " connected to MQTT host ", MQTT_HOST,
        " on port ", MQTT_PORT, "\n\n")
end)
m:on("offline", function(m) print ("Offline") end)

m:publish("/esp/"..NODE_ID,"hello world from "..NODE_ID,0,0, function(conn) 
    print("sent") 
end)

m:connect(MQTT_HOST, MQTT_PORT, 0, function(conn) 
    print("connected") 
end)

