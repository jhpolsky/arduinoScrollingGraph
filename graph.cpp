void page2()
{

  if (millis() - lastmillis >= 10000) { //Only do this every 10s

    if (newpage == 1) { //If we just started
      tft.fill(rgb(0, 0, 0)); //Clear the LCD
      newpage = 0;
      sensor = strtof (temp1, NULL); //Do this just in case they're empty
      sensor *= 100;

      xmax = float(scrmax) / 100; //Axis labels
      xmin = float(scrmin) / 100;
      xmid = (xmax + xmin) / 2;
    }

        lastmillis = millis(); // Update lastmillis
        tft.fill(rgb(0, 0, 0)); //Blank the screen
        tft.setTextSize(1); //Smallest text size
        tft.setTextColor(GREY, BLACK); 

        tft.setCursor(105, 0);  //Print axis labels
        tft.print(xmax);
        tft.setCursor(105,60);
        tft.print(xmid);
        tft.setCursor(105,120);
        tft.print(xmin);

        tft.setTextColor(RED, BLACK);  //Print temp and last update time
        tft.setCursor(1, 3);
        tft.print(temp1);
        tft.setCursor(27, 3);
        tft.print("C ");
        tft.print(time1);

        tft.drawLine(0,64,105,64,GREY); //Draw axis lines
        tft.drawLine(0,0,105,0,GREY);
        tft.drawLine(0,127,105,127,GREY);


    for (int i = 1; i<103; i++) //drawArray is raw temp readings, we need to scale to LCD height
    {
        scaleddrawArray[i] = map(drawArray[i], scrmin, scrmax, 0, 128);
    }
    for (int i = 2; i<103; i++)
    {
        tft.drawLine (i-1, 128-scaleddrawArray[i-1], i, 128-scaleddrawArray[i], RED ); //Draw that fucker
    }
        tft.copy_buffer(); //LCD doesn't do nuffin till this
  }
}

/*This is in the setup section:*/

Particle.subscribe("Temperature", tempHandler, MY_DEVICES); 

/*And this all works, even if I go to a different "page" on the LCD for a few minutes, because this runs any time the other Photon sends a new temperature value:*/

void tempHandler(const char *event, const char *data) //Particle.subscribe events
{
detachInterrupt(D5); //Stop listening for button presses cause they'll break things
strcpy (temp1, data); //Assign the data we got from the new temp info to "temp1" as a string
strcpy (time1, (Time.format(Time.now(), "%l:%M:%S %p"))); //Time1 is the last time we got an update

    sensor = strtof (temp1, NULL); //"sensor" is a float version of temp1
    sensor *= 100; //Multiply it by 100 because reasons
    if (scrmax == 0) {scrmax = sensor + 100;} //If this is the first time, set the graph ranges to + or - 1C
    if (scrmin == 0) {scrmin = sensor - 100;}
    if (sensor > scrmax) { scrmax = sensor; } //Auto-ranging of the graph y axis
    if (sensor < scrmin) { scrmin = sensor; }
    xmax = float(scrmax) / 100; //Graph axis labels
    xmin = float(scrmin) / 100;
    xmid = (xmax + xmin) / 2;


  if (firstvalue == 1)  //If this is the first time
  {
    for(int i(0); i < 105; ++i)
     {
       drawArray[i] = sensor; //Fill the whole array with the current temp value so we start off with one straight line
     }
    firstvalue = 0;
  }

  drawArray[104] = sensor; //Set the newest value in the array to the current temp
  for(int i(0); i < 105; ++i)
  {
     tempArray[i-1] = drawArray[i]; //Then shift that array one to the left, for the scrolling effect
   }
  for(int i(0); i < 105; ++i)
  {
     drawArray[i] = tempArray[i]; //Then make a note of it
   }
  attachInterrupt(D5, btnpush, FALLING); //And now we can start listening for button presses again
}
