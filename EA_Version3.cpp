#include <Trade\Trade.mqh>

CTrade trade;
int orderPlaced = 0; // Variable to track whether an order has been placed

//--- input parameters
input double   lot = 0.01;
input int      minCandleParts = 10;
input int      stopLossParts = 2;
input int      takeProfitParts = 8;

void OnTick()
{
    // Check if an order has already been placed for the current candle
    if (orderPlaced == iTime(_Symbol, PERIOD_CURRENT, 0))
        return; // Skip the current tick if an order has already been placed

    // Get the last 10 candles
    int candlesToRetrieve = 10;
    MqlRates rates[];
    ArraySetAsSeries(rates, true);
    CopyRates(_Symbol, PERIOD_CURRENT, 0, candlesToRetrieve, rates);

    // Find the candle with the minimum volume
    double minVolume = rates[0].tick_volume;
    int minVolumeIndex = 0;
    for (int i = 1; i < candlesToRetrieve; i++)
    {
        if (rates[i].tick_volume < minVolume)
        {
            minVolume = rates[i].tick_volume;
            minVolumeIndex = i;
        }
    }

    // Calculate the volume for each part
    double partVolume = minVolume / minCandleParts;

    // Calculate stop loss and take profit volumes
    double stopLossVolume = stopLossParts * partVolume;
    double takeProfitVolume = takeProfitParts * partVolume;

    // Set the trading signal based on the volume of the minimum volume candle
    string signal = (rates[minVolumeIndex].close > rates[minVolumeIndex].open) ? "Buy" : "Sell";

    // Check for pending orders
    if (PositionsTotal() < 1 && HistoryOrdersTotal() == 0)
    {
        double askPrice = SymbolInfoDouble(_Symbol, SYMBOL_ASK);
        double bidPrice = SymbolInfoDouble(_Symbol, SYMBOL_BID);

        // Execute the trade
        int ticket = 0;
        if (signal == "Buy")
        {
            ticket = trade.Buy(lot, _Symbol, askPrice, (askPrice - stopLossVolume * _Point), (askPrice + takeProfitVolume * _Point), "Buy order comment");
        }
        else if (signal == "Sell")
        {
            //ticket = trade.Sell(lot, _Symbol, bidPrice, stopLossVolume * _Point, takeProfitVolume * _Point, "Sell order comment");
        }

        if (ticket > 0)
        {
            Print(signal + " order placed successfully. Ticket: ", ticket);

            // Set the orderPlaced variable to the current candle's timestamp
            orderPlaced = iTime(_Symbol, PERIOD_CURRENT, 0);
        }
        else
        {
            Print("Error placing " + signal + " order. Error code: ", GetLastError());
        }
    }

    // Display the current signal, minimum volume, and calculated volumes in the comment section
    Comment("Signal now: " + signal +
            ", Min Volume Candle Volume: " + DoubleToString(minVolume, 2) +
            ", Stop Loss Volume: " + DoubleToString(stopLossVolume, 2) +
            ", Take Profit Volume: " + DoubleToString(takeProfitVolume, 2));
}
