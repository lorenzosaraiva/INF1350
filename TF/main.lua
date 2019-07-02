local mqtt = require("mqtt_library")

enemyAmount = 5
topSpeed = 0
minSpeed = 0
fallSpeed = 10
totalBullets = 100
megaShots = 3
bombsAmount = 10
playerX = 50
currentLevel = 1
bossHP = 10
enemyW  = 40
enemyH = 20

local function newblip (vel)
  local x, y = 0, 0
  local width, height = love.graphics.getDimensions( )
  local function up()
    while true do
      x = x+vel
      if x > width then
        -- volta para a esquerda da janela
        x = 0
        y = y + fallSpeed
      end
      if y > height then 
        -- Perde o Jogo
      end
      coroutine.yield ()
    end
  end
  return {
    update = coroutine.wrap(up),
    affected = function (pos)
      if pos>x and pos<x+20 then
      -- "pegou" o blip
        return true
      else
        return false
      end
    end,
    draw = function ()
      if currentLevel == 1 then
        love.graphics.draw(spaceInvaders0, x, y, 0, 0.1)
      end
      
      if currentLevel == 2 then
        love.graphics.draw(spaceInvaders1, x, y, 0, 0.1)
      end
      if currentLevel == 3 then
        love.graphics.draw(spaceInvaders2, x, y, 0, 0.1)
      end
      if currentLevel == 4 then
        love.graphics.draw(spaceInvaders3, x, y, 0, 0.1)
      end
    end,
    getPositionX = function()
      return x
    end,
    getPositionY = function()
      return y
    end
  }
end

local function shoot (xPos, yPos)
  totalBullets = totalBullets - 1
  local width, height = love.graphics.getDimensions( )
  local x, y = xPos, yPos
  return {
    draw = function ()
      y = y - 10
      love.graphics.rectangle("line", x, y, 2, 5)
    end,
    getPositionX = function()
      return x
    end,
    getPositionY = function()
      return y
    end
  }
end


local function bomb (xPos, yPos)
  local width, height = love.graphics.getDimensions( )
  local x, y = xPos, yPos
  local xspeed, yspeed = 0, 5
  return {
    draw = function ()
      y = y - yspeed
      x = x + xspeed
      love.graphics.circle("line", x, y, 10, 10)
    end,
    getPositionX = function()
      return x
    end,
    getPositionY = function()
      return y
    end,
    setSpeedX = function(val)
      xspeed = val
    end,
    setSpeedY = function(val)
      yspeed = val
    end
  }
end

local function newplayer ()
  playerX, y = 50, 500
  local width, height = love.graphics.getDimensions( )
  return {
  try = function ()
    return playerX
  end,
  update = function (dt)
    if playerX > width then
      playerX = 0
    end
  end,
  
 keypressed =
    function(key)
        if love.keyboard.isDown("left")   then
          x = x - 10
        end
        if love.keyboard.isDown("right")  then
          x = x + 10
        end
        if love.keyboard.isDown("down")  then
          y = y + 10
        end
        if love.keyboard.isDown("up")  then
          y = y - 10
        end
    end,
    
  draw = function ()

    love.graphics.draw(jet, playerX, y, 0, 0.05,0.05)
  end
  }
end

function love.keypressed(key)
  player.keypressed(key)
  if key == 'a' then
    pos = player.try()
    table.insert(bulletList,shoot(x, y))
  end
  if key == 'z' then
    if megaShots > 0 then
      table.insert(bulletList,shoot(x - 20, y))
      table.insert(bulletList,shoot(x - 40, y))
      table.insert(bulletList,shoot(x, y))
      table.insert(bulletList,shoot(x + 20, y))
      table.insert(bulletList,shoot(x + 40, y))
      megaShots = megaShots - 1
      totalBullets = totalBullets + 5
    end
  end
  
  if key == 'q' then
    if bombsAmount > 0 then
      table.insert(bombs,bomb(x, y))
      bombsAmount = bombsAmount - 1
    end
  end
end

function love.load()
  mqtt_client = mqtt.client.create("mqtt-dashboard.com", 1883 ,mqttcb)
  mqtt_client:connect("cliente love 1")
  mqtt_client:subscribe({"reativos"})
  spaceInvaders0 = love.graphics.newImage("greenblack.jpg")
  spaceInvaders1 = love.graphics.newImage("alien.png")
  spaceInvaders2 = love.graphics.newImage("alien2.png")
  spaceInvaders3 = love.graphics.newImage("alien3.png")

  jet = love.graphics.newImage ("jet.png")
  love.graphics.setBlendMode("add")

  love.keyboard.setKeyRepeat(true)
  startGame()
end

function checkCollision()
  
  for i, enemy in ipairs(listabls) do
      for index, bullet in ipairs(bulletList) do
        if currentLevel == 4 then
          enemyW = 800
          enemyH = 200
        end
        if intersects({xPos = enemy.getPositionX(), yPos = enemy.getPositionY(), width = enemyW, height = enemyH}, {xPos = bullet.getPositionX(), yPos = bullet.getPositionY(), width = 2, height = 5}) then
          if currentLevel == 4 then
            bossHP = bossHP - 1
            table.remove(bulletList, index)  
            if bossHP <= 0 then
              table.remove(listabls, i) -- esse blip "morre" 
              return
            end
          else
            table.remove(listabls, i) -- esse blip "morre" 
            table.remove(bulletList, index) -- esse blip "morre" 
            mqtt_client:publish("apertou-tecla", "kill")
            return -- assumo que apenas um blip morre
          end
        end
      end
      
      for index, bomb in ipairs(bombs) do 
        if intersects({xPos = enemy.getPositionX(), yPos = enemy.getPositionY(), width = enemyW, height = enemyH}, {xPos = bomb.getPositionX(), yPos = bomb.getPositionY(), width = 10, height = 10}) then
          table.remove(listabls, i)
          bomb.setSpeedX(math.random(-5, 5))
          bomb.setSpeedY(math.random(-5, 5))
        end
      end
    end
    
end
  

function love.draw()
  love.graphics.print("Total bullets: " .. totalBullets, 10, 10)
  love.graphics.print("Enemies left: " .. #listabls, 10, 30)

  player.draw()
  for i = 1,#listabls do
    listabls[i].draw()
  end
  
  for i = 1,#bombs do
    bombs[i].draw()
  end
  
  for i = 1, #bulletList do
    bulletList[i].draw()
  end
end


function startGame()
  love.keyboard.setKeyRepeat(true)
  player =  newplayer()
  totalBullets = 100
  megaShots = 3
  bombsAmount = 10
  listabls = {}
  bombs = {}
  bulletList = {}    
  for i = 1, enemyAmount do
    listabls[i] = newblip(i + topSpeed)
  end
end

function nextlevel()
  currentLevel = currentLevel + 1
  
  if currentLevel == 4 then
    topSpeed = topSpeed -1
    minSpeed = minSpeed -1
    enemyAmount = 1
    fallSpeed = fallSpeed + 5
  else
    topSpeed = topSpeed + 1
    minSpeed = minSpeed + 1
    enemyAmount = enemyAmount + 5
    fallSpeed = fallSpeed + 5
  end
  startGame()
end
  
function love.update(dt)
  player.update(dt)
  mqtt_client:handler()
  checkCollision()
  if #listabls == 0 then 
    love.window.showMessageBox("Vitória", "Você ganhou! Aperte OK para próxima fase", enterbutton)
    
    nextlevel()
      
  end
  for i, enemy in ipairs(listabls) do
    enemy.update()
    if enemy.getPositionY() >= love.graphics.getHeight() then
      love.window.showMessageBox("Derrota", "Aperte OK para tentar de novo.", enterbutton)
      startGame()
    end
  end
  
  if totalBullets == 0 then
    love.window.showMessageBox("Sem Balas!", "Aperte OK para tentar de novo.", enterbutton)
    startGame()
  end
end

function intersects(rect1, rect2)
  if rect1.xPos < rect2.xPos and rect1.xPos + rect1.width > rect2.xPos and
     rect1.yPos < rect2.yPos and rect1.yPos + rect1.height > rect2.yPos then
    return true
  else
    return false
  end
end

function mqttcb(topic, message)
  if message == 'left' then
    playerX = playerX - 40
  end
  if message == 'right' then
    playerX = playerX + 40
  end
  if message == 'tiro' then
    table.insert(bulletList,shoot(playerX + 38, 500))
  end
  if message == 'timeout' then
    love.window.showMessageBox("Fim do tempo!", "Aperte OK para tentar de novo.", enterbutton)
    startGame()
  end
  
end

function love.keypressed(key)
  if key == 'a' then
    nextlevel()
  end
  if key == 'right' then
    playerX = playerX + 20
  end
end

function wait(seconds)
  local start = os.time()
  repeat until os.time() > start + seconds
end


  