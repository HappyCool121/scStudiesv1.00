

//I FUCKING HATE CPP 

    #include "sierrachart.h"
    SCDLLName("ONIv1")

    SCSFExport scsf_ONI1(SCStudyInterfaceRef sc)
    {
        SCString msg;

    //inputs for the start date and time of the calculation of ON inventory 
    
    SCInputRef i_year = sc.Input[0];
    SCInputRef i_month = sc.Input[1];
    SCInputRef i_sday = sc.Input[2];
    SCInputRef i_eday = sc.Input[3];
    SCInputRef i_shour = sc.Input[4];
    SCInputRef i_ehour = sc.Input[5];
    SCInputRef i_sminute = sc.Input[6];
    SCInputRef i_eminute = sc.Input[7];
    SCInputRef i_sSecond = sc.Input[8];
    SCInputRef i_eSecond = sc.Input[9];

    //display on the top right hand corner of chart 
	SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];
	SCInputRef Input_HorizontalPosition = sc.Input[11];
	SCInputRef Input_VerticalPosition = sc.Input[12];

    sc.MaintainVolumeAtPriceData = 1;

    if (sc.SetDefaults)
    {
        sc.GraphName = "ONInventory";
        sc.GraphRegion = 0;
        
        i_year.Name = "Year";
        i_year.SetInt(2024);
        
        i_month.Name = "Month";
        i_month.SetInt(10);

        i_sday.Name = "Start Date";
        i_sday.SetInt(24);

        i_eday.Name = "End Date";
        i_eday.SetInt(25);

        i_shour.Name = "Start Hour";
        i_shour.SetInt(16);
        
        i_ehour.Name = "End Hour";
        i_ehour.SetInt(9);

        i_sminute.Name = "Start Minutes";
        i_sminute.SetInt(15);
        
        i_eminute.Name = "End Minutes";
        i_eminute.SetInt(25);

        i_sSecond.Name = "Start Seconds";
        i_sSecond.SetInt(0);

        i_eSecond.Name = "End Seconds";
        i_eSecond.SetInt(0);

        Subgraph_TextDisplay.Name = "Text Display";
		Subgraph_TextDisplay.LineWidth = 10;
		Subgraph_TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Subgraph_TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
		Subgraph_TextDisplay.SecondaryColor = RGB(255, 255, 255);
		Subgraph_TextDisplay.SecondaryColorUsed = true;
		Subgraph_TextDisplay.DisplayNameValueInWindowsFlags = 0;

		Input_HorizontalPosition.Name.Format("Horizontal Position From Left (1-%d)", static_cast<int>(CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION));
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, static_cast<int>(CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION));

		Input_VerticalPosition.Name.Format("Vertical Position From Bottom (1-%d)", static_cast<int>(CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION));
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, static_cast<int>(CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION));


        return;
    }

    //set the start and end date of the calculation
    SCDateTime startDate;
    startDate.SetDateTimeYMDHMS(i_year.GetInt(), i_month.GetInt(), i_sday.GetInt(), i_shour.GetInt(), i_sminute.GetInt(), i_sSecond.GetInt());

    int syear, smonth, sday, shour, sminute, sseconds;
    startDate.GetDateTimeYMDHMS(syear, smonth, sday, shour, sminute, sseconds);


    SCDateTime endDate; 
    endDate.SetDateTimeYMDHMS(i_year.GetInt(), i_month.GetInt(), i_eday.GetInt(), i_ehour.GetInt(), i_eminute.GetInt(), i_eSecond.GetInt());

    int eyear, emonth, eday, ehour, eminute, eseconds;
    endDate.GetDateTimeYMDHMS(eyear, emonth, eday, ehour, eminute, eseconds);


    //finding the index of the bar at the start date and time
    int startBarIndex = 0;
    int endbarindex = 0;

    for(int i = 0; i< sc.ArraySize; i++){

        SCDateTime cBarDateTime = sc.BaseDateTimeIn[i];

        int Year, Month, Day, Hour, Minute, Second;

        cBarDateTime.GetDateTimeYMDHMS(Year, Month, Day, Hour, Minute, Second);
        
        if(Year == syear && Month == smonth && Hour == shour && Minute == sminute && Second == sseconds) {
            
            startBarIndex = i;
            
            //msg.Format("Found bar at index: %d, DateTime: %d", startBarIndex, targeTime);
            //sc.AddMessageToLog(msg, 1);
        }

        if (Year == eyear && Month == emonth && Day == eday && Hour == ehour && Minute == eminute && Second == eseconds) {

            endbarindex = i;

            break;
        }
    }

    if(endbarindex == 0){
        endbarindex = sc.ArraySize;
        
    }

    //print the values 
    msg.Format("start index: %d, %d, %d, %d", startBarIndex, shour, sminute, sseconds);
    sc.AddMessageToLog(msg, 1);

    msg.Format("end index: %d, %d, %d, %d", endbarindex, ehour, eminute, eseconds);
    sc.AddMessageToLog(msg, 1);

    msg.Format("array size: %d", sc.ArraySize);
    sc.AddMessageToLog(msg, 1);


    //before we start anything, we need the overnight highs and lows so that we can get the volume between these prices
    float &ONH = sc.GetPersistentFloat(0);
    float &ONL = sc.GetPersistentFloat(1);
    float &yClose = sc.GetPersistentFloat(2);
    float &Open = sc.GetPersistentFloat(3);

    //sc.GetOHLCForDate(sc.BaseDateTimeIn[sc.ArraySize-1], Open, ONH, ONL, yClose);

    Open = sc.Open[startBarIndex];
    yClose = sc.Close[endbarindex];
    ONH = 0;
    ONL = 999999;

    for (int i = startBarIndex; i <= endbarindex; i++){

        float cHigh = sc.High[i];
        float cLow = sc.Low[i];

        if (cHigh > ONH) {

            ONH = cHigh;

        }

        if (cLow < ONL) {

            ONL = cLow;
        }
    }

    msg.Format("ONH: %f, ONL: %f, Open: %f", ONH, ONL, Open);
    sc.AddMessageToLog(msg, 1);


    //to confirm that we have the right values 
    //msg.Format("%f, %f, %f, %f",  Open, ONH, ONL, yClose);
    //sc.AddMessageToLog(msg, 1);


    //convert to integers so that we can use VAP container
    int ONHigh = sc.PriceValueToTicks(ONH);
    int ONLow = sc.PriceValueToTicks(ONL);
    int open = sc.PriceValueToTicks(Open); 
    
    //well use the ON open since its troublesome to get yesterdays close
    //test again
    //msg.Format("%d, %d, %d",  ONHigh, ONLow, open);
    //sc.AddMessageToLog(msg,1);

    //determine range of prices above and below yesterdays close 
    int priceRangeA = (ONHigh - open) + 1;
    int priceRangeB = (open-ONLow) + 1;

    //msg.Format("%d, %d" , priceRangeA, priceRangeB);
    //sc.AddMessageToLog(msg,1);
    

    //now for the volume at price

    //creating array for the volumes at each price above and below, set the size of the array to the range of prices (in ticks)
    int a[priceRangeA] = {};
    int b[priceRangeB] = {};

    //number of elements in each array for the volumes
    int numEA = sizeof(a) / sizeof(a[0]);
    int numEB = sizeof(b) / sizeof(b[0]);

    /*
    how it goes:
    the code will iterate through all prices in pricerangeb, starting at the ONL all the way to the open
    for each price, it will run to find the total volume at each price level
    this total volume at each price level will be appended to an array
    we will sum this array of volumes as the total total volume 
    */


    //float targetPrice = 5845.25;
    //float priceInTicks = sc.PriceValueToTicks(targetPrice); 

    float totalVolumeAtPrice = 0;
    float placeholderPrice = 0;
    float Bvolume = 0;
    float Avolume = 0;


    // Iterate through the bars, from startbar index to end bar index


    for (int price = ONLow, i = 0;  i < numEB; i++, price++){
        
        totalVolumeAtPrice = 0;
        
        s_VolumeAtPriceV2* p_VolumeAtPrice = nullptr;

        unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

        for (int BarIndex = startBarIndex; BarIndex < endbarindex; BarIndex++){
            
            if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                
                // If found, log the volume at that price
                float volumeAtPrice = p_VolumeAtPrice->Volume;
                
                //msg.Format("BARindex: %d, Price: %.2f, Volume: %.0f", BarIndex, price, volumeAtPrice);
                //sc.AddMessageToLog(msg, 1);

                totalVolumeAtPrice = totalVolumeAtPrice + volumeAtPrice;

            }
        
        }    

        //msg.Format("Price: %.2f, Volume: %.0f", sc.TicksToPriceValue(price), totalVolumeAtPrice);
        //sc.AddMessageToLog(msg, 1);
        Bvolume = Bvolume + totalVolumeAtPrice;

    }
    
    for (int price = open, i = 0;  i < numEA; i++, price++){
        
        totalVolumeAtPrice = 0;
        
        s_VolumeAtPriceV2* p_VolumeAtPrice = nullptr;

        unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

        for (int BarIndex = startBarIndex; BarIndex < endbarindex; BarIndex++){
            
            if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                
                // If found, log the volume at that price
                float volumeAtPrice = p_VolumeAtPrice->Volume;
                
                //msg.Format("BARindex: %d, Price: %.2f, Volume: %.0f", BarIndex, price, volumeAtPrice);
                //sc.AddMessageToLog(msg, 1);

                totalVolumeAtPrice = totalVolumeAtPrice + volumeAtPrice;

            }
        
        }    

        //msg.Format("Price: %.2f, Volume: %.0f", sc.TicksToPriceValue(price), totalVolumeAtPrice);
        //sc.AddMessageToLog(msg, 1);
        Avolume = Avolume + totalVolumeAtPrice;

    }
    
    msg.Format("total volume above: %f", Avolume);
    sc.AddMessageToLog(msg, 1);

    msg.Format("down volume %f", Bvolume);
    sc.AddMessageToLog(msg, 1);
    

    float totalVolume = Avolume + Bvolume;
    
    float overnightUpVolumePercentage = ((Avolume/(Bvolume + Avolume))*100);
    msg.Format("volume above percentage %.2f", overnightUpVolumePercentage);
    sc.AddMessageToLog(msg, 1);

    float overnightDownVolumePercentage = ((Bvolume/(Bvolume + Avolume))*100);
    msg.Format("volume above percentage %.2f", overnightDownVolumePercentage);
    sc.AddMessageToLog(msg, 1);

    msg.Format("net long: %.0f", (Avolume - Bvolume));
    sc.AddMessageToLog(msg, 1);

    SCString ValueText;
    ValueText.Format("total volume: %.0f \nup volume: %.0f \ndown volume: %.0f \nup volume percentage: %.02f \ndown volume percentage: %.02f \nON session from: %d to %d", totalVolume, Avolume, Bvolume, overnightUpVolumePercentage, overnightDownVolumePercentage, sday, eday);


    sc.AddAndManageSingleTextDrawingForStudy
        (
            sc,                          // Sierra Chart Interface
            false,                       // Not persistent, meaning the drawing is not saved across chart redraws
            Input_HorizontalPosition.GetInt(),  // Horizontal position of the text
            Input_VerticalPosition.GetInt(),    // Vertical position of the text
            Subgraph_TextDisplay,         // Subgraph properties (like color, style, etc.)
            false,                       // Not selectable
            ValueText,                   // The actual text to display (containing the value or label)
            true                         // Whether to draw the text or update existing text
        );

}





















/*
    int barI = 0;

    SCDateTime firstBarDate = sc.BaseDateTimeIn[barI];

    int Year, Month, Day, Hour, Minute, Second;

    firstBarDate.GetDateTimeYMDHMS(Year, Month, Day, Hour, Minute, Second);

    int totalseconds = firstBarDate.GetTimeInSeconds();



    msg.Format("%d, %d, %d, %d, %d, %d", Year, Month, Day, Hour, Minute, Second, totalseconds);
    sc.AddMessageToLog(msg, 1);

    
*/
            


/*
    if (startBarIndex != -1) {
        msg.Format("Found bar at index: %d, DateTime: %d", startBarIndex, targeTime);
        sc.AddMessageToLog(msg, 2);
    }

    else {
        sc.AddMessageToLog("No bar found for the specified date and time.", 1);


    }
*/


/*  trying to find the end index with calculations, not really worth it
int SecondsPerBar = 0;

n_ACSIL::s_BarPeriod BarPeriod;
sc.GetBarPeriodParameters(BarPeriod);
if (BarPeriod.ChartDataType == INTRADAY_DATA && BarPeriod.IntradayChartBarPeriodType == IBPT_DAYS_MINS_SECS)
{
    SecondsPerBar = BarPeriod.IntradayChartBarPeriodParameter1;
}


int numberOfBars = (7*60*60)/SecondsPerBar;

    //msg.Format("AAAAAAAAAAAAAAAAA%d, %d", numberOfBars, SecondsPerBar);
    //sc.AddMessageToLog(msg, 1);

*/


 //   msg.Format("%d, %d" , numEA, numEB);
 //   sc.AddMessageToLog(msg,1);
/*
    float PlaceholderPrice;

    for(int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++ ){

        s_VolumeAtPriceV2 *p_VolumeAtPrice = nullptr;

        for (int PriceB = ONLow, i = 0; i < priceRangeB; PriceB++, i++) {

            unsigned int InsertionIndex = -1;

            if(sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, PriceB, &p_VolumeAtPrice, InsertionIndex)){

                b[i] += p_VolumeAtPrice->Volume;
                


                msg.Format("Price: %.2f, Volume Found: %.0f", sc.TicksToPriceValue(PriceB), p_VolumeAtPrice->Volume);
                sc.AddMessageToLog(msg, 1);

                PlaceholderPrice = sc.TicksToPriceValue(PriceB);

            }

            else {
                // Log if the function fails to find volume data
            msg.Format("No volume found for Price: %.2f", sc.TicksToPriceValue(PriceB));
            sc.AddMessageToLog(msg, 0);
            }
        }
    
    }
*/





/*
float PlaceholderPrice;
const s_VolumeAtPriceV2 *p_VolumeAtPrice = NULL;



    for(int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++ ){

        for (int PriceB = ONLow, i = 0; i < priceRangeB; PriceB++, i++) {

            if (!sc.VolumeAtPriceForBars->GetVAPElementAtIndex(BarIndex, PriceB, &p_VolumeAtPrice))
			
                break;
            
            b[i] += p_VolumeAtPrice->Volume;

            msg.Format("Price: %.2f, Volume Found: %.0f", sc.TicksToPriceValue(PriceB), p_VolumeAtPrice->Volume);
            sc.AddMessageToLog(msg, 1);

            PlaceholderPrice = sc.TicksToPriceValue(PriceB);


        }

    }

int VAPSizeAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(BarIndex);

for (int VAPIndex = 0; VAPIndex < VAPSizeAtBarIndex; VAPIndex++) {
    if (sc.VolumeAtPriceForBars->GetVAPElementAtIndex(BarIndex, VAPIndex, &p_VolumeAtPrice)) {
        float price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;
        float volume = p_VolumeAtPrice->Volume;
        msg.Format("VAP Element: Price: %.2f, Volume: %.0f", price, volume);
        sc.AddMessageToLog(msg, 1);
    }
}
*/


/*
float PlaceholderPrice;
const s_VolumeAtPriceV2 *p_VolumeAtPrice = NULL;

    for(int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++ ){

        for (int PriceB = ONLow, i = 0; i < priceRangeB; PriceB++, i++) {

            if (!sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, VAPIndex, &p_VolumeAtPrice))
			
                break;
            
            b[i] += p_VolumeAtPrice->Volume;

            msg.Format("Price: %.2f, Volume Found: %.0f", sc.TicksToPriceValue(PriceB), p_VolumeAtPrice->Volume);
            sc.AddMessageToLog(msg, 1);

            PlaceholderPrice = sc.TicksToPriceValue(PriceB);


        }

    }

    */
/*
    
    SCDateTime MySCDateTime; //Locally defined SCDateTime variable.
    //MySCDateTime will contain, with this function call, the specified date time.
    MySCDateTime.SetDateTimeYMDHMS(2024, 10, 24, 16, 30, 0);
    // Your target price level
    float targetPrice = 5760.00;
    float priceInTicks = sc.PriceValueToTicks(targetPrice);
    s_VolumeAtPriceV2* p_VolumeAtPrice = nullptr;
    unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

    // Iterate through the bars
    for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++) {
        
        // Get the bar's date and time
        SCDateTime BarDateTime = sc.BaseDateTimeIn[BarIndex];

        // Only process bars that are after the specified StartDate
        if (BarDateTime >= MySCDateTime) {
            // Try to get the volume at the specific price for the current bar
            if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, priceInTicks, &p_VolumeAtPrice, InsertionIndex)) {
                // If found, log the volume at that price
                float volumeAtPrice = p_VolumeAtPrice->Volume;
                
                msg.Format("BarIndex: %d, Price: %.2f, Volume: %.0f", BarIndex, targetPrice, volumeAtPrice);
                sc.AddMessageToLog(msg, 1);
            }
        }
    }

}

*/