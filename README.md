#ESP8266RelayServer 

The Linknode R4 is a low cost 4-relay, ESP8266 Wifi enabled, Arduino compatible relay controller from Link Sprite. 

Here is the official wiki from LinkSprite.
http://www.linksprite.com/wiki/index.php5?title=LinkNode_R4:_Arduino-compatible_WiFi_relay_controller

Here is a link to the one I ordered on Amazon:
https://www.amazon.com/gp/product/B01FUC9U58

The default program loaded from the factory is intended to be used with the linksprite.io website to allow controlling the relays from anywhere on the internet. It does this by having the Linknode R4 "poll" the linksprite.io website every 1 second to determine if it should change its state. When the server indicates a state change, the Linknode sets the appropriate relay on or off as instructed. 

Basically you go to http://linksprite.io and create an account, then set up devices using a registration code and a device ID for each relay, then you enter that information in to the Linknode's "Arduino Sketch" (sample in the linksprite wiki) and turn it on. It will continually poll the service waiting for something to happen.
c
I was never actually able to get it working however. For some reason my Linknode was never able to successfully get commands back from the server. I don't know if the Sketch I had was out of date with what the server was expecting, or if I just wasn't getting it set up right, or if there was a bug in the server software.

I didn't spend too much time trying to figure it out though, because I didn't really like how it worked anyway. It relies on the linksprite.io website always being available (who knows how long it will be around and free), and the fact that it is polling means there will be up to a 1 second lag from the time a command is given to the time the relay responds. Not to mention creating a lot of unnecessary traffic. 

I wanted a better way. Something that was self-reliant, responsive, and efficient. So rather than having the Linknode continually polling a distant server, I turned it in to a server itself, with its own REST api. This project is the result of that effort.

##A Better Way

The file ESP8266RelayServer.ino in this repository contains the Arduino sketch which will program the Linknode R4 to be a Wifi webserver and respond to http GET and POST requests. The protocol is fairly simple. You make http post requests to activate the relays and GET requests to get the status. 
Any number of Linknode R4 servers can be attached to your local network. You will need to create some software to front-end them however. 

In order to use this Sketch, 

1. Follow the steps in the Linksprite R4 wiki to download the Arduino IDE and add the appropriate board definition, then load up the ESP8266RelayServer.ino file from this git repository.
2. Modify the ssid and password constants to point to your wifi network.
3. Compile and upload the program to the Linknode R4. You will need a TTL Serial cable. The cheapest I found was this one on Amazon: https://www.amazon.com/gp/product/B00QT7LQ88

Use the Serial Monitor capability in the Arduino IDE to watch the output when the Linknode R4 boots up. It will show the assigned IP and MAC Address. 

You can use a tool such as Postman (https://www.getpostman.com/) to test the API to make sure it is working.

In my case, I have an ASP.NET Core webserver running in-house that is exposed to the internet which I can access from anywhere. That server is running an app with my UI, then it talks to the Linknode's API on the back end to control the relays. This allows me to secure it in any way I see fit, as well as providing whatever type of UI I might want. At some point I may post that code here as well, but at the moment it's not quite ready for prime time.

The other alternative if you aren't concerned about security is to open up your firewall to send requests directly from the internet to the Linknode. Note however that this method provides NO security and anyone that happens across your device on the internet will be able to control it. Maybe you want that, but be warned.

By using this method, the lag time from POST to relay action can be measured in milliseconds. Much more consistent than using a public service.

Suggestions, enhancements, comments, bug reports welcome.

##API Documentation

For all reference to relay#, use the values 0 to 3 to indicate which relay.

##Turn on specified relay
**POST api/relay/_&lt;relay#&gt;_/on**

Response: 200
{ "message": "Relay _&lt;relay#&gt;_ ON" }

##Turn off specified relay
**POST api/relay/_&lt;relay#&gt;_/off**

Response: 200
{ "message": "Relay _&lt;relay#&gt;_ OFF" }

##Turn on all relays
**POST api/relay/all/on**

Response: 200
{ "message": "All Relays = ON" }

##Turn off all relays
**POST api/relay/all/off**

Response: 200
{ "message": "All Relays = OFF" }

##Return the current status of all relays
**GET api/relay**

Response:  200
{ "relays": [ _state0_, _state1_, _state2_, _state3_ ] }
Where _stateN_ = 0 for off, 1 for on.

For any request other than the ones listed above, the response status will be 400 - Bad Request.
Response body:
{ "message": "Invalid request" }
