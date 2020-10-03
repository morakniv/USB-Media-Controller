# USB-Media-Controller
USB Media Controller using the ATMEGA32U4 microcontroller with the Arduino IDE. It is recognised as a USB Device in Windows. 
Visual feedback is achieved using a NeoPixel 24 RGB LED Ring. Easy to modify to add the functions you want. 

// Made by E. Selaker - elias@sealker.se 
// Based on code from wolfgang ziegler - (https://wolfgang-ziegler.com/blog/pc-volume-control) with changes made for it to work with my hardware and added functions.  
// Very few comments made by the original author so I've added some to explain what the code is doing. 
// Adding more keys to the code is quite simple as long as they aren't dependent on timing. What I did was to put them in the beginning of the loop. 
// I had a lot of trouble getting my encoder to work as it used a different pinout and method to output the pulses compared to standard rotary encoders.
// I Don't have much experience writing code, an amature at best. Send me message if you find something wrong or code that is poorly written. 

