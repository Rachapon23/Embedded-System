const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html> 
<html> 
<body> 
<div id="demo"> 
<h1>The ESP-32 Update web page without refresh</h1> 
<button type="button" onclick="sendData(11)" style="background: rgb(202, 60, 60); 
height: 40px; width: 100px">LED1 ON</button> 
<button type="button" onclick="sendData(21)" style="background: rgb(202, 60, 60); 
height: 40px; width: 100px">LED2 ON</button>
<button type="button" onclick="sendData(31)" style="background: rgb(202, 60, 60); 
height: 40px; width: 100px">LED3 ON</button> 
<button type="button" onclick="sendData(41)" style="background: rgb(202, 60, 60); 
height: 40px; width: 100px">LED4 ON</button><br><br>
<button type="button" onclick="sendData(10)" style="background: rgb(100,116,255); 
height: 40px; width: 100px">LED1 OFF</button> 
<button type="button" onclick="sendData(20)" style="background: rgb(100,116,255); 
height: 40px; width: 100px">LED2 OFF</button>
<button type="button" onclick="sendData(30)" style="background: rgb(100,116,255); 
height: 40px; width: 100px">LED3 OFF</button>
<button type="button" onclick="sendData(40)" style="background: rgb(100,116,255); 
height: 40px; width: 100px">LED4 OFF</button><br><br> 
State of [LED1, LED2, LED3, LED4] is >> <span id="LEDState">NA, NA, NA, NA</span><br> 
</div> 
<div> 
<br>SHTC-3 sensor : <span id="ADCValue">0</span><br> 
</div> 
<script>
  function ready() {
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() { 
      if(this.responseText) {
        document.getElementById("LEDState").innerHTML = this.responseText;
      }
    }; 
    xhttp.open("GET", "getLED", true);
    xhttp.send();
 }
 function sendData(led) { 
  var xhttp = new XMLHttpRequest(); 
  xhttp.onreadystatechange = function() { 
    if (this.readyState == 4 && this.status == 200) { 
      document.getElementById("LEDState").innerHTML = this.responseText; 
    } 
  }; 
  xhttp.open("GET", "setLED?LEDstate="+led, true); 
  xhttp.send(); 
 } 
 setInterval(function() { 
 // Call a function repetatively with 2 Second interval 
  getData(); 
 }, 2000); //2000mSeconds update rate 
 
 function getData() { 
  var xhttp = new XMLHttpRequest(); 
  xhttp.onreadystatechange = function() { 
    if (this.readyState == 4 && this.status == 200) { 
      document.getElementById("ADCValue").innerHTML = this.responseText; 
    } 
  }; 
  xhttp.open("GET", "readADC", true); 
  xhttp.send(); 
 } 
 document.addEventListener("DOMContentLoaded", ready);
</script> 
<br><a href="https://www.facebook.com/profile.php?id=100007567420275">By 
Rachapon Pongkittisak</a> 
</body> 
</html> 
)=====";
