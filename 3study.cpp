#include "sierrachart.h"
SCDLLName("study3")



SCSFExport scsf_study2(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCSubgraphRef s_BarCounter = sc.Subgraph[0];
   

    if (sc.SetDefaults)
    {
        sc.GraphName = "study3";
        sc.GraphRegion = 1;

        s_BarCounter.Name = "Second entry long counter";
        s_BarCounter.PrimaryColor = COLOR_BLUE;
        s_BarCounter.LineStyle = LINESTYLE_SOLID;

        return;
    }

    float &cLow = sc.GetPersistentFloat(0);
    float &fLow = sc.GetPersistentFloat(1);
    int &BarCounter = sc.GetPersistentInt(0);


    for (int i=0; i<sc.ArraySize; i++){

        SCDateTime BarDateTime = sc.BaseDateTimeIn[i];
            int Hour = BarDateTime.GetHour();
            int Minute = BarDateTime.GetMinute();
            if (Hour==9 && Minute ==30) {
                //reset
                cLow = 0;
                fLow = 0;
                BarCounter = 0;
            }
        
        if (sc.Low[i] < cLow) {
            if (BarCounter == 1 && sc.Low[i] > fLow) {
                BarCounter = 2;
                //cLow = sc.Low [i];
            }

            else if (BarCounter == 0) {
                cLow = sc.Low[i];
                fLow = cLow;
                BarCounter = 1;
            }

            else if (BarCounter == 2) {
                cLow = sc.Low[i];
                BarCounter= 1;
                //fLow = cLow;
            }

            else if (sc.Low[i] < fLow) {
                BarCounter = 0;
                cLow = sc.Low[i];
                fLow = cLow;
            }

        }

        else if (sc.Low[i] > cLow) { 
            cLow = sc.Low[i];

        }

        s_BarCounter[i] = BarCounter;
        
        }
}



/*
    if (sc.Low[i] < cLow) {
        if (BarCounter == 1 && sc.Low[i] > fLow) {
            BarCounter = 2;
            cLow = sc.low [i];
        }

        else if (BarCounter == 0)
            cLow = sc.Low[i];
            fLow = cLow;
            BarCounter = 1;

        else if (BarCounter == 2) {
            cLow = sc.Low[i];
            BarCounter= 0;
            fLow = cLow;
        }

    }

    else if (sc.Low[i] > cLow) { 
        cLow = sc.Low[i];

    }

    else if (sc.Low[i] < fLow && BarCounter != 0) {
        BarCounter = 0;
        fLow = sc.Low[i];
    }


*/



/*

 if (sc.Low[i] < cLow || cLow == 0) {
        cLow = sc.Low[i];
        fLow = cLow;  
        BarCounter = 1;
    }
    
    else if (sc.Low[i] > cLow && sc.Low[i] > fLow && BarCounter == 1) {
        BarCounter = 2;
    }

    else if (sc.Low[i] < fLow) {
        BarCounter = 0;
        fLow = sc.Low[i];
    }
    
    
    */