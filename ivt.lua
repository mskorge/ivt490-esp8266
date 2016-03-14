uart.setup(0,9600,8,0,1)
uart.on("data", 0, 
      function(data)
        print("receive from uart:", data)
        if data=="quit" then 
          uart.on("data") 
        end        
    end, 0)