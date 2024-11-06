#include "sierrachart.h"
#include <cmath>
SCDLLName("4barprofilev3")


SCSFExport scsf_4barprofilev3(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_nBars = sc.Input[0];
    SCInputRef i_drawprofile = sc.Input[1];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

    SCSubgraphRef s_NumHighs = sc.Subgraph[1];

    sc.MaintainVolumeAtPriceData = 1;

    //sc.UpdateAlways = 1;

    sc.AutoLoop = 1;


    if(sc.SetDefaults)
    {
        sc.GraphName = "4barperiodv3";
        sc.GraphRegion = 0;

        i_nBars.Name = "Number of bars to calculate?";
        i_nBars.SetInt(4);

        i_drawprofile.Name = "Change to 1 to draw volume profile";
        i_drawprofile.SetInt(0);

        s_NumHighs.Name = "Number of highs";
        s_NumHighs.PrimaryColor = COLOR_YELLOW;
        s_NumHighs.LineStyle = LINESTYLE_SOLID;

        

        // Add a button input

    }



        static int updateCounter = 0;
        //Defining all variables and arrays before loop
        int startBarIndex = 0;
        int endbarindex = 0;

        float &high = sc.GetPersistentFloat(0);
        float &low = sc.GetPersistentFloat(1);

        int HighInTicks = 0;
        int LowInTicks = 0;

        int priceRange =   1;

        float totalVolumeAtPrice = 0;
        float Placeholder = 0;
        int totalvolume = 0;

        float POC = 0;
        int POCinticks = 0;
        //int size = 0;
        int vpoc = 0;
        int targetIndex = 0;
        int cVolume = 0;

        int VAvol = 0;
        float VAH = POC;
        float VAL = POC;

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

        int counter = 0;
        //float ratioArray[200] = {};
        //int sizeArray[200] = {};



    //for loop used for manual looping
    //for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++){

        //do calculations only after every 10 ticks
        
        //updateCounter++;

        //if (updateCounter > 1) {
            /*
            for (int i = 0; i < 200; i++){
                array[i] = 0;
                priceArray[i] = 0;
                //sizeArray[i] = 0;
            }
            */

            //on reset, we aim to clear all arrays and ensure that all drawings are removed 
            
            //clear drawings
            //sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  
            //sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_ALL, 0);

            //recalculate all essential values
            static SCDateTimeMS LastExecutionTime = sc.CurrentSystemDateTimeMS; // Start with current time

            // Define the time interval (in milliseconds) for the block execution (e.g., 1000ms = 1 second)
            int IntervalMS = 200; // Set your desired interval in milliseconds

            // Get the current system time

            SCDateTimeMS CurrentTime = sc.CurrentSystemDateTimeMS;

            // Calculate the time difference in milliseconds
            int64_t TimeElapsed = (CurrentTime - LastExecutionTime).GetMillisecond();

            // Check if the elapsed time is greater than or equal to the specified interval
            if (TimeElapsed >= IntervalMS) {

                msg.Format("%d", TimeElapsed);
                sc.AddMessageToLog(msg, 1);

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
            vpoc = 0;
            POCinticks = 0;
            POC = 0;
            targetIndex = 0;
            
            int priceVolumeArray[priceRange][2] = {{}};

            for (int price = LowInTicks, i = 0;  i < priceRange; i++, price++){
                
                totalVolumeAtPrice = 0;
                
                s_VolumeAtPriceV2 * p_VolumeAtPrice = nullptr;

                unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

                for (int BarIndex = startBarIndex; BarIndex < endbarindex; BarIndex++){
                    
                    if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                        
                        // If found, log the volume at that price
                        //msg.Format("Barindex: %d, Price: %.2f, Volume: %.0f", BarIndex, sc.TicksToPriceValue(price), volumeAtPrice);
                        //sc.AddMessageToLog(msg, 1);

                        totalVolumeAtPrice += p_VolumeAtPrice->Volume;

                    }
                
                }   
/*
                array[i] = totalVolumeAtPrice;
                priceArray[i] = price;
                totalvolume += totalVolumeAtPrice;
*/
                priceVolumeArray[i][1] = totalVolumeAtPrice;
                priceVolumeArray[i][0] = price;

                cVolume = priceVolumeArray[i][1];

                if (cVolume > vpoc){

                        vpoc = cVolume;
                        POCinticks = price;
                        POC = sc.TicksToPriceValue(price);

                        targetIndex = i;
                }

                totalvolume += totalVolumeAtPrice;


            }

                        
            msg.Format("vpoc: %d, %d, %d %.0f", priceVolumeArray[targetIndex][1], vpoc, totalvolume, POC);
            sc.AddMessageToLog(msg, 1);


                //msg.Format("VALUES FROM VAP LOOP %.0f %.0f %.0f", array[i], priceArray[i], totalvolume);
                //sc.AddMessageToLog(msg, 1);

            // i have a two dimensional array
            // in the rows, we have pointers which point to the price in ticks, which is in the VAP container
            // in the column, (which is just one element) we have pointers which point to the volume, which is also in the VAP container
            // essentially, we have an array of pointers 
            // so we do not have to "chnage" the values in the arrays since they are pointing to memory addresses
            // which contain values that change (price in ticks and volume)

/*
            int totalVolumeAtPrice = 0;


            for (int i = startBarIndex; i < endbarindex; i++){

                for (int price = LowInTicks, j = 0; price < HighInTicks; price++, j++){

                    totalVolumeAtPrice = 0;
                    
                    s_VolumeAtPriceV2 * p_VolumeAtPrice = nullptr;

                    unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart
                    
                    if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(i, price, &p_VolumeAtPrice, InsertionIndex)) {
                    
                        totalVolumeAtPrice += p_VolumeAtPrice->Volume;

                        msg.Format("%d", totalVolumeAtPrice);
                        sc.AddMessageToLog(msg, 1);

                        
                        totalvolume += totalVolumeAtPrice;

                    }

                }

            }

*/   

            VAvol = totalvolume * 0.7;
            volumeCount = 0;
            CountUp = targetIndex + 1;
            CountDown = targetIndex -  1;
            int VAHinticks = POCinticks;
            int VALinticks = POCinticks;
            int* ptr = &priceVolumeArray[0][0];

            while (volumeCount < VAvol && (CountUp < priceRange || CountDown > 0)) {
                
                if (CountUp < priceRange) {
                    volumeCount += array[CountUp];
                    CountUp++;
                    VAHinticks++;
                    finalcountUp = CountUp;
                }

                else if (CountDown > 0){
                    volumeCount += array[CountDown];
                    CountDown--;
                    VALinticks--;
                    finalcountDown = CountDown;
                }
            
            }
            
/*
            VAvol = totalvolume * 0.7;
            volumeCount = 0;
            CountUp = targetIndex + 1;
            CountDown = targetIndex - 1;

            // Use pointers to traverse the priceVolumeArray

            int* ptrUp = &priceVolumeArray[targetIndex + 1][1]; // Start from volume column of targetIndex + 1
            int* ptrDown = &priceVolumeArray[targetIndex - 1][1]; // Start from volume column of targetIndex - 1

            // Continue while there's volume to count and valid range in CountUp/CountDown
            while (volumeCount < VAvol && (CountUp < priceRange && CountDown > 0)) {

                // If CountUp is within bounds and its volume is greater than CountDown's volume, prioritize it
                    
                if (CountUp < priceRange){
                    volumeCount += *ptrUp;  // Add volume at CountUp
                    CountUp++;              // Move to the next price level upwards
                    ptrUp++;                // Move the pointer up
                    finalcountUp = CountUp;
                }

                if (CountDown > 0){
                    volumeCount += *ptrDown; // Add volume at CountDown
                    CountDown--;             // Move to the next price level downwards
                    ptrDown--;               // Move the pointer down
                    finalcountDown = CountDown;
                    
                } 
            }
*/

            VAL = priceVolumeArray[finalcountDown][0];
            VAH = priceVolumeArray[finalcountUp][0];


            msg.Format("VAH: %.0f, VAL; %.0f", sc.TicksToPriceValue(VAHinticks), sc.TicksToPriceValue(VALinticks));
            sc.AddMessageToLog(msg, 1);


            updateCounter++;

            msg.Format("COUNTER: %d", updateCounter);
            sc.AddMessageToLog(msg, 1);

            LastExecutionTime = CurrentTime;

            s_NumHighs[sc.Index] = POC;

            }
            
            
           //updateCounter = 0;

    //}

    

}
