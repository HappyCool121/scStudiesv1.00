#include "sierrachart.h"
SCDLLName("4barprofilev1")


SCSFExport scsf_4barprofilev1(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_nBars = sc.Input[0];
    SCInputRef i_drawprofile = sc.Input[1];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

    sc.MaintainVolumeAtPriceData = 1;

    sc.UpdateAlways = 1;

    //activate manual looping
    sc.AutoLoop = 0;


    if(sc.SetDefaults)
    {
        sc.GraphName = "4barperiod";
        sc.GraphRegion = 0;

        i_nBars.Name = "Number of bars to calculate?";
        i_nBars.SetInt(4);

        i_drawprofile.Name = "Change to 1 to draw volume profile";
        i_drawprofile.SetInt(0);

        // Add a button input

    }


//for loop used for manual looping
for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++){



        //start and end bar index calculation
        int startBarIndex = 0;
        int endbarindex = 0;

        startBarIndex = sc.ArraySize - i_nBars.GetInt();
        endbarindex = sc.ArraySize;

        //msg.Format("start bar: %d, end bar: %d", startBarIndex, endbarindex);
        //sc.AddMessageToLog(msg, 1);

        //find high and low for the bars

        float &high = sc.GetPersistentFloat(0);
        float &low = sc.GetPersistentFloat(1);

        //get low and high of the past n bars determined by user

        high = sc.High[startBarIndex];
        low = sc.Low[startBarIndex];

        for (int i = startBarIndex; i <= endbarindex; i++){

            float cHigh = sc.High[i];
            float cLow = sc.Low[i];

            if (cHigh > high) {
                high = cHigh;
            }

            if (cLow < low && cLow != 0) {
                low = cLow;
            }
        }

    //    msg.Format("High: %.0f, low: %.0f", high, low);
    //    sc.AddMessageToLog(msg, 1);

        float open = sc.Open[startBarIndex];

        //create empty array for volumes at each price (must be in ticks)
        int HighInTicks = sc.PriceValueToTicks(high);
        int LowInTicks = sc.PriceValueToTicks(low);

        int priceRange = HighInTicks - LowInTicks + 1;

        float array[priceRange] = {};
        float priceArray[priceRange] = {};

        float totalVolumeAtPrice = 0;
        float Placeholder = 0;
        float totalvolume = 0;

        //iterate through the bars to get VAP for the past n number of bars determined by user
        for (int price = LowInTicks, i = 0;  i < priceRange; i++, price++){
                
                totalVolumeAtPrice = 0;
                
                s_VolumeAtPriceV2* p_VolumeAtPrice = nullptr;

                unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

                for (int BarIndex = startBarIndex; BarIndex < endbarindex; BarIndex++){
                    
                    if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                        
                        // If found, log the volume at that price
                        float volumeAtPrice = p_VolumeAtPrice->Volume;
                        
                        //msg.Format("Barindex: %d, Price: %.2f, Volume: %.0f", BarIndex, sc.TicksToPriceValue(price), volumeAtPrice);
                        //sc.AddMessageToLog(msg, 1);

                        totalVolumeAtPrice = totalVolumeAtPrice + volumeAtPrice;
                    }
                
                }   

            array[i] += totalVolumeAtPrice;
            priceArray[i] += price;
            totalvolume += totalVolumeAtPrice;

        }

        float POC = 0;
        int POCinticks = 0;
        int size = sizeof(array) / sizeof(array[0]);
        float vpoc = 0;
        int targetIndex = -1;


        //calculate volume at POC
        for (int i = 0, price = LowInTicks; i < size; i++, price++) {

            float cVolume = array[i];

            if (cVolume > vpoc){
                vpoc = cVolume;
                POCinticks = price;
                POC = sc.TicksToPriceValue(price);

                targetIndex = i;
            }

        }


        //msg.Format("VPOC is %.0f at %.02f, check price: %.02f", vpoc, POC, sc.TicksToPriceValue(priceArray[targetIndex]));
        //sc.AddMessageToLog(msg, 1);

        //to calculate VAH and VAL, using a while loop instead of for loops and if statements since there are too many conditions

        float VAvol = totalvolume * 0.7;

        float VAH = POC;
        float VAL = POC;

        int VAHinticks = POCinticks;
        int VALinticks = POCinticks;

        int CountUp = targetIndex + 1;
        int CountDown = targetIndex - 1;

        int finalcountUp = 0;
        int finalcountDown = 0;

        float volumeCount = vpoc;

        while (volumeCount < VAvol && (CountUp < priceRange || CountDown > 0)) {
            
            if (CountUp < priceRange && array[CountUp] > array[CountDown]) {
                volumeCount += array[CountUp];
                CountUp++;
                VAHinticks++;
                finalcountUp = CountUp;
            }

            else if (CountDown > 0 && array[CountDown] > array[CountUp]){
                volumeCount += array[CountDown];
                CountDown--;
                VALinticks--;
                finalcountDown = CountDown;
            }
            
        }

            //msg.Format("VAH: %.02f, VAL: %.02f, POC: %.02f, high: %.02f low: %0.2f", sc.TicksToPriceValue(VAHinticks), sc.TicksToPriceValue(VALinticks), POC, high, low);
            //sc.AddMessageToLog(msg, 1);

        static int updateCounter = 0;
        updateCounter++;

        if (updateCounter >= 100){
                
                sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  // Delete all user-drawn objects
                sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_ALL, 0);
                updateCounter = 0;


        //drawing the VAL, VAH, POC, High and Low
        
            s_UseTool Tool3;
            Tool3.Clear();
            Tool3.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool3.DrawingType = DRAWING_LINE;
            Tool3.LineNumber =  1;
            Tool3.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
            Tool3.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize];
            Tool3.BeginValue = high;
            Tool3.EndValue = high;
            Tool3.Text = "high";
            Tool3.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool3.Region = 0;
            Tool3.Color = RGB(128, 128, 128);
            Tool3.LineWidth = 2;
            Tool3.AddMethod = UTAM_ADD_OR_ADJUST;
            sc.UseTool(Tool3); 

            
            s_UseTool Tool1;
            Tool1.Clear();
            Tool1.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool1.DrawingType = DRAWING_LINE;
            Tool1.LineNumber =  2;
            Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
            Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize];
            Tool1.BeginValue = low;
            Tool1.EndValue = low;
            Tool1.Text = "low";
            Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool1.Region = 0;
            Tool1.Color = RGB(128, 128, 128);
            Tool1.LineWidth = 2;
            Tool1.AddMethod = UTAM_ADD_OR_ADJUST;
            sc.UseTool(Tool1); 


            s_UseTool Tool2;
            Tool2.Clear();
            Tool2.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool2.DrawingType = DRAWING_LINE;
            Tool2.LineNumber = 3;
            Tool2.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
            Tool2.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 ];
            Tool2.BeginValue = sc.TicksToPriceValue(VAHinticks);
            Tool2.EndValue = sc.TicksToPriceValue(VAHinticks);
            Tool2.Text = "VAH";
            Tool2.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool2.Region = 0;
            Tool2.Color = RGB(0, 180, 0);
            Tool2.LineWidth = 2;
            Tool2.AddMethod = UTAM_ADD_OR_ADJUST;
            sc.UseTool(Tool2); 


            s_UseTool Tool4;
            Tool4.Clear();
            Tool4.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool4.DrawingType = DRAWING_LINE;
            Tool4.LineNumber =  4; 
            Tool4.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
            Tool4.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 ];
            Tool4.BeginValue = sc.TicksToPriceValue(VALinticks);
            Tool4.EndValue = sc.TicksToPriceValue(VALinticks);
            Tool4.Text = "VAL";
            Tool4.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool4.Region = 0;
            Tool4.Color = RGB(0, 180, 0);
            Tool4.LineWidth = 2;
            Tool4.AddMethod = UTAM_ADD_OR_ADJUST;
            sc.UseTool(Tool4); 


            s_UseTool Tool5;
            Tool5.Clear();
            Tool5.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool5.DrawingType = DRAWING_LINE;
            Tool5.LineNumber =  5;
            Tool5.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
            Tool5.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 ];
            Tool5.BeginValue = POC;
            Tool5.EndValue = POC;
            Tool5.Text = "POC";
            Tool5.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool5.Region = 0;
            Tool5.Color = RGB(255, 255, 255);
            Tool5.LineWidth = 2;
            Tool5.AddMethod = UTAM_ADD_OR_ADJUST;
            sc.UseTool(Tool5); 

        //now draw the volume profile 

        if (i_drawprofile.GetInt() == 1){

        //this is to determine the size of the bars for the volume profile
        float ratioArray[priceRange] = {};
        int sizeArray[priceRange] = {};

        for (int i = 0; i < priceRange; i++){
            ratioArray[i] = array[i]/totalvolume;
        }

        float minRatio = ratioArray[0];
        float maxRatio = ratioArray[0];

        for (int i = 0; i < priceRange; i++){
            
            if (ratioArray[i] < minRatio){
                minRatio = ratioArray[i];
            }

            if (ratioArray[i] > maxRatio){
                maxRatio = ratioArray[i];
            }
        }
    

        for (int i = 0; i < priceRange; i++) {
        
        // Normalize the ratio to a scale of 0 to 7, then add 1 to shift it to 1-8 range
            if (maxRatio > minRatio) {
                sizeArray[i] = static_cast<int>(((ratioArray[i] - minRatio) / (maxRatio - minRatio)) * 7 + 1);
            } 
            
            else {
                // If all ratios are the same, assign a default value
                sizeArray[i] = 1; // Or handle it differently based on your requirements
            }

        // Log the size for debugging
        //msg.Format("%d", sizeArray[i]);
        //sc.AddMessageToLog(msg, 1);
        
        }


        for (int i = 0; i < priceRange; i++){

            if (i < finalcountDown || i > finalcountUp && i_drawprofile.GetInt() == 1){
                
                sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  // Delete all user-drawn objects            
                s_UseTool Tool; 
                Tool.Clear();
                Tool.DrawingType = DRAWING_RECTANGLEHIGHLIGHT;
                Tool.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9]; 
                Tool.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 - sizeArray[i]];
                Tool.BeginValue = (sc.TicksToPriceValue(priceArray[i]) - 0.125); 
                Tool.EndValue = (sc.TicksToPriceValue(priceArray[i]) + 0.125); 
                Tool.Color = RGB(255, 255, 255);  // Red  
                Tool.LineWidth = 1; //To see the outline this must be 1 or greater.  
                Tool.SecondaryColor = RGB(128, 128, 128); 
                Tool.TransparencyLevel = 50; 
                Tool.AddMethod = UTAM_ADD_OR_ADJUST; 
                Tool.LineNumber = 200 + i;
                Tool.AddAsUserDrawnDrawing = 1; 
                //Tool.Text.Format("%.0f", array[i]);
                //Tool.FontSize = 10;
                //Tool.TextAlignment = DT_RIGHT;
                Tool.ChartNumber = sc.ChartNumber;
                sc.UseTool(Tool); 
            }


            else if (i >= finalcountDown && i <= finalcountUp && i_drawprofile.GetInt() == 1){
                
                sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  // Delete all user-drawn object
                s_UseTool Tool; 
                Tool.Clear();
                Tool.DrawingType = DRAWING_RECTANGLEHIGHLIGHT;
                Tool.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9]; 
                Tool.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 - sizeArray[i]];
                Tool.BeginValue = (sc.TicksToPriceValue(priceArray[i]) - 0.125); 
                Tool.EndValue = (sc.TicksToPriceValue(priceArray[i]) + 0.125); 
                Tool.Color = RGB(255, 255, 255);  // Red  
                Tool.LineWidth = 1; //To see the outline this must be 1 or greater.  
                Tool.SecondaryColor = RGB(0, 180, 0); 
                Tool.TransparencyLevel = 50; 
                Tool.AddMethod = UTAM_ADD_OR_ADJUST; 
                Tool.LineNumber = 100 + i;
                Tool.AddAsUserDrawnDrawing = 1; 
                //Tool.Text.Format("%.0f", array[i]);
                //Tool.FontSize = 10;
                //Tool.TextAlignment = DT_RIGHT;
                Tool.ChartNumber = sc.ChartNumber;
                sc.UseTool(Tool); 
            

                
                s_UseTool Tool7; 
                Tool7.Clear();
                Tool7.DrawingType = DRAWING_TEXT;
                Tool7.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9]; 
                Tool7.BeginValue = (sc.TicksToPriceValue(priceArray[i]) + 0.08); 
                Tool7.Color = RGB(255, 255, 255);  //white
                Tool7.AddMethod = UTAM_ADD_OR_ADJUST; 
                Tool7.LineNumber = 300 + i;
                Tool7.FontSize = 8;
                Tool7.AddAsUserDrawnDrawing = 1; 
                Tool7.ChartNumber = sc.ChartNumber;
                Tool7.Text.Format("%.0f", array[i]);
                sc.UseTool(Tool7); 
            }

        }

        }

        }

    }

}

    






/*
1. get bar index of previous 4 bars (or otherwise stated by the user in teh input)
2. obtain the high, low and open of the 4 bars
3. iterate through the 4 bars and get an array of volumes at each price level for teh range of the four bars
3. calculate the poc
4. calculate the volume traded above and below
5. calculate the value area volume
6. calculate the VAL and VAH  


    for (int i = 0; i < priceRange; i++){

        if (volumeCount >= VAvol) {
            break;
        }

        if (array[CountUp] > array[CountDown]) {

            volumeCount += array[CountUp];
            VAHinticks += 1;
            CountUp++;

        }

        else {

            volumeCount += array[CountDown];
            VALinticks -= 1;
            CountDown--;

        }

        if (CountUp >= priceRange -1) {
            
        }

    }


*/

