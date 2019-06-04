m:subscribe("apertou-tecla",0,  
       -- fÃ§ chamada qdo inscriÃ§Ã£o ok:
       function (client) 
         print("subscribe success") 
       end
)

m:on("message", 
    function(client, topic, data)   
      print(topic .. ":" )   
      if data ~= nil then print(data) end
    end
)
