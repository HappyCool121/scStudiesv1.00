#include "sierrachart.h"
SCDLLName("4barprofilev2")


SCSFExport scsf_4barprofilev2(SCStudyInterfaceRef sc)
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

        static int updateCounter = 0;
        //Defining all variables and arrays before loop
        int startBarIndex = 0;
        int endbarindex = 0;

        float &high = sc.GetPersistentFloat(0);
        float &low = sc.GetPersistentFloat(1);
        float open = sc.Open[startBarIndex];

        int HighInTicks = 0;
        int LowInTicks = 0;

        int priceRange =   1;

        float totalVolumeAtPrice = 0;
        float Placeholder = 0;
        float totalvolume = 0;

        float POC = 0;
        int POCinticks = 0;
        //int size = 0;
        float vpoc = 0;
        int targetIndex = -1;
        float cVolume = 0;

        float VAvol = 0;
        float VAH = POC;
        float VAL = POC;

        int VAHinticks = POCinticks;
        int VALinticks = POCinticks;

        int CountUp = 0;
        int CountDown = 0;

        int finalcountUp = 0;
        int finalcountDown = 0;

        float volumeCount = vpoc;

        //float ratioArray[priceRange] = {};
        //int sizeArray[priceRange] = {};

        float minRatio = 0;
        float maxRatio = 0;
        
        
        float array[200] = {};
        float priceArray[200] = {};
        float ratioArray[200] = {};
        int sizeArray[200] = {};

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
        //Tool3.Text = "high";
        //Tool3.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool3.Region = 0;
        Tool3.Color = RGB(128, 128, 128);
        Tool3.LineWidth = 2;
        Tool3.AddMethod = UTAM_ADD_OR_ADJUST;

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
        //Tool1.Text = "low";
        //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool1.Region = 0;
        Tool1.Color = RGB(128, 128, 128);
        Tool1.LineWidth = 2;
        Tool1.AddMethod = UTAM_ADD_OR_ADJUST;

        s_UseTool Tool2;
        Tool2.Clear();
        Tool2.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool2.DrawingType = DRAWING_LINE;
        Tool2.LineNumber = 3;
        Tool2.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
        Tool2.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 ];
        //Tool2.Text = "VAH";
        //Tool2.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool2.Region = 0;
        Tool2.Color = RGB(0, 255, 0);
        Tool2.LineWidth = 2;
        Tool2.AddMethod = UTAM_ADD_OR_ADJUST;

        s_UseTool Tool4;
        Tool4.Clear();
        Tool4.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool4.DrawingType = DRAWING_LINE;
        Tool4.LineNumber =  4; 
        Tool4.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
        Tool4.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9 ];
        //Tool4.Text = "VAL";
        //Tool4.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool4.Region = 0;
        Tool4.Color = RGB(0, 255, 0);
        Tool4.LineWidth = 2;
        Tool4.AddMethod = UTAM_ADD_OR_ADJUST;

        s_UseTool Tool5;
        Tool5.Clear();
        Tool5.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool5.DrawingType = DRAWING_LINE;
        Tool5.LineNumber =  5;
        Tool5.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - 8];
        Tool5.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9];
        //Tool5.Text = "POC";
        //Tool5.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool5.Region = 0;
        Tool5.Color = RGB(255, 255, 255);
        Tool5.LineWidth = 2;
        Tool5.AddMethod = UTAM_ADD_OR_ADJUST;



    //for loop used for manual looping
    for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++){

        //do calculations only after every 10 ticks
        
        updateCounter++;

        //if (updateCounter > 1) {

            for (int i = 0; i < 200; i++){
                array[i] = 0;
                priceArray[i] = 0;
                ratioArray[i] = 0;
                sizeArray[i] = 0;
            }

            //on reset, we aim to clear all arrays and ensure that all drawings are removed 
            
            //clear drawings
            //sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  
            //sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_ALL, 0);

            //recalculate all essential values
            startBarIndex = sc.ArraySize - i_nBars.GetInt();
            endbarindex = sc.ArraySize;

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

            LowInTicks = sc.PriceValueToTicks(low);
            HighInTicks = sc.PriceValueToTicks(high);

            priceRange = HighInTicks - LowInTicks + 1;

            //obtain volumes for every price level within range of n bars

            totalvolume = 0;

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

                array[i] = totalVolumeAtPrice;
                priceArray[i] = price;
                totalvolume += totalVolumeAtPrice;
                //msg.Format("VALUES FROM VAP LOOP %.0f %.0f %.0f", array[i], priceArray[i], totalvolume);
                //sc.AddMessageToLog(msg, 1);


            }

            //msg.Format("random price: %0.2f with %0.2f", priceArray[7], array[7]);
            //sc.AddMessageToLog(msg, 1);

            //recalculate volume at POC

            //size = sizeof(array) / sizeof(array[0]);
            vpoc = 0;
            POCinticks = 0;
            POC = 0;
            targetIndex = 0;

            for (int i = 0, price = LowInTicks; i < priceRange; i++, price++) {

                cVolume = array[i];

                if (cVolume > vpoc){
                    vpoc = cVolume;
                    POCinticks = price;
                    POC = sc.TicksToPriceValue(price);

                    targetIndex = i;
                }

            }

            msg.Format("POC: %.02f VPOC; %.02f", POC, vpoc);
            sc.AddMessageToLog(msg, 1);


            //recalculate VAH and VAL

            VAvol = totalvolume * 0.7;
            volumeCount = 0;
            CountUp = targetIndex + 1;
            CountDown = targetIndex -  1;
            VAHinticks = POCinticks;
            VALinticks = POCinticks;

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

            VAL = sc.TicksToPriceValue(VALinticks);
            VAH = sc.TicksToPriceValue(VAHinticks);

            msg.Format("VAH %.02f, VAL %.02f, POC %.02f% .02fd", VAH, VAL, POC, vpoc, Index);
            sc.AddMessageToLog(msg, 1);

            if (i_drawprofile.GetInt() == 1){

                sc.UseTool(Tool3);         

                sc.UseTool(Tool1); 


                Tool2.BeginValue = sc.TicksToPriceValue(VAHinticks);
                Tool2.EndValue = sc.TicksToPriceValue(VAHinticks);  
                sc.UseTool(Tool2); 
                    

                Tool4.BeginValue = sc.TicksToPriceValue(VALinticks);
                Tool4.EndValue = sc.TicksToPriceValue(VALinticks);
                sc.UseTool(Tool4);
                
                Tool5.BeginValue = POC;
                Tool5.EndValue = POC;
                sc.UseTool(Tool5); 
        
            }

            /*
            if (i_drawprofile.GetInt() == 1){

                for (int i = 0; i < priceRange; i++){
                    
                    ratioArray[i] = array[i]/totalvolume;
                
                }

                minRatio = ratioArray[0];
                maxRatio = ratioArray[0];

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
                        Tool.Color = RGB(0,0,0);  // Red  
                        Tool.LineWidth = 1; //To see the outline this must be 1 or greater.  
                        Tool.SecondaryColor = RGB(128, 128, 128); 
                        Tool.TransparencyLevel = 50; 
                        Tool.AddMethod = UTAM_ADD_ONLY_IF_NEW; 
                        Tool.LineNumber = 200 + i;
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
                        Tool.SecondaryColor = RGB(209, 246, 255); 
                        Tool.TransparencyLevel = 50; 
                        Tool.AddMethod = UTAM_ADD_ONLY_IF_NEW; 
                        Tool.LineNumber = 100 + i;
                        //Tool.Text.Format("%.0f", array[i]);
                        //Tool.FontSize = 10;
                        //Tool.TextAlignment = DT_RIGHT;
                        Tool.ChartNumber = sc.ChartNumber;
                        sc.UseTool(Tool); 
                    

                        s_UseTool Tool7; 
                        Tool7.Clear();
                        Tool7.DrawingType = DRAWING_TEXT;
                        Tool7.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9]; 
                        Tool7.BeginValue = (sc.TicksToPriceValue(priceArray[i] + priceArray[i + 1]))/2; 
                        Tool7.Color = RGB(0, 0, 0);  //white
                        Tool7.AddMethod = UTAM_ADD_OR_ADJUST; 
                        Tool7.LineNumber = 300 + i;
                        Tool7.FontSize = 8; 
                        Tool7.ChartNumber = sc.ChartNumber;
                        Tool7.Text.Format("%.0f", array[i]);
                        //sc.UseTool(Tool7); 
                    }
                
                }

            }*/
            
           updateCounter = 0;

        //}

    }

}