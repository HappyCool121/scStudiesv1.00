#include "sierrachart.h"
SCDLLName("study2")



SCSFExport scsf_study2(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCSubgraphRef s_NumHighs = sc.Subgraph[0];
    SCSubgraphRef s_NumLows = sc.Subgraph[1];
    SCSubgraphRef s_BarCounter = sc.Subgraph[2];

    if (sc.SetDefaults)
    {
        sc.GraphName = "study2";
        sc.GraphRegion = 2;

        s_NumHighs.Name = "Number of highs";
        s_NumHighs.PrimaryColor = COLOR_YELLOW;
        s_NumHighs.LineStyle = LINESTYLE_SOLID;

        s_NumLows.Name = "Number of lows";
        s_NumLows.PrimaryColor = COLOR_RED;
        s_NumLows.LineStyle = LINESTYLE_SOLID;

        s_BarCounter.Name = "Second entry long counter";
        s_BarCounter.PrimaryColor = COLOR_BLUE;
        s_BarCounter.LineStyle = LINESTYLE_SOLID;

        return;
    }

    //count the number of highs and lows
    //a particular symbol in a day
    //for a given bar period

    //grab the memory address of f0, so we can hold this variable between function calls
    float &yHOD = sc.GetPersistentFloat(0);
    int &NumHighs = sc.GetPersistentInt(0);
    float &yLOD = sc.GetPersistentFloat(1);
    int &NumLows = sc.GetPersistentInt(1);

    float &cLow = sc.GetPersistentFloat(2);
    int &BarCounter = sc.GetPersistentInt(2);
    

    for (int i=0; i<sc.ArraySize; i++){

    //reset at the new day
        SCDateTime BarDateTime = sc.BaseDateTimeIn[i];
        int Hour = BarDateTime.GetHour();
        int Minute = BarDateTime.GetMinute();
        if (Hour==9 && Minute ==30) {
            //reset
            yHOD = 0;
            NumHighs = 0; 
            yLOD = 10000;
            NumLows = 0;
            cLow = 0;
            BarCounter = 0;
        }

    //initialise the yHOD
    //check if the current bar is higher than the yHOD
        if (sc.High[i] > yHOD) {
            yHOD = sc.High[i];
            NumHighs++;
        }

        if (sc.Low[i] < yLOD) {
            yLOD = sc.Low[i];
            NumLows++;
        }

        if (sc.Low[i] > cLow) {
            cLow = sc.Low[i];
            BarCounter++;
        }

        else if (sc.Low[i] < cLow) {
            cLow = sc.Low[i];
            BarCounter = 0;
        }


        //plot the number of highs
        s_NumHighs[i] = NumHighs;
        s_NumLows[i] = NumLows;
        s_BarCounter[i] = BarCounter;
    
    }
    
}

//a new low -> bar counter set to one (first low)
//we keep getting higher lows, bar counter remains at one
//if we keep getting lower lows, bar counter reamins at one 
//once we make a new low, bar counter increments to 2
//if we make lower lows, bar counter still remains at 2
// but if a newer low is lower than very first low, bar counter set back to 1

/*

second entry long counter

    float &cLow = sc.GetPersistentFloat(2);
    float &1Low = sc.GetPersistentFloat(3);
    int &BarCounter = sc.GetPersistentInt(2);
    

    clow = 0;
    1low  = 0; 
    BarCounter = 0;

    if (sc.Low[i] > clow) {
        clow = sc.Low[i];
        1low = sc.Low[i];
        BarCounter = 1;

    }

    else if (sc.Low[i] > clow && sc.Low[i] > 1low && Barcounter == 1) {
        clow = sc.Low[1];
        1low = 0;
        BarCounter == 2;
    
    }










*/

