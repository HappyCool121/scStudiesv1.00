#include "sierrachart.h"
#include <cmath>
SCDLLName("tradeSys2")



SCSFExport scsf_tradeSys2(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCSubgraphRef s_buyEntry = sc.Subgraph[0];
    SCSubgraphRef s_sellEntry = sc.Subgraph[1];


    sc.MaintainVolumeAtPriceData = 1;

    //sc.UpdateAlways = 1;

    sc.AutoLoop = 1;


    if(sc.SetDefaults)
    {
        sc.GraphName = "TradeSys2 - HEIKEN ASHI";
        sc.GraphRegion = 0;

        s_buyEntry.Name = "Buy Entry";
        s_buyEntry.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
        s_buyEntry.LineWidth = 3;
        s_buyEntry.PrimaryColor = RGB(0, 255, 0);
        
        s_sellEntry.Name = "Sell Entry";
        s_sellEntry.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
        s_sellEntry.LineWidth = 3;
        s_sellEntry.PrimaryColor = RGB(255, 0, 0);

        sc.AllowMultipleEntriesInSameDirection = false;
		sc.MaximumPositionAllowed = 3;
		sc.SupportReversals = true;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = true;
		sc.SupportAttachedOrdersForTrading = true;

		sc.CancelAllOrdersOnEntriesAndReversals = true;
		sc.AllowEntryWithWorkingOrders = true;
		sc.CancelAllWorkingOrdersOnExit = true;
		sc.AllowOnlyOneTradePerBar = true;

		sc.MaintainTradeStatisticsAndTradesData = true;
        // Add a button input

    }

    static int Bar1 = 3;
    static int Bar2 = 3;

    static int prevIndex = -1;
    int currentIndex = sc.Index;

    static int buycount = 0;
    static int sellcount = 0;

    static int intrade = 0;


    if (currentIndex > prevIndex){



        if (sc.Open[sc.Index - 2] > sc.Close[sc.Index - 2]){
            Bar1 = 0;
            msg.Format("previous 2 bars negative");
            sc.AddMessageToLog(msg, 1);
        } 

        else if (sc.Open[sc.Index - 2] < sc.Close[sc.Index - 2]){
            Bar1 = 1;
            msg.Format("previous 2 bars positive");
            sc.AddMessageToLog(msg, 1);

        }

        if (sc.Open[sc.Index - 1] > sc.Close[sc.Index - 1]){
            Bar2 = 0;
            msg.Format("previous bar negative");
            sc.AddMessageToLog(msg, 1);

        }

        else if (sc.Open[sc.Index - 1] < sc.Close[sc.Index - 1]){
            Bar2 = 1;
            msg.Format("previous bar positive");
            sc.AddMessageToLog(msg, 1);

        }

        s_SCPositionData PositionData;
        sc.GetTradePosition(PositionData);

        int qty = PositionData.PositionQuantity;


            if (Bar1 == 1 && Bar2 == 1 && qty <= 0){

                s_buyEntry[sc.Index] = sc.Low[sc.Index];
                
                sc.FlattenAndCancelAllOrders();

                //intrade = 1;
                
                s_SCNewOrder NewOrder;
                NewOrder.OrderQuantity = 1;
                NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
                NewOrder.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;

                NewOrder.Target1Offset = 5;
                NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;
                NewOrder.Stop1Offset = 2;
                NewOrder.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP;

                sc.BuyEntry(NewOrder);  


/*
                s_SCNewOrder Stop;
                Stop.OrderQuantity = 1;
                Stop.OrderType = SCT_ORDERTYPE_STOP;
                Stop.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
                Stop.Price1 = sc.Low[sc.Index] - 1;
                sc.BuyExit(Stop);
                
                s_SCNewOrder profit;
                profit.OrderQuantity = 1;
                profit.OrderType = SCT_ORDERTYPE_STOP;
                profit.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
                profit.Price1 = sc.High[sc.Index] + 1;
                sc.BuyExit(profit);
*/
                
                
                msg.Format("previous 2 bars positive, buy entry");
                sc.AddMessageToLog(msg, 1);

                buycount++;
            
            }
            

            else if (Bar1 == 0 && Bar2 == 0 && (qty >= 0)){
                
                s_sellEntry[sc.Index] = sc.Low[sc.Index];

                sc.FlattenAndCancelAllOrders;

                //intrade = -1;
            
                s_SCNewOrder NewOrder;
                NewOrder.OrderQuantity = 1;
                NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
                NewOrder.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;


                NewOrder.Target1Offset = 5;
                NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

                NewOrder.Stop1Offset = 2;
                NewOrder.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP;

                sc.SellEntry(NewOrder);

/*
                s_SCNewOrder Stop;
                Stop.OrderQuantity = 1;
                Stop.OrderType = SCT_ORDERTYPE_STOP;
                Stop.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
                Stop.Price1 = sc.High[sc.Index - 1];
                sc.SellExit(Stop); 

                s_SCNewOrder profit;
                profit.OrderQuantity = 1;
                profit.OrderType = SCT_ORDERTYPE_LIMIT;
                profit.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
                profit.Price1 = sc.Low[sc.Index] + 1;
                sc.SellExit(profit); 
*/
                msg.Format("previous 2 bars negative, sell entry");
                sc.AddMessageToLog(msg, 1);

                sellcount++;

            }
                
            msg.Format("total buy: %d total sell: %d", buycount, sellcount);
            sc.AddMessageToLog(msg, 1);

            prevIndex = currentIndex;

    }


}


