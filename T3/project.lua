local led1 = 3
local led2 = 6
local sw1 = 1
local sw2 = 2
local mytimer = tmr.create()

local function right ()
  if gpio.read(sw2) == gpio.LOW then
     m:publish("apertou-tecla", "tiro",
           0, 0, function (c) print ("enviou!") end)
  else
     m:publish("apertou-tecla", "right",
           0, 0, function (c) print ("enviou!") end)
  end
end

local function left ()
  m:publish("apertou-tecla", "left",
           0, 0, function (c) print ("enviou!") end)
end

m:on("message", 
    function(client, topic, data)   
      if data == 'kill' then
        gpio.write(led1, gpio.HIGH)
        tmr.delay(200000)
        gpio.write(led1, gpio.LOW)
      end
    end
)

-- coloca os pinos dos leds em modo de saida
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)
-- apaga os leds
gpio.write(led1, gpio.LOW)
gpio.write(led2, gpio.LOW)
-- coloca um sinal estavel nas chaves 1 e 2
gpio.mode(sw1,gpio.INT,gpio.PULLUP)
gpio.mode(sw2,gpio.INT,gpio.PULLUP)
-- cadastra a funcao cbchave para ser chamada se
-- a chave 1 for apertada
gpio.trig(sw1, "down", right)
gpio.trig(sw2, "up", left)
