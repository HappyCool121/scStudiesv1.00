#include "sierrachart.h"
#include <cmath>
#include <iostream>
#include <math.h>
SCDLLName("volume test 0.1")


void print (SCStudyInterfaceRef sc, SCString text){
    sc.AddMessageToLog(text, 1);
}

//elements needed for getvolumeby price function
//start bar index
//end bar index
//value area percentage (float)
//volume by price array
//ask volume by price array 
//bid volume by price array 
//delta volume by price array 
//price array 
//output array (double)
//output array (int)

void getVolumeAndVAbyPrice (SCStudyInterfaceRef sc, int startBar, int endbar, double VApercent, int volumeArray0[], int askVolumeArray[], int bidVolumeArray[], int deltaArray[], double priceArray0[], double outputArray0[], int outputArray1[]){


//purpose of this function is to get volume at price for a specific range of bars
//total volume used to calculate vpoc, and determine the value area
//ask volume and bid volume to see where aggressive sellers and buyers are active
//delta is to identify absorbtion/exhaustion or simply delta divergence to identidy potential reversal

//note that start bar would most likely be obtained from the bar index array from getdata function/linear regression function itself


    int lowInTicks = sc.PriceValueToTicks(sc.Close[endbar]);
    int highInTicks = sc.PriceValueToTicks(sc.Close[endbar]);

    for (int i = startBar; i < endbar + 1; i++){

        if (lowInTicks > sc.PriceValueToTicks(sc.Low[i])){
            lowInTicks = sc.PriceValueToTicks(sc.Low[i]);
        }
        if (highInTicks < sc.PriceValueToTicks(sc.High[i])){
            highInTicks = sc.PriceValueToTicks(sc.High[i]);
        }

    }

    SCString output = "";
    output.Format("low: %d, high: %d, implied size: %d, index: %d, %d", lowInTicks/4, highInTicks/4, highInTicks - lowInTicks, startBar, endbar);
    print(sc, output);

//price must be in ticks
//volume array is int, price array is double, both have a 0 at the end
//start and end of price and bar index are INCLUSIVE
//outputArray0[0] = vpocPrice0;
//outputArray0[1] = VAH;
//outputArray0[2] = VAL;
//outputArray1[0] = vpoc0;
//outputArray1[1] = range;
    
    int targetIndex = 0;
    int vpoc0 = 0;
    double vpocPrice0 = 0;
    int range = highInTicks - lowInTicks;
    int totalvolume = 0;

    for (int price = lowInTicks, i = 0;  price < highInTicks + 1; i++, price++){
        
        int totalVolumeAtPrice = 0;
        int totalBidVolumeAtPrice = 0;
        int totalAskVolumeAtPrice = 0;
        int deltaAtPrice = 0;
        
        s_VolumeAtPriceV2 * p_VolumeAtPrice = nullptr;

        unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

        for (int BarIndex = startBar; BarIndex < endbar + 1; BarIndex++){
            
            if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                            
                SCString output = "";
                output.Format("volume found: %d, %d, %d, %d", p_VolumeAtPrice->Volume,  p_VolumeAtPrice->BidVolume, p_VolumeAtPrice->AskVolume, (p_VolumeAtPrice->AskVolume - p_VolumeAtPrice->BidVolume));
                //print(sc, output);

                totalVolumeAtPrice += p_VolumeAtPrice->Volume;
                totalBidVolumeAtPrice += p_VolumeAtPrice->BidVolume;
                totalAskVolumeAtPrice += p_VolumeAtPrice->AskVolume;
                deltaAtPrice += p_VolumeAtPrice->AskVolume - p_VolumeAtPrice->BidVolume;

            }
        
        }  

        volumeArray0[i] = totalVolumeAtPrice;
        bidVolumeArray[i] = totalBidVolumeAtPrice;
        askVolumeArray[i] = totalAskVolumeAtPrice;
        deltaArray[i] = deltaAtPrice;
        priceArray0[i] = sc.TicksToPriceValue(price);
        totalvolume += totalVolumeAtPrice;

        if (volumeArray0[i] > vpoc0){

            vpoc0 = volumeArray0[i];
            vpocPrice0 = sc.TicksToPriceValue(price);
            targetIndex = i;

        }

    }

    int VAvol = static_cast<int> (std::round(VApercent * totalvolume));
    int volumeCount = 0;

    double VAH = 0;
    double VAL = 0;

    int countUp = targetIndex;
    int countDown = targetIndex;

    while (volumeCount < VAvol && countUp < range && countDown > 0){

        if (countUp < range){

            volumeCount += volumeArray0[countUp];
            countUp++;

        }
        if (countDown > 0){

            volumeCount += volumeArray0[countDown];
            countDown--;

        }
    }

    //possible to add algorithm to find absorbtion here?
    //highest and lowest delta for the price levels
    //or general areas of high/low delta and high levels of ask/bid volumes 
    //most likely will implement a sort algorithm to find highest/lowest three
    //possible to determine relative size of delta compared to the total price? so that the absolute values dont really determine the highest/lowest

    VAH = priceArray0[countUp];
    VAL = priceArray0[countDown];

    outputArray0[0] = vpocPrice0;
    outputArray0[1] = VAH;
    outputArray0[2] = VAL;
    outputArray0[3] = sc.TicksToPriceValue(highInTicks);
    outputArray0[4] = sc.TicksToPriceValue(lowInTicks);

    //highest delta/ask/bid prices

    outputArray1[0] = vpoc0;
    outputArray1[1] = range;
    //highest bid/ask/delta

    output.Format("high: %.02f, low: %.02f, vpoc: %.02f, vah: %.02f, val; %.02f", outputArray0[3], outputArray0[4], outputArray0[0], VAH, VAL);
    print(sc, output);

    for (int i = lowInTicks, arraycount = 0; i < highInTicks; i++, arraycount++){

        output.Format("volume: %d, ask: %d, bid: %d, delta: %d, count: %d = %d?, price: %.02f", volumeArray0[arraycount], askVolumeArray[arraycount], bidVolumeArray[arraycount], deltaArray[arraycount], arraycount, range, priceArray0[arraycount]);
        //print(sc, output);

    }

}


void drawProfiles (SCStudyInterfaceRef sc, int size, int numberOfBars, int randomSeedNumber, double priceArray[], int volumeArray[], int bidVolumeArray[], int askVolumeArray[], int deltaArray[]){

    //normalize the total volume at price

    sc.AddMessageToLog("excute draw function", 1);

    SCString output = "";
    output.Format("size: %d", size);
    print(sc, output);
    
    int volumeSize[size] = {};
    int volumeColor[size] {};
    int highestVolume = volumeArray[0];
    int lowestVolume = volumeArray[0];

    for (int i = 0; i < size; i++){

        if (volumeArray[i] > highestVolume){
            highestVolume = volumeArray[i];
        }

        if (volumeArray[i] < lowestVolume){
            lowestVolume = volumeArray[i];
        }

    }

    output.Format("maxvol: %d, minvol: %d", highestVolume, lowestVolume);
    print(sc, output);

    float volumeRange = highestVolume - lowestVolume;
    if (volumeRange == 0) {
        volumeRange = 1; // Avoid division by zero by setting range to 1
        sc.AddMessageToLog("Highest and lowest volume are equal; normalization may not work as intended.", 1);
    }

    for (int i = 0; i < size; i++) {

        volumeSize[i] = static_cast<int>(1 + ((volumeArray[i] - lowestVolume) * (numberOfBars)) / volumeRange);

        volumeColor[i] = static_cast<int>(1 + ((volumeArray[i] - lowestVolume) * (255 - 1)) / volumeRange);

    }

    for (int i = 0; i < size; i++){

        output.Format("price: %.02f, %d, %d, %d, length: %d", priceArray[i], volumeSize[i], volumeArray[i], i, numberOfBars);
        //print(sc, output);

        s_UseTool Tool100;
        Tool100.Clear();
        Tool100.ChartNumber = sc.ChartNumber;
        Tool100.DrawingType = DRAWING_LINE;
        Tool100.LineNumber = randomSeedNumber + i;
        //Tool100.Text = bottom2;
        //Tool100.TextAlignment = DT_BOTTOM | DT_LEFT;
        Tool100.Region = 0;
        Tool100.Color = RGB(50 + i , 100 + i, 255);
        Tool100.LineWidth = 1;
        Tool100.AddMethod = UTAM_ADD_OR_ADJUST;
        Tool100.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize];
        Tool100.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize - volumeSize[i]]; 
        Tool100.BeginValue = priceArray[i];
        Tool100.EndValue = priceArray[i];  
        Tool100.DrawUnderneathMainGraph = 1;
        sc.UseTool(Tool100);

    }


}


//elements needed for volume per bar:
//start index
//end index
//volume per bar array 
//ask volume per bar array 
//bid volume per bar array 
//delta per bar array 
//cum delta array 

void getVolumePerBar(SCStudyInterfaceRef sc, int startBarIndex, int endBarIndex, int volumePerBarArray[], int askVolumePerBarArray[], int bidVolumePerBarArray[], int deltaPerBarArray[], int cumDeltaPerBar[]){

    for (int i = startBarIndex, size = 0; i < endBarIndex; i++, size++){

        volumePerBarArray[size] = sc.Volume[i];
        askVolumePerBarArray[size] = sc.AskVolume[i];
        bidVolumePerBarArray[size] = sc.BidVolume[i];
        deltaPerBarArray[size] = sc.BidVolume[i] - sc.AskVolume[i];

    }

    cumDeltaPerBar[0] = deltaPerBarArray[0];

    for (int i = startBarIndex + 1, size = 1; i < endBarIndex; i++, size++){

        cumDeltaPerBar[size] = cumDeltaPerBar[0] + deltaPerBarArray[size];

        SCString output;
        output.Format("vol: %d, ask: %d, bid: %d, delta: %d cum delta: %d, count: %d", volumePerBarArray[size], askVolumePerBarArray[size], bidVolumePerBarArray[size], deltaPerBarArray[size], cumDeltaPerBar[size], size);
        //print(sc, output);

    }

}

void drawVolumePerBar (SCStudyInterfaceRef sc, int range, int startIndex, int endIndex, int volumePerBarArray[], int deltaPerBarArray[], int cumDeltaPerBarArray[]){


    int size = endIndex - startIndex; 


    SCString output;

    int volumeSize[size] = {};
    int deltaSize[size] = {}; //absolute size
    int deltaBool[size] = {}; //positive negative
    int cumdeltasize[size] = {}; //absolute size
    int cumdeltaBool[size] = {}; //positive/negative

    int minVol = volumePerBarArray[1];
    int maxVol = volumePerBarArray[1];

    int minDelta = 50;
    int maxDelta = 50;

    int minCumDelta = cumDeltaPerBarArray[0];
    int maxCumDelta = cumDeltaPerBarArray[0];


    //find max/min for all arrays and positive/negative for delta and cum delta

    for (int i = 0; i < size; i++){

        if (deltaPerBarArray[i] >= 0){
            deltaBool[i] = 1;
        }

        if (deltaPerBarArray[i] < 0){
            deltaBool[i] = 0;
            deltaPerBarArray[i] = -deltaPerBarArray[i];

            output.Format("delta bool: %d, changed val: %d", deltaBool[i], deltaPerBarArray[i]);
            //print(sc, output);

        }

        if (cumDeltaPerBarArray[i] >= 0){
            cumdeltaBool[i] = 1;
        }

        if (cumDeltaPerBarArray[i] < 0){
            cumdeltaBool[i] = 0;
            cumDeltaPerBarArray[i] = -cumDeltaPerBarArray[i];

            output.Format("cum delta bool: %d, changed val: %d", deltaBool[i], cumDeltaPerBarArray[i]);
            //print(sc, output);
        }

        
        if (minVol > volumePerBarArray[i]){
            minVol = volumePerBarArray[i];
        }

        if (maxVol < volumePerBarArray[i]){
            maxVol = volumePerBarArray[i];
        }

        if (minDelta > deltaPerBarArray[i]){
            minDelta = deltaPerBarArray[i];
        }

        if (maxDelta < deltaPerBarArray[i]){
            maxDelta = deltaPerBarArray[i];
        }

        if (minCumDelta > cumDeltaPerBarArray[i]){
            minCumDelta = cumDeltaPerBarArray[i];
        }

        if (maxCumDelta < cumDeltaPerBarArray[i]){
            maxCumDelta = cumDeltaPerBarArray[i];
        }
        
        output.Format("delta bool: %d, cumdeltabool: %d", deltaBool[i], cumdeltaBool[i]);
        //print(sc, output);

    }

    output.Format("maxvol: %d, minvol: %d, maxdelta: %d, mindelta: %d, max cumdelta: %d, min cumdelta: %d", maxVol, minVol, maxDelta, minDelta, maxCumDelta, minCumDelta);
    print(sc, output);

    float volumeRange = maxVol - minVol;
    float deltaRange = maxDelta - minDelta;
    float cumdeltaRange = maxCumDelta - minCumDelta;

    for (int i = 0; i < size; i++){

        volumeSize[i] = static_cast<int>(1 + ((volumePerBarArray[i] - minVol) * (range/2 - 1)) / volumeRange);

        deltaSize[i] = static_cast<int>(1 + ((deltaPerBarArray[i] - minDelta) * (range - 1)) / deltaRange);
        
        cumdeltasize[i] = static_cast<int>(1 + ((cumDeltaPerBarArray[i] - minCumDelta) * (cumdeltaRange - 1)) / volumeRange);

        output.Format("volume: %d %d, delta: %d %d, cumdelta: %d %d", volumePerBarArray[i], volumeSize[i], deltaPerBarArray[i], deltaSize[i], cumDeltaPerBarArray[i], cumdeltasize[i]);
        //print(sc, output);

    }

    for (int i = 0, index = startIndex; i < size; i++, index++){
        
        s_UseTool Tool300;
        Tool300.Clear();
        Tool300.LineNumber = 300 + i;
        Tool300.ChartNumber = sc.ChartNumber;
        Tool300.DrawingType = DRAWING_LINE;
        //Tool300.Text = bottom2;
        //Tool300.TextAlignment = DT_BOTTOM | DT_LEFT;
        Tool300.Region = 0;
        Tool300.Color = RGB(255 , 255, 255);
        Tool300.LineWidth = 2;
        Tool300.AddMethod = UTAM_ADD_OR_ADJUST;
        Tool300.BeginIndex = index;
        Tool300.EndIndex = index;
        Tool300.UseRelativeVerticalValues = 1;
        Tool300.BeginValue = 1;
        Tool300.EndValue =  1 + volumeSize[i];  
        Tool300.DrawUnderneathMainGraph = 1;
        //sc.UseTool(Tool300);


        if (deltaBool[i] == 0){

            s_UseTool Tool400;
            Tool400.Clear();
            Tool400.LineNumber = 600 + i;
            Tool400.ChartNumber = sc.ChartNumber;
            Tool400.DrawingType = DRAWING_LINE;
            //Tool400.Text = bottom2;
            //Tool400.TextAlignment = DT_BOTTOM | DT_LEFT;
            Tool400.Region = 0;
            Tool400.Color = RGB(51,101,255);
            Tool400.LineWidth = 2;
            Tool400.AddMethod = UTAM_ADD_OR_ADJUST;
            Tool400.BeginIndex = index;
            Tool400.EndIndex = index;
            Tool400.UseRelativeVerticalValues = 1;
            Tool400.BeginValue =  1;
            Tool400.EndValue =   1+ deltaSize[i]; 
            Tool400.DrawUnderneathMainGraph = 0;
            sc.UseTool(Tool400);

        }
        else if (deltaBool[i] == 1){

             s_UseTool Tool400;
            Tool400.Clear();
            Tool400.LineNumber = 600 + i;
            Tool400.ChartNumber = sc.ChartNumber;
            Tool400.DrawingType = DRAWING_LINE;
            //Tool400.Text = bottom2;
            //Tool400.TextAlignment = DT_BOTTOM | DT_LEFT;
            Tool400.Region = 0;
            Tool400.Color = RGB(255,167,0);
            Tool400.LineWidth = 2;
            Tool400.AddMethod = UTAM_ADD_OR_ADJUST;
            Tool400.BeginIndex = index;
            Tool400.EndIndex = index;
            Tool400.UseRelativeVerticalValues = 1;
            Tool400.BeginValue = 100  - 1;
            Tool400.EndValue =  (100 - 1) - deltaSize[i]; 
            Tool400.DrawUnderneathMainGraph = 0;
            sc.UseTool(Tool400);

        }

    }

}




SCSFExport scsf_volumetest01(SCStudyInterfaceRef sc)
{
    SCString msg;

    sc.MaintainVolumeAtPriceData = 1;

    sc.AutoLoop = 0;

    if(sc.SetDefaults)
    {
        sc.GraphName = "volume test 0.1";
        sc.GraphRegion = 0;

        return;

    }

    static int currentIndex = 0;

        if (currentIndex < sc.Index){


            sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TOOL_DELETE_ALL);  
            sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_ALL, 0);



        //elements needed for getvolumeby price function
        //start bar index
        //end bar index
        //value area percentage (float)
        //volume by price array
        //ask volume by price array 
        //bid volume by price array 
        //delta volume by price array 
        //price array 
        //output array (double)
        //output array (int)

        
        int volumeByPriceArray[500] = {};
        int askVolumeByPriceArray[500] = {};
        int bidVolumeByPriceArray[500] = {};
        int deltaByPriceArray[500] = {};
        double priceArrayForVBP[500] = {};
        double outputArrayVBPdouble[10] = {};
        int outputArrayVBPint[10] = {};

        //elements needed for volume per bar:
        //start index
        //end index
        //volume per bar array 
        //ask volume per bar array 
        //bid volume per bar array 
        //delta per bar array 
        //cum delta array 

        int volumeperbararray[500] = {};
        int askvolumeperbararray[500] = {};
        int bidvolumeperbararray[500] = {};
        int deltaperbararray[500] = {};
        int cumdeltaperbararray[500] = {};


        getVolumeAndVAbyPrice(sc, sc.ArraySize - 40, sc.ArraySize - 1, 0.7, volumeByPriceArray, askVolumeByPriceArray, bidVolumeByPriceArray, deltaByPriceArray, priceArrayForVBP, outputArrayVBPdouble, outputArrayVBPint);

        drawProfiles(sc, outputArrayVBPint[1], 39, 0, priceArrayForVBP, volumeByPriceArray, bidVolumeByPriceArray, askVolumeByPriceArray, deltaByPriceArray);

        getVolumePerBar(sc, sc.ArraySize - 40, sc.ArraySize - 1, volumeperbararray, askvolumeperbararray, bidvolumeperbararray, deltaperbararray, cumdeltaperbararray);

        drawVolumePerBar(sc, 20, sc.ArraySize - 40, sc.ArraySize - 1, volumeperbararray, deltaperbararray, cumdeltaperbararray);

        currentIndex = sc.Index;

    }

}

