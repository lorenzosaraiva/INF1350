wificonf = { ssid = "Lorenzoâ€™s iPhone",
pwd = "12345678",
got_ip_cb = function (con)
        print ("meu IP: ", con.IP) end,
    save = false}


wifi.setmode(wifi.STATION)
wifi.sta.config(wificonf)



