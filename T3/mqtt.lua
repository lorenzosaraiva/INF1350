m = mqtt.Client("clientid", 120)
-- conecta com servidor mqtt na mÃ¡quina 'ipbroker' e porta 1883:
m:connect("37.187.106.16", 1883, 0,
  -- callback em caso de sucesso  
  function(client) print("connected") end, 
  -- callback em caso de falha 
  function(client, reason) 
    print("failed reason: "..reason) 
  end)