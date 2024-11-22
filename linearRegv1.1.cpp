#include "sierrachart.h"
#include <cmath>
#include <iostream>
#include <math.h>
SCDLLName("linearRegv1.1")


SCSFExport scsf_linearRegv1(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_lookBack = sc.Input[0];
    SCInputRef i_type = sc.Input[1];
    SCInputRef i_predict = sc.Input[2];
    SCInputRef i_rMin = sc.Input[3];
    SCInputRef i_r2Min = sc.Input[4];
    SCInputRef i_barsBack = sc.Input[5];

    SCSubgraphRef s_regLine = sc.Subgraph[0];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[1];
	SCInputRef Input_HorizontalPosition = sc.Input[6];
	SCInputRef Input_VerticalPosition = sc.Input[7];

    sc.AutoLoop = 0;

    if(sc.SetDefaults)
    {
        sc.GraphName = "linear regression v1.1";
        sc.GraphRegion = 0;

        i_lookBack.Name = "Number of bars back to start from";
        i_lookBack.SetInt(50);

        i_type.Name = "Select price type for each bar";
        i_type.SetCustomInputStrings("Open; Close; High; Low; Midpoint");
        i_type.SetCustomInputIndex(0);

        i_predict.Name = "Time forward to predict (in minutes)";
        i_predict.SetInt(5);

        i_rMin.Name = "Minimum r value to stop iterating?";
        i_rMin.SetFloat(0.7);

        i_r2Min.Name = "Minimum R2 value to stop iterating?";
        i_r2Min.SetFloat(0.7);

        i_barsBack.Name = "How many bars back to end regression?";
        i_barsBack.SetInt(1);

        s_regLine.Name = "regression line";
        s_regLine.PrimaryColor = RGB(255, 255, 255);
        s_regLine.LineStyle = LINESTYLE_SOLID;
        s_regLine.LineWidth = 2;

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

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//Getting data....................................................................................................................................................
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
    int userDefinedRange = i_lookBack.GetInt() - i_barsBack.GetInt();
    int chartbarperiod = 5;

    double priceArray[userDefinedRange] = {};
    int secondsArray[userDefinedRange] = {};
    double barRangeArray[userDefinedRange] = {};

    double * pPrice = &priceArray[0];
    int * sArray = &secondsArray[0];
    double * pBarRange = &barRangeArray[0];

    int startBarIndex = sc.ArraySize - i_lookBack.GetInt();
    int endBarIndex = sc.ArraySize - i_barsBack.GetInt();

    double slope = 0;
    double intercept = 0;
    double volatility1 = 0;
    double rValue = 0;
    double r2Value = 0;
    double ActualAngle = 0;
    double rAngle = 0;
    double predictVal = 0;

    double SSdeviations = 0;
    bool regRange = false;
    
    bool tresholdMet = false;

    msg.Format("range: %d", userDefinedRange);
    sc.AddMessageToLog(msg, 1);

//fill up arrays with data with the maximum range
//we will then access these two arrays for consecutive iterations

    for (int i = startBarIndex, j = 0 ; i < endBarIndex; i++, j++){

        if (i_type.GetIndex() == 0){

            *(pPrice + j) = sc.Open[i];
            *(sArray + j) = (j + 1) * chartbarperiod;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);
            
            msg.Format("counter: %d", j);
            sc.AddMessageToLog(msg, 1);
            
            *(pBarRange + j) = sc.High[i] - sc.Low[i];

        }
        else if (i_type.GetIndex() == 1){

            *(pPrice + j) = sc.Close[i];
            *(sArray + j) = (j + 1) * chartbarperiod;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("counter: %d", j);
            sc.AddMessageToLog(msg, 1);

            *(pBarRange + j) = sc.High[i] - sc.Low[i];   

        }
        else if (i_type.GetIndex() == 2){

            *(pPrice + j) = sc.High[i];
            *(sArray + j) = (j + 1) * chartbarperiod;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("counter: %d", j);
            sc.AddMessageToLog(msg, 1);

            *(pBarRange + j) = sc.High[i] - sc.Low[i];

        }
        else if (i_type.GetIndex() == 3){

            *(pPrice + j) = sc.Low[i];
            *(sArray + j) = (j + 1) * chartbarperiod;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("counter: %d", j);
            sc.AddMessageToLog(msg, 1);

            *(pBarRange + j) = sc.High[i] - sc.Low[i];
        }

        else if (i_type.GetIndex() == 4){

            *(pPrice + j) = (sc.High[i] + sc.Low[i]) / 2;
            *(sArray + j) = (j + 1) * chartbarperiod;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("counter: %d", j);
            sc.AddMessageToLog(msg, 1);

            *(pBarRange + j) = sc.High[i] - sc.Low[i];

        }

    }

//start iterating from i_lookback.getint() bars back, then go forwards until r and r2 value are within users preset threshold

    for (int i = 0, currentRange = userDefinedRange; i < userDefinedRange; i++, currentRange--){

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//Processing data.................................................................................................................................................
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
        double sumX = 0;
        double sumY = 0;
        double sumXY = 0;
        double sumX2 = 0;
        double sumY2 = 0;
        double meanY = 0;

        double SSresidual = 0;
        double SStotal = 0;

        double totalBarRange = 0;
        double maxBarRange = 0;

        predictVal = 0;

        for (int rangeCounter = 0, o = i; rangeCounter < currentRange; rangeCounter++, o++){
            
            sumX += *(sArray + o);
            sumY += *(pPrice + o);
            sumXY += *(sArray + o) * *(pPrice + o);
            sumX2 += *(sArray + o) * *(sArray + o);
            sumY2 += *(pPrice + o) * *(pPrice + o);

            totalBarRange += *(pBarRange + o);

            if (*(pBarRange + o) > maxBarRange){

                maxBarRange = *(pBarRange + o);

            }

            msg.Format("counter2: %d", o);
            //sc.AddMessageToLog(msg, 1);

        }

        msg.Format("sumx: %.02f, sumy: %.02f, sumxy: %.02f, sumx2: %.02f, sumy2: %.02f", sumX, sumY, sumXY, sumX2, sumY2);
        //sc.AddMessageToLog(msg, 1);

//calculations

        //regression equation
        slope = (currentRange * sumXY - sumX * sumY) / (currentRange * sumX2 - sumX * sumX);
        intercept = (sumY - slope * sumX) / currentRange;

        //msg.Format("slope: %.05f, intercept: %.05f, current range: %d", slope, intercept, currentRange);
        //sc.AddMessageToLog(msg, 1);

        //volatility
        volatility1 = totalBarRange / currentRange;

        //r value 
        rValue = (currentRange * sumXY - sumX * sumY) / (sqrt((currentRange * sumX2 - sumX * sumX) * (currentRange * sumY2 - sumY * sumY)));

        //r2value
        meanY = sumY / currentRange;

        for (int rangeCounter = 0, o = i; rangeCounter < currentRange; rangeCounter++, o++){

            SSresidual += pow(*(pPrice + o) - (slope * *(sArray + o) + intercept), 2);
            SStotal += pow(*(pPrice + o) - meanY, 2);

            SSdeviations = *(pPrice + o) - (slope * *(sArray + o) + intercept);

            if (abs(SSdeviations) < volatility1){

                regRange = true;

            }

        }

        r2Value = 1.0 - SSresidual/SStotal;

//break the loop when the minimum r and r2 value is met

        if (abs(rValue) > i_rMin.GetFloat() && r2Value > i_r2Min.GetFloat()){

            //angle
            rAngle = atan(abs(slope)) * 180 / M_PI;

            double pricedif = sc.Close[endBarIndex] - sc.Open[startBarIndex + i];
            int timedif = userDefinedRange * chartbarperiod;
            ActualAngle = atan(abs(pricedif) / timedif) * 180 / M_PI;

            msg.Format("break, r: %.05f, r2: %.05f, range: %d", rValue, r2Value, currentRange);
            sc.AddMessageToLog(msg, 1);

            msg.Format("Regression angle: %.02f, actual angle: %.02f", rAngle, ActualAngle);
            sc.AddMessageToLog(msg, 1);

            msg.Format("price dif: %.02f, timedif: %d", pricedif, timedif);
            sc.AddMessageToLog(msg, 1);

            predictVal = slope * (userDefinedRange * chartbarperiod + i_predict.GetInt() * chartbarperiod) + intercept;

            s_UseTool Tool1;
            Tool1.Clear();
            Tool1.ChartNumber = sc.ChartNumber;
            //Tool.LineNumber will be automatically set. No need to set this.
            Tool1.DrawingType = DRAWING_LINE;
            //Tool1.LineNumber =  1;
            Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - currentRange - i_barsBack.GetInt()];
            Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + i_predict.GetInt()]; 
            //Tool1.Text = "high";
            //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
            Tool1.Region = 0;
            Tool1.Color = RGB(255, 255, 255);
            Tool1.LineWidth = 2;
            Tool1.AddMethod = UTAM_ADD_ALWAYS;

            Tool1.BeginValue = slope * (secondsArray[0]) + intercept;
            Tool1.EndValue = predictVal;  
            sc.UseTool(Tool1);

            tresholdMet = true;

            //break;
        }

        if (tresholdMet == true){
            //break;
        }

        if (currentRange == 0 && tresholdMet == false){

            sc.AddMessageToLog("no regression line found", 1);
            msg.Format("no break, r: %.05f, r2: %.05f, range: %d", rValue, r2Value, currentRange);
            sc.AddMessageToLog(msg, 1);

        }

    }

    SCString ValueText;
    ValueText.Format("predicted price: %.02f \nslope: %.05f \nintercept: %.05f \nr value: %.05f \nr2 value: %.05f \nregression angle: %.05f \nactual angle: %.05f \nwithin volatilty: %d",predictVal, slope, intercept, rValue, r2Value, rAngle, ActualAngle, regRange);


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

