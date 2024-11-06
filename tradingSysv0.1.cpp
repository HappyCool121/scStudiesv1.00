#include "sierrachart.h"
#include <cmath>
SCDLLName("tradeSys1")



SCSFExport scsf_tradeSys1(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_nBars = sc.Input[0];
    SCInputRef i_drawVA = sc.Input[1];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

    
    SCSubgraphRef s_VAH = sc.Subgraph[2];
    SCSubgraphRef s_VAL = sc.Subgraph[3];
    SCSubgraphRef s_vPOC = sc.Subgraph[4];
    SCSubgraphRef s_buyEntry = sc.Subgraph[5];
    SCSubgraphRef s_sellEntry = sc.Subgraph[6];


    sc.MaintainVolumeAtPriceData = 1;

    //sc.UpdateAlways = 1;

    sc.AutoLoop = 1;


    if(sc.SetDefaults)
    {
        sc.GraphName = "TradeSys1";
        sc.GraphRegion = 0;

        i_nBars.Name = "Number of bars to calculate?";
        i_nBars.SetInt(4);

        i_drawVA.Name = "Change to 1 to draw value area";
        i_drawVA.SetInt(0);

/*
        s_VAH.Name = "VAH";
        s_VAH.PrimaryColor = RGB(0, 240, 0);
        s_VAH.LineStyle = LINESTYLE_SOLID;
        s_VAH.LineWidth = 1;

        
        s_VAL.Name = "VAL";
        s_VAL.PrimaryColor = RGB(240, 0, 0);
        s_VAL.LineStyle = LINESTYLE_SOLID;
        s_VAL.LineWidth = 1;
*/
        s_vPOC.Name = "VPOC";
        s_vPOC.PrimaryColor = RGB(255, 255, 255);
        s_vPOC.LineStyle = LINESTYLE_SOLID;
        s_vPOC.LineWidth = 1;

        s_buyEntry.Name = "Buy Entry";
        s_buyEntry.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        s_buyEntry.LineWidth = 3;
        s_buyEntry.PrimaryColor = RGB(0, 255, 0);

        
        s_sellEntry.Name = "Buy Entry";
        s_sellEntry.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        s_sellEntry.LineWidth = 3;
        s_sellEntry.PrimaryColor = RGB(255, 0, 0);

        sc.AllowMultipleEntriesInSameDirection = false;
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = true;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		sc.SupportAttachedOrdersForTrading = false;

		sc.CancelAllOrdersOnEntriesAndReversals = true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;
		sc.AllowOnlyOneTradePerBar = true;

		sc.MaintainTradeStatisticsAndTradesData = true;
        

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

            //recalculate all essential values
            static SCDateTimeMS LastExecutionTime = sc.CurrentSystemDateTimeMS; // Start with current time

            // Define the time interval (in milliseconds) for the block execution (e.g., 1000ms = 1 second)
            int IntervalMS = 200; // Set your desired interval in milliseconds

            // Get the current system time

            SCDateTimeMS CurrentTime = sc.CurrentSystemDateTimeMS;

            // Calculate the time difference in milliseconds
            int64_t TimeElapsed = (CurrentTime - LastExecutionTime).GetMillisecond();

            // Check if the elapsed time is greater than or equal to the specified interval
            //if (TimeElapsed >= IntervalMS) {

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
            
            int priceVolumeArray[200][2] = {{}};
            int* ptr= &priceVolumeArray[0][0];

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

                //priceVolumeArray[i][1] = totalVolumeAtPrice;
                //priceVolumeArray[i][0] = price;

                *(ptr + (2 * i)) = price;
                *(ptr + (2 * i) + 1) = totalVolumeAtPrice;

                cVolume = *(ptr + (2 * i) + 1);

                if (cVolume > vpoc){

                        vpoc = cVolume;
                        POCinticks = price;
                        POC = sc.TicksToPriceValue(price);

                        targetIndex = i;
                }

                totalvolume += totalVolumeAtPrice;


            }

            
            //msg.Format("ptr test %d", *(ptr + 2), priceVolumeArray[2][0]);
            //sc.AddMessageToLog(msg, 1);

                        
            msg.Format("vpoc: %d, %d, %d %.02f", *(ptr + 2 * targetIndex + 1), vpoc, totalvolume, POC);
            sc.AddMessageToLog(msg, 1);


        int VAHinticks = POCinticks;
        int VALinticks = POCinticks;

        if (i_drawVA.GetInt() == 1){

            VAvol = totalvolume * 0.7;
            volumeCount = 0;

            CountUp = targetIndex + 1;
            CountDown = targetIndex -  1;

            int* pCountUp = &priceVolumeArray[targetIndex][1];
            int* pCountDown = &priceVolumeArray[targetIndex][1];


            while (volumeCount < VAvol && (CountUp < priceRange || CountDown > 0)) {
                
                if (CountUp < priceRange) {
                    volumeCount += *pCountUp;
                    CountUp ++;
                    pCountUp = pCountUp + 2;
                    VAHinticks++;
                    //finalcountUp = CountUp;
                }

                else if (CountDown > 0){
                    volumeCount += *pCountDown;
                    pCountDown = pCountDown - 2;
                    CountDown--;
                    VALinticks--;
                    //finalcountDown = CountDown;
                }
            
            }

            msg.Format("VAH: %.02f, VAL; %.02f", sc.TicksToPriceValue(VAHinticks), sc.TicksToPriceValue(VALinticks));
            sc.AddMessageToLog(msg, 1);
        
        }

            //VAL = priceVolumeArray[finalcountDown][0];
            //VAH = priceVolumeArray[finalcountUp][0];

            updateCounter++;

            msg.Format("COUNTER: %d", updateCounter);
            sc.AddMessageToLog(msg, 1);

            LastExecutionTime = CurrentTime;

            s_vPOC[sc.Index] = POC;

        if (i_drawVA.GetInt() == 1) {
            s_VAH[sc.Index] = sc.TicksToPriceValue(VAHinticks);
            s_VAL[sc.Index] = sc.TicksToPriceValue(VALinticks);
        }

        static float preVpoc = 0;
        static int prevBar = 0;
        int currentBar = sc.Index;
        static int cTrend = 0;
        static int buyCount = 0;
        static int sellCount = 0;


    if (currentBar > prevBar){

        if (POC > preVpoc && cTrend == -1){
            s_buyEntry[sc.Index] = sc.Low[sc.Index];
            cTrend = 1;
            s_SCNewOrder NewOrder;
            NewOrder.OrderQuantity = 1;
            NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
            NewOrder.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
            sc.BuyEntry(NewOrder);

            sellCount++;

        }

        else if (POC < preVpoc && cTrend == 1){
            s_sellEntry[sc.Index] = sc.High[sc.Index];
            cTrend = -1;

     		s_SCNewOrder NewOrder;
            NewOrder.OrderQuantity = 1;
            NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
            NewOrder.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
            sc.SellEntry(NewOrder);

            buyCount++;
            
        }

        else if (POC == preVpoc){
            cTrend = 0;

        }

        prevBar = currentBar;
        preVpoc = POC;

    }

        
            //}
            
            
           //updateCounter = 0;

    //}

    

}
