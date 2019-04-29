local xinit = 10
local yinit = 10
local w = 100
local h = 50
local rets = {}


function love.load()
  
  for i = 0,10,1 
  do 
    rets[i] = retangulo(xinit, yinit, w, h)
    xinit = xinit + 60
    yinit = yinit + 60
  end  
end

function naimagem (mx, my, x, y) 
  return (mx>x) and (mx<x+w) and (my>y) and (my<y+h)
end


function love.keypressed(key)
  for i = 0,10,1 
  do 
    rets[i].keypressed(key)
  end 
end

function love.draw ()
for i = 0,10,1 
  do 
    rets[i].draw()
  end 
end


function retangulo (x,y,w,h)
local originalx, originaly, rx, ry, rw, rh = x, y, x, y, w, h
return {
  draw = 
    function ()
      love.graphics.rectangle("line", rx, ry, rw, rh)
    end,
    keypressed =
    function(key)
      local mx, my = love.mouse.getPosition() 
        if love.keyboard.isDown("down")  and naimagem(mx, my, rx, ry)  then
          ry = ry + 10
        end
        if love.keyboard.isDown("right")  and naimagem(mx, my, rx, ry)  then
          rx = rx + 10
        end
    end
  }
  end
--4 - O retangulo nao anda continuamente qunado a tecla down se mantem apertada 
