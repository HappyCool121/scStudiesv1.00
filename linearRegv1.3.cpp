#include "sierrachart.h"
#include <cmath>
#include <iostream>
#include <math.h>
SCDLLName("linearRegv1.3")


SCSFExport scsf_linearRegv1(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_lookBack = sc.Input[0];
    SCInputRef i_type = sc.Input[1];
    SCInputRef i_predict = sc.Input[2];
    SCInputRef i_barsBack = sc.Input[3];

    SCSubgraphRef s_regLine = sc.Subgraph[3];

    SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[4];
	SCInputRef Input_HorizontalPosition = sc.Input[5];
	SCInputRef Input_VerticalPosition = sc.Input[6];

    sc.AutoLoop = 0;

    if(sc.SetDefaults)
    {
        sc.GraphName = "linear regression v1.3";
        sc.GraphRegion = 0;

        i_lookBack.Name = "Number of bars";
        i_lookBack.SetInt(60);

        i_type.Name = "Select price type for each bar";
        i_type.SetCustomInputStrings("Open; Close; High; Low; Midpoint");
        i_type.SetCustomInputIndex(0);

        i_predict.Name = "Time forward to predict (in minutes)";
        i_predict.SetInt(5);

        i_barsBack.Name = "How many bars back from latest bar to stop calculating?";
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

//.......................................getting data.......................................


//getting price data 

    for (int il = i_lookBack.GetInt(); il > 10; il--){

    double priceArray[il] = {};
    int secondsArray[il] = {};
    double priceRange[il] = {};

    double * pPrice = &priceArray[0];
    int * pTime = &secondsArray[0];
    double * pRange = &priceRange[0];

    double startPrice = sc.Open[sc.ArraySize - il];
    double endPrice = sc.Close[sc.ArraySize - 1];

    int barRange = il;

    for (int i = (sc.ArraySize - il), j = 0; i < sc.ArraySize; i++, j++){

        if (i_type.GetIndex() == 0){

            *(pPrice + j) = sc.Open[i];
            *(pTime + j) = sc.BaseDateTimeIn[i].GetTimeInSeconds() / 60;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            *(pRange + j) = sc.High[i] - sc.Low[i];

        }
        else if (i_type.GetIndex() == 1){

            *(pPrice + j) = sc.Close[i];
            *(pTime + j) = sc.BaseDateTimeIn[i].GetTimeInSeconds() / 60;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            *(pRange + j) = sc.High[i] - sc.Low[i];   

        }
        else if (i_type.GetIndex() == 2){

            *(pPrice + j) = sc.High[i];
            *(pTime + j) = sc.BaseDateTimeIn[i].GetTimeInSeconds() / 60;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            
            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            *(pRange + j) = sc.High[i] - sc.Low[i];

        }
        else if (i_type.GetIndex() == 3){

            *(pPrice + j) = sc.Low[i];
            *(pTime + j) = sc.BaseDateTimeIn[i].GetTimeInSeconds() / 60;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            *(pRange + j) = sc.High[i] - sc.Low[i];
        }

        else if (i_type.GetIndex() == 4){

            *(pPrice + j) = (sc.High[i] + sc.Low[i]) / 2;
            *(pTime + j) = sc.BaseDateTimeIn[i].GetTimeInSeconds() / 60;

            msg.Format("Price: %f", priceArray[j]);
            sc.AddMessageToLog(msg, 1);

            msg.Format("time in seconds: %d", secondsArray[j]);
            sc.AddMessageToLog(msg, 1);

            *(pRange + j) = sc.High[i] - sc.Low[i];

        }

    }

    
    
//.......................................processing data.......................................
    
    double sumX = 0;
    double sumY = 0;
    double sumXY = 0;
    double sumX2 = 0;
    double sumY2 = 0;

    double totalRange = 0;

    double maxrange = 0;

    for (int i = 0; i < il; i++){

        sumX += *(pTime + i);
        sumY += *(pPrice + i);
        sumXY += (*(pTime + i) * *(pPrice + i));
        sumX2 += (*(pTime + i) * *(pTime + i));
        sumY2 += (*(pPrice + i) * *(pPrice + i));

        totalRange += *(pRange + i);

        if (*(pRange + i) > maxrange){
            maxrange = *(pRange + i);
        }

    }

// calculation of regression equation

    msg.Format("sumx: %f, sumy: %f, sumxy: %f, sumx2: %f", sumX, sumY, sumXY, sumX2);
    sc.AddMessageToLog(msg, 1);

    double slopeN = il * sumXY - sumX * sumY;
    double slopeD = il * sumX2 - sumX * sumX;
    double slope = slopeN / slopeD;

    msg.Format("num: %f, denom: %f, slope: %f", slopeN, slopeD, slope);
    sc.AddMessageToLog(msg, 1);

    double bN = sumY - slope * sumX;
    double bD = il;
    double b = bN / bD;

    msg.Format("%f %f %f", bN, bD, b);
    sc.AddMessageToLog(msg, 1);

    double predictVAL = slope * (secondsArray[il - 1] + i_predict.GetInt()) + b;

    msg.Format("predicted price %d minutes ahead: %.02f", i_predict.GetInt(), predictVAL);
    sc.AddMessageToLog(msg, 1);

    double Vol = totalRange / il;

//calculation of r values

    double rN = il * sumXY - sumX * sumY;
    double rD = sqrt((il * sumX2 - sumX * sumX) * (il * sumY2 - sumY * sumY));
    double r = rN/rD;

    msg.Format("r value: %f", r);
    sc.AddMessageToLog(msg, 1);

//calculation of r^2 values

    double SSresidual = 0;
    double SStotal = 0;
    double r2 = 0;
    double meanY = sumY / il;

    for (int i = 0; i < il; i++){
        SSresidual += pow(*(priceArray + i) - (slope * *(secondsArray + i) + b), 2);
        SStotal += pow(*(priceArray + i) - meanY, 2);
    }

    r2 = 1.0 - SSresidual/SStotal;

    msg.Format("r^2: %f", r2);
    sc.AddMessageToLog(msg, 1);


//calculation of regression equation angle

    double angle = atan(abs(slope)) * 180 / M_PI;

    msg.Format("angle: %.02f", angle);
    sc.AddMessageToLog(msg, 1);

//calculation of effective price chnage angle

    double pricedif = endPrice - startPrice;
    double timedif = *(secondsArray + il - 1) - *(secondsArray);
    double eAngle = atan(abs(pricedif)/timedif) * 180 /M_PI;

    msg.Format("Effective angle: %.02f, price: %.02f, time: %.0f, start: %.02f, close: %.02f", eAngle, pricedif, timedif, sc.Open[sc.ArraySize - il], sc.Close[sc.ArraySize - 1]);
    sc.AddMessageToLog(msg, 1);

//...........................................................Drawing lines...........................................................

    msg.Format("r value : %f", r);

    s_UseTool Tool1;
    Tool1.Clear();
    Tool1.ChartNumber = sc.ChartNumber;
    //Tool.LineNumber will be automatically set. No need to set this.
    Tool1.DrawingType = DRAWING_LINE;
    //Tool1.LineNumber =  1;
    Tool1.BeginDateTime = sc.BaseDateTimeIn[sc.ArraySize - il];
    Tool1.EndDateTime = sc.BaseDateTimeIn[sc.ArraySize + i_predict.GetInt()]; 
    //Tool1.Text = "high";
    //Tool1.TextAlignment = DT_BOTTOM | DT_RIGHT;
    Tool1.Region = 0;
    Tool1.Color = RGB(255, 255, 255);
    Tool1.LineWidth = 2;
    Tool1.AddMethod = UTAM_ADD_ALWAYS;

    Tool1.BeginValue = sc.Open[sc.ArraySize - il];
    Tool1.EndValue = predictVAL;  
    sc.UseTool(Tool1);

    }
/*
    SCString ValueText;
    ValueText.Format("Predicted price: %.0f \nr value: %.05f \nr^2 value: %.05f \n volatility: %.02f \nregression angle: %.02f \nreal angle: %.02f", predictVAL, r, r2, Vol, angle, eAngle);


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
*/

}

