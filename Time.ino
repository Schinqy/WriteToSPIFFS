void doThisAtEvery(int ms){
    //Serial.println("Checking Time");
 
    if( millis() - lastDoTime >= ms){
        // Must save the lastDoTime
        writeDataToFile();
        lastDoTime = millis();
      }
  
 // Serial.println(myTime);
}
