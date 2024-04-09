# LED Arduino weather display

# An easier, simpler way to forecast the weather

# Components:

- Arduino Uno R4 wifi
  I chose this arduino as I would need wireless access to fetch the weather data
- LCD Screen
  I needed an LCD screen to easily display the time and date so the user can know when each forecast is for
- LED strip
  This was neccary so users can easily understand the weather forecast using light and color. (Useful for users with short vision problems)

  # Problem
- People are busy and they don't have time to give their full attention to a weather forecast on the TV or phone.  

  # Use cases
  - As a busy user I want a clear and easy way to quickly recieve important weather infomation so I can be well informed
 
  # Solution
  - Using LEDS I will use color to convey important weather information that can easily be seen from afar so users can multitask while getting ready for the day. I am using an LCD screen to display time and temp and an LED matrix and light strip to display the weather condition. My solution isn't perfect. I didn't what to have to use an LCD display as that would require the user to be close to the device to read. In the future I would replace this with a LED matrix time clock that would make it easier to read from afar. Another issue is that the arduino LED matrix is built in, this means it can be hard to see as a prototype as there are many cables blocking the view. In the future I plan to solder up these cables or replace the LED matrix for an external one.
  - One assumption I made is that the weather API I used was accurate and well suited for Dunedin weather.
 
# Breadboard

![image](https://github.com/mahoneybj/arduino-weather/assets/65274137/b06cbdd3-c2a3-44c0-b569-03e52093989a)
