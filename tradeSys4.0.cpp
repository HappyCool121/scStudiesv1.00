#include "sierrachart.h"
#include <cmath>
#include <iostream>
#include <math.h>
SCDLLName("tradesystem4.0")

void print (SCStudyInterfaceRef sc, SCString text){
    sc.AddMessageToLog(text, 1);
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//Getting data for linear regression-----------------------------------------------------------------------------------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void getData (SCStudyInterfaceRef sc, int startIndex, int endIndex, int type, int BarPeriod, double priceArray[], int timeArray[], double weightArray[], double rangeArray[], int weightSetting){

    if (type == 0){
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            priceArray[arrayCounter] = sc.Open[i];
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;
            weightArray[arrayCounter] = 1;
            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

            SCString output = "";
            output.Format("%.02f", priceArray[arrayCounter]);
            //print(sc, output);

        }
    }

    else if (type == 1){
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            priceArray[arrayCounter] = sc.Close[i];
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;
            weightArray[arrayCounter] = 1;
            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

            SCString output = "";
            output.Format("%.02f", priceArray[arrayCounter]);
            //print(sc, output);

        }
    }

    else if (type == 2){
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            priceArray[arrayCounter] = sc.High[i];
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;
            weightArray[arrayCounter] = 1;
            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

            SCString output = "";
            output.Format("%.02f", priceArray[arrayCounter]);
            //print(sc, output);

        }
    }
    else if (type == 3){
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            priceArray[arrayCounter] = sc.Low[i];
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;
            weightArray[arrayCounter] = 1;
            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

            SCString output = "";
            output.Format("%.02f", priceArray[arrayCounter]);
            //print(sc, output);

        }
    }
    else if (type == 4){
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            priceArray[arrayCounter] = (sc.High[i] + sc.Low[i])/2;
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;
            weightArray[arrayCounter] = 1;
            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

            SCString output = "";
            output.Format("%.02f", priceArray[arrayCounter]);
            //print(sc, output);

        }
    }
    else if (type == 5){

        int volumeArray[200] = {};

        for (int barIndex = startIndex, arrayCounter = 0; barIndex < endIndex; barIndex++, arrayCounter++){
            
            s_VolumeAtPriceV2 * p_VolumeAtPrice = nullptr;

            unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

            int vpoc = 0;
            float vpocPrice = 0;
                
            int volumeArrayForBar[50] = {};
            int priceArrayForBar[50] = {};

            for (int price = sc.PriceValueToTicks(sc.Low[barIndex]), arrayIndex = 0; price < sc.PriceValueToTicks(sc.High[barIndex]); price++, arrayIndex++){
                
                if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(barIndex, price, &p_VolumeAtPrice, InsertionIndex)) {

                    volumeArrayForBar[arrayIndex] = p_VolumeAtPrice->Volume;
                    priceArrayForBar[arrayIndex] = price;

                    //If found, log the volume at that price

                    if (volumeArrayForBar[arrayIndex] > vpoc){

                        vpoc  = volumeArrayForBar[arrayIndex];
                        vpocPrice = sc.TicksToPriceValue(price);
                    } 

                }
            
            }   

            volumeArray[arrayCounter] = vpoc;
            priceArray[arrayCounter] = vpocPrice;
            timeArray[arrayCounter] = (arrayCounter + 1) * BarPeriod;

            SCString output = "";
            output.Format("vpoc price %.02f, vpoc value %d, time %d", priceArray[arrayCounter], volumeArray[arrayCounter], timeArray[arrayCounter]);
            //print(sc, output);

        }

        int minvol = volumeArray[0];
        int maxvol = volumeArray[0];

        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){

            if (volumeArray[arrayCounter] > maxvol){
                maxvol = volumeArray[arrayCounter];
            }

            if (volumeArray[arrayCounter] < minvol){
                minvol = volumeArray[arrayCounter];
            }

            rangeArray[arrayCounter] = sc.High[i] - sc.Low[i];

        }
        
        for (int i = startIndex, arrayCounter = 0; i < endIndex; i++, arrayCounter++){
            
            
            if (weightSetting == 0){

                weightArray[arrayCounter] = 0.999 + pow(static_cast<double>(volumeArray[i] - minvol)/(maxvol - minvol), 1) * 0.001;
                SCString output = "";
                output.Format("%.02f", priceArray[arrayCounter]);
                //print(sc, output);

            }
            else if (weightSetting == 1){

                weightArray[arrayCounter] = 0.9999 + pow(static_cast<double>(volumeArray[i] - minvol)/(maxvol - minvol), 1) * 0.0001;
                SCString output = "";
                output.Format("%.02f", priceArray[arrayCounter]);
                //print(sc, output);

            }
            else if (weightSetting == 2){

                weightArray[arrayCounter] = 0.99999 + pow(static_cast<double>(volumeArray[i] - minvol)/(maxvol - minvol), 1) * 0.00001;
                SCString output = "";
                output.Format("%.02f", priceArray[arrayCounter]);
                //print(sc, output);

            }
            else if (weightSetting == 3){

                weightArray[arrayCounter] = 0.999999 + pow(static_cast<double>(volumeArray[i] - minvol)/(maxvol - minvol), 1) * 0.000001;
                SCString output = "";
                output.Format("%.02f", priceArray[arrayCounter]);
                //print(sc, output);

            }
            else if (weightSetting == 4){

                weightArray[arrayCounter] = 0.9999999 + pow(static_cast<double>(volumeArray[i] - minvol)/(maxvol - minvol), 1) * 0.0000001;
                SCString output = "";
                output.Format("%.02f", priceArray[arrayCounter]);
                //print(sc, output);

            }


        }

    }

}


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//function to compute linear regression------------------------------------------------------------------------------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void linear_regression (SCStudyInterfaceRef sc, int arraySize, int offsetIndex, double priceArray[], int timeArray[], double weightArray[], double rangeArray[], double outputArray[]){

    double WSX = 0;
    double WSY = 0;
    double WSSX = 0;
    double WSXY = 0;
    double WSSY = 0;
    double SSresidual = 0;
    double SStotal = 0;
    int n = arraySize;
    double maxRangeForVol = 0;
    double totalRangeForVol = 0;
    double averageVol = 0;

    for (int i = offsetIndex, j = 0; j < n; i++, j++){

        WSX += weightArray[i] * (timeArray[i]);
        WSY += weightArray[i] * priceArray[i];
        WSSX += weightArray[i] * timeArray[i] * timeArray[i];
        WSXY += weightArray[i] * timeArray[i] * priceArray[i];
        WSSY += weightArray[i] * priceArray[i] * priceArray[i];

        totalRangeForVol += rangeArray[i];

        if (maxRangeForVol > rangeArray[i]){

            maxRangeForVol = rangeArray[i];

        }
        

    }

    averageVol = totalRangeForVol / n;

    double slope = (n * WSXY - WSX * WSY) / (n * WSSX - WSX * WSX);
    double intercept = (WSY - slope * WSX) / n;

    double rValue = (n * WSXY - WSX * WSY) / (sqrt((n * WSSX - WSX * WSX) * (n * WSSY - WSY * WSY)));

    double meanY = WSY/n;

    for (int i = offsetIndex, j = 0; j < n; i++, j++){

        SSresidual += pow((priceArray[i] - (slope * timeArray[i] + intercept)), 2);
        SStotal += pow((priceArray[i] - meanY), 2);
        
    }

    double R2value = 1.0 - (SSresidual/SStotal);

    outputArray[0] = slope;
    outputArray[1] = intercept;
    outputArray[2] = rValue;
    outputArray[3] = R2value;
    outputArray[4] = maxRangeForVol;
    outputArray[5] = averageVol;
    
    SCString output = "";
    output.Format("%.05f, %.05f, %.05f, %.05f", outputArray[0], outputArray[1], outputArray[2], outputArray[3]);
    //print(sc, output);

}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//function to calculate VA and VPOC>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void getVolumeAndVA (SCStudyInterfaceRef sc, int startBar, int endbar, double VApercent, int volumeArray0[], double priceArray0[], double outputArray0[], int outputArray1[]){

    int lowInTicks = sc.PriceValueToTicks(sc.Close[sc.ArraySize - 1]);
    int highInTicks = sc.PriceValueToTicks(sc.Close[sc.ArraySize - 1]);

    for (int i = startBar; i < endbar + 1; i++){
        if (lowInTicks > sc.PriceValueToTicks(sc.Low[i])){
            lowInTicks = sc.PriceValueToTicks(sc.Low[i]);
        }
        if (highInTicks < sc.PriceValueToTicks(sc.High[i])){
            highInTicks = sc.PriceValueToTicks(sc.High[i]);
    }
    
    SCString output = "";
    output.Format("low: %d, high: %d", lowInTicks, highInTicks);
    //print(sc, output);

    }

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
        
        s_VolumeAtPriceV2 * p_VolumeAtPrice = nullptr;

        unsigned int InsertionIndex = -1;  // Used internally by Sierra Chart

        for (int BarIndex = startBar; BarIndex < endbar + 1; BarIndex++){
            
            if (sc.VolumeAtPriceForBars->GetVAPElementForPriceIfExists(BarIndex, price, &p_VolumeAtPrice, InsertionIndex)) {
                            
                SCString output = "";
                output.Format("volume found: %d", p_VolumeAtPrice->Volume);
                //print(sc, output);

                totalVolumeAtPrice += p_VolumeAtPrice->Volume;

            }
        
        }  

        volumeArray0[i] = totalVolumeAtPrice;
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

    VAH = priceArray0[countUp];
    VAL = priceArray0[countDown];

    outputArray0[0] = vpocPrice0;
    outputArray0[1] = VAH;
    outputArray0[2] = VAL;
    outputArray0[3] = sc.TicksToPriceValue(highInTicks);
    outputArray0[4] = sc.TicksToPriceValue(lowInTicks);

    outputArray1[0] = vpoc0;
    outputArray1[1] = range;

    SCString output = "";
    output.Format("%.02f, %d, %.02f, %.02f", outputArray0[0], outputArray1[0], outputArray0[1], outputArray0[2]);
    //print(sc, output);

}



SCSFExport scsf_tradesys40(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_nbars = sc.Input[0]; //for VPOC 
    SCInputRef i_barsback = sc.Input[1]; //for VPOC
    SCInputRef i_currentTrend = sc.Input[2]; //output from linear regression
    SCInputRef i_barsbackLinReg = sc.Input[3];
    SCInputRef i_lookback = sc.Input[4];
    SCInputRef i_type = sc.Input[5];
    SCInputRef i_barperiod = sc.Input[6];
    SCInputRef i_weightSetting = sc.Input[7];
	SCInputRef Input_HorizontalPosition = sc.Input[8];
	SCInputRef Input_VerticalPosition = sc.Input[9];
    SCInputRef i_minimumR = sc.Input[10];
    SCInputRef i_minimumR2 = sc.Input[11];
    SCInputRef i_compareType = sc.Input[12];
    SCInputRef i_VAvolume = sc.Input[13];
    SCInputRef i_calculateProfiles = sc.Input[14];
    SCInputRef i_drawProfiles = sc.Input[15];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

    SCSubgraphRef s_VAH = sc.Subgraph[1];
    SCSubgraphRef s_VAL = sc.Subgraph[2];
    SCSubgraphRef s_vPOC = sc.Subgraph[3];

    SCSubgraphRef s_linReg = sc.Subgraph[4];
    SCSubgraphRef s_linRegUpper = sc.Subgraph[5];
    SCSubgraphRef s_linRegLower = sc.Subgraph[6];

    sc.MaintainVolumeAtPriceData = 1;

    //sc.UpdateAlways = 1;

    sc.AutoLoop = 1;

    if(sc.SetDefaults)
    {
        sc.GraphName = "tradesys4.0";
        sc.GraphRegion = 0;

        i_nbars.Name = "Number of bars to calculate for VPOC?";
        i_nbars.SetInt(16);

        i_barsback.Name = "How many bars back to end calculation for VPOC?";
        i_barsback.SetInt(1);

        i_currentTrend.Name = "select 1 to use technical indicators to assess trend";
        i_currentTrend.SetInt(0);

        i_barsbackLinReg.Name = "How many bars back to stop calculating regression equation?";
        i_barsbackLinReg.SetInt(1);

        i_lookback.Name = "how many bars back to start linear regression calculation?";
        i_lookback.SetInt(150);

        i_type.Name = "select type of price points for linear regression";
        i_type.SetCustomInputStrings("Open; Close; High; Low; Midpoint; weighted VPOC");
        i_type.SetCustomInputIndex(0);

        i_barperiod.Name = "Enter chart bar period";
        i_barperiod.SetInt(5);

        i_weightSetting.Name = "Select the scale of weights";
        i_weightSetting.SetCustomInputStrings("Very High; High; Default; Low; Very Low;");
        i_weightSetting.SetCustomInputIndex(1);

        i_minimumR.Name = "Minimum r value";
        i_minimumR.SetFloat(0.7);

        i_minimumR2.Name = "minimum r2 value";
        i_minimumR2.SetFloat(0.7);

        i_compareType.Name = "Select regression equation select type";
        i_compareType.SetCustomInputStrings("Highest both r and R2 value;Highest r value;Highest R2 value;Specific r and R2 value");
        i_compareType.SetCustomInputIndex(3);

        i_VAvolume.Name = "Insert value area volume percentage";
        i_VAvolume.SetFloat(0.7);

        i_calculateProfiles.Name = "Calculate Profile?";
        i_calculateProfiles.SetCustomInputStrings("No;Yes");
        i_calculateProfiles.SetCustomInputIndex(0);

        i_drawProfiles.Name = "Draw Profile lines??";
        i_drawProfiles.SetCustomInputStrings("No;Yes");
        i_drawProfiles.SetCustomInputIndex(0);

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

    static int counter = 0;

    static int currentIndex = 0;

    msg.Format("%d, current index: %d, sc index: %d", counter, currentIndex, sc.Index);
    sc.AddMessageToLog(msg, 1);

    int volumeArray1[100] = {};
    double priceArray1[100] = {};
    double outputArray0[4] = {};
    int outputArray1[4] = {};

    if (i_calculateProfiles.GetIndex() == 1){

        getVolumeAndVA(sc, sc.ArraySize - i_nbars.GetInt(), sc.ArraySize - i_barsback.GetInt(), i_VAvolume.GetFloat(), volumeArray1, priceArray1, outputArray0, outputArray1);

    }

    if (i_drawProfiles.GetIndex() == 1){

        s_UseTool Tool3;
        Tool3.Clear();
        Tool3.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool3.DrawingType = DRAWING_LINE;
        Tool3.LineNumber =  30;
        Tool3.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - i_nbars.GetInt()];
        Tool3.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize];
        //Tool3.Text = "high";
        //Tool3.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool3.Region = 0;
        Tool3.Color = RGB(128, 128, 128);
        Tool3.LineWidth = 2;
        Tool3.AddMethod = UTAM_ADD_OR_ADJUST;

        s_UseTool Tool6;
        Tool6.Clear();
        Tool6.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool6.DrawingType = DRAWING_LINE;
        Tool6.LineNumber =  60;
        Tool6.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - i_nbars.GetInt()];
        Tool6.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize];
        //Tool6.Text = "low";
        //Tool6.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool6.Region = 0;
        Tool6.Color = RGB(128, 128, 128);
        Tool6.LineWidth = 2;
        Tool6.AddMethod = UTAM_ADD_OR_ADJUST;

        s_UseTool Tool2;
        Tool2.Clear();
        Tool2.ChartNumber = sc.ChartNumber;
        //Tool.LineNumber will be automatically set. No need to set this.
        Tool2.DrawingType = DRAWING_LINE;
        Tool2.LineNumber = 20;
        Tool2.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - i_nbars.GetInt()];
        Tool2.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9];
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
        Tool4.LineNumber =  40; 
        Tool4.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - i_nbars.GetInt()];
        Tool4.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9];
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
        Tool5.LineNumber =  50;
        Tool5.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - i_nbars.GetInt()];
        Tool5.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + 9];
        //Tool5.Text = "POC";
        //Tool5.TextAlignment = DT_BOTTOM | DT_RIGHT;
        Tool5.Region = 0;
        Tool5.Color = RGB(255, 255, 255);
        Tool5.LineWidth = 2;
        Tool5.AddMethod = UTAM_ADD_OR_ADJUST;

        Tool3.BeginValue = (outputArray0[3]);
        Tool3.EndValue = (outputArray0[3]);
        sc.UseTool(Tool3);         

        Tool6.BeginValue = (outputArray0[4]);
        Tool6.EndValue = (outputArray0[4]);
        sc.UseTool(Tool6); 

        Tool2.BeginValue = outputArray0[1];
        Tool2.EndValue = outputArray0[1];  
        sc.UseTool(Tool2); 

        Tool4.BeginValue = outputArray0[2];
        Tool4.EndValue = outputArray0[2];
        sc.UseTool(Tool4);
        
        Tool5.BeginValue = outputArray0[0];
        Tool5.EndValue = outputArray0[0];
        sc.UseTool(Tool5);
    
    }

    double priceArray[i_lookback.GetInt()] = {};
    int timeArray[i_lookback.GetInt()] = {};
    double weightArray[i_lookback.GetInt()] = {};
    double rangeArray[i_lookback.GetInt()] = {};
    double outputArray[6] = {};
    bool conditionHit = false;
    double highestR = 0;
    double highestR2 = 0;
    int index = 0;
    double slope = 0;
    double intercept = 0;


    if (currentIndex < sc.Index){
  
        SCString output = "";
        output.Format("%d, %d", currentIndex, sc.Index);
        print(sc, output);

    getData(sc, sc.ArraySize - i_lookback.GetInt(), sc.ArraySize - i_barsbackLinReg.GetInt(), i_type.GetIndex(), i_barperiod.GetInt(), priceArray, timeArray, weightArray, rangeArray, i_weightSetting.GetIndex());

        
        if (i_compareType.GetIndex() == 0){

            //highest r and R2 value

                counter++;
                currentIndex = sc.Index;

            for (int k = 0, countdown = i_lookback.GetInt() - i_barsbackLinReg.GetInt(); countdown > i_barsbackLinReg.GetInt() + 10; k++, countdown--){

                linear_regression(sc, countdown, k, priceArray, timeArray, weightArray, rangeArray, outputArray);

                if (abs(outputArray[2]) > highestR && abs(outputArray[3] > highestR2)){

                    highestR = abs(outputArray[2]);
                    highestR2 = abs(outputArray[3]);
                    index = k;
                    slope = outputArray[0];
                    intercept = outputArray[1];

                }

            }

            s_UseTool Tool1;
            Tool1.Clear();
            Tool1.ChartNumber = sc.ChartNumber;
            Tool1.DrawingType = DRAWING_LINE;
            Tool1.LineNumber = 100;
            Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - (i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index)];
            Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize]; 
            //Tool1.Text = linetext;
            //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool1.Region = 0;
            Tool1.Color = RGB(255, 255, 255);
            Tool1.LineWidth = 1;
            Tool1.AddMethod = UTAM_ADD_OR_ADJUST;

            Tool1.BeginValue = slope * (timeArray[index]) + intercept;
            Tool1.EndValue = slope * timeArray[i_lookback.GetInt() - i_barsbackLinReg.GetInt() - 1] + intercept;  
            sc.UseTool(Tool1);



            SCString output = "";
            output.Format("best value for highest r and R2: %d, r: %.05f, r2: %.05f", index, highestR, highestR2);
            print(sc, output);

            SCString ValueText;
            ValueText.Format("no. of bars: %d \nr value: %.05f \nr2 value: %.05f", i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index, highestR, highestR2);


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


            return; 

        }

        else if (i_compareType.GetIndex() == 1){

            //highest r and R2 value

                counter++;
                currentIndex = sc.Index;
            
            for (int k = 0, countdown = i_lookback.GetInt() - i_barsbackLinReg.GetInt(); countdown > i_barsbackLinReg.GetInt() + 10; k++, countdown--){

                linear_regression(sc, countdown, k, priceArray, timeArray, weightArray, rangeArray, outputArray);

                if (abs(outputArray[2]) > highestR){

                    highestR = abs(outputArray[2]);
                    highestR2 = abs(outputArray[3]);
                    index = k;
                    slope = outputArray[0];
                    intercept = outputArray[1];

                }

            }

            s_UseTool Tool1;
            Tool1.Clear();
            Tool1.ChartNumber = sc.ChartNumber;
            Tool1.DrawingType = DRAWING_LINE;
            Tool1.LineNumber =  100;
            Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - (i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index)];
            Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize]; 
            //Tool1.Text = linetext;
            //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool1.Region = 0;
            Tool1.Color = RGB(255, 255, 255);
            Tool1.LineWidth = 2;
            Tool1.AddMethod = UTAM_ADD_OR_ADJUST;

            Tool1.BeginValue = slope * (timeArray[index]) + intercept;
            Tool1.EndValue = slope * timeArray[i_lookback.GetInt() - i_barsbackLinReg.GetInt() - 1] + intercept;  
            sc.UseTool(Tool1);

            SCString output = "";
            output.Format("best value for highest R: %d, r: %.05f, r2: %.05f", index, highestR, highestR2);
            print(sc, output);

            SCString ValueText;
            ValueText.Format("no. of bars: %d \nr value: %.05f \nr2 value: %.05f", i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index, highestR, highestR2);


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


            return; 

        }

        else if (i_compareType.GetIndex() == 2){

            //highest r and R2 value

                counter++;
                currentIndex = sc.Index;
            
            for (int k = 0, countdown = i_lookback.GetInt() - i_barsbackLinReg.GetInt(); countdown > i_barsbackLinReg.GetInt() + 10; k++, countdown--){

                linear_regression(sc, countdown, k, priceArray, timeArray, weightArray, rangeArray, outputArray);

                if (abs(outputArray[3] > highestR2)){

                    highestR = abs(outputArray[2]);
                    highestR2 = abs(outputArray[3]);
                    index = k;
                    slope = outputArray[0];
                    intercept = outputArray[1];

                }

            }

            s_UseTool Tool1;
            Tool1.Clear();
            Tool1.ChartNumber = sc.ChartNumber;
            Tool1.DrawingType = DRAWING_LINE;
            Tool1.LineNumber =  100;
            Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - (i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index)];
            Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize]; 
            //Tool1.Text = linetext;
            //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool1.Region = 0;
            Tool1.Color = RGB(255, 255, 255);
            Tool1.LineWidth = 2;
            Tool1.AddMethod = UTAM_ADD_OR_ADJUST;

            Tool1.BeginValue = slope * (timeArray[index]) + intercept;
            Tool1.EndValue = slope * timeArray[i_lookback.GetInt() - i_barsbackLinReg.GetInt() - 1] + intercept;  
            sc.UseTool(Tool1);

            SCString output = "";
            output.Format("best value for highest R2 value: %d, r: %.05f, r2: %.05f", index, highestR, highestR2);
            print(sc, output);

            SCString ValueText;
            ValueText.Format("no. of bars: %d \nr value: %.05f \nr2 value: %.05f", i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index, highestR, highestR2);


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


            return; 

        }

        else if (i_compareType.GetIndex() == 3){

            //highest r and R2 value

                counter++;
                currentIndex = sc.Index;
            
            for (int k = 0, countdown = i_lookback.GetInt() - i_barsbackLinReg.GetInt(); countdown > i_barsbackLinReg.GetInt() + 10; k++, countdown--){

                linear_regression(sc, countdown, k, priceArray, timeArray, weightArray, rangeArray, outputArray);

                if (abs(outputArray[2]) > i_minimumR.GetFloat() && abs(outputArray[3]) > i_minimumR2.GetFloat()){

                    slope = outputArray[0];
                    intercept = outputArray[1];
                    highestR = outputArray[2];
                    highestR2 = outputArray[3];
                    index = k;
      
                    s_UseTool Tool1;
                    Tool1.Clear();
                    Tool1.ChartNumber = sc.ChartNumber;
                    Tool1.DrawingType = DRAWING_LINE;
                    Tool1.LineNumber = 100;
                    Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - (i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index)];
                    Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize]; 
                    //Tool1.Text = linetext;
                    //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
                    Tool1.Region = 0;
                    Tool1.Color = RGB(255, 255, 255);
                    Tool1.LineWidth = 2;
                    Tool1.AddMethod = UTAM_ADD_OR_ADJUST;

                    Tool1.BeginValue = slope * (timeArray[index]) + intercept;
                    Tool1.EndValue = slope * timeArray[i_lookback.GetInt() - i_barsbackLinReg.GetInt() - 1] + intercept;  
                    sc.UseTool(Tool1);

                    SCString output = "";
                    output.Format("best value for selected values: %d, r: %.05f, r2: %.05f", index, highestR, highestR2);
                    print(sc, output);

                    SCString ValueText;
                    ValueText.Format("no. of bars: %d \nr value: %.05f \nr2 value: %.05f", i_lookback.GetInt() - i_barsbackLinReg.GetInt() - index, highestR, highestR2);


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


                    return; 

                    break;

                }

            }

        }

    }

}