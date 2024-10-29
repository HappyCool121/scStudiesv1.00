#include "sierrachart.h"
SCDLLName("happyAmazingWow-1st study!!!")

SCSFExport scsf_happycool(SCStudyInterfaceRef sc)
{
    SCString msg;

    SCInputRef i_favouriteNumber = sc.Input[0];

    if (sc.SetDefaults)
    {
        sc.GraphName = "Happy Cool";
        sc.GraphRegion = 0;

        i_favouriteNumber.Name = "what is your favourite number?";
        i_favouriteNumber.SetInt(99);
        return;
    }

    msg.Format("Favourite Number = %d", i_favouriteNumber.GetInt());
    sc.AddMessageToLog(msg, 1);


}