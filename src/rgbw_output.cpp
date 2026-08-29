#include "rgbw_output.h"

#include "esp32-hal-rmt.h"

namespace
{
    // 10 MHz -> one RMT tick = 100 ns
    constexpr uint32_t RMT_FREQUENCY = 10000000;

    // SK6812 timing in 100 ns ticks
    //
    // logical 0: 0.3 us HIGH, 0.9 us LOW
    // logical 1: 0.6 us HIGH, 0.6 us LOW
    constexpr uint16_t T0H = 3;
    constexpr uint16_t T0L = 9;
    constexpr uint16_t T1H = 6;
    constexpr uint16_t T1L = 6;

    // Generous latch/reset time after every frame.
    constexpr uint16_t LATCH_US = 300;

    uint8_t outputPin = 0;
    uint16_t configuredLedCount = 0;

    rmt_data_t *rmtBuffer = nullptr;
    size_t rmtBufferSize = 0;

    bool initialized = false;


    void encodeBit(rmt_data_t &symbol, bool value)
    {
        symbol.level0 = 1;
        symbol.level1 = 0;

        if (value)
        {
            symbol.duration0 = T1H;
            symbol.duration1 = T1L;
        }
        else
        {
            symbol.duration0 = T0H;
            symbol.duration1 = T0L;
        }
    }


    void encodeByte(rmt_data_t *symbols, uint8_t value)
    {
        for (uint8_t bit = 0; bit < 8; ++bit)
        {
            encodeBit(
                symbols[bit],
                (value & (0x80 >> bit)) != 0
            );
        }
    }
}


bool rgbwOutputInit(uint8_t pin, uint16_t ledCount)
{
    if (ledCount == 0)
    {
        Serial.println("RGBW RMT: invalid LED count");
        return false;
    }

    outputPin = pin;
    configuredLedCount = ledCount;

    // One RMT symbol per transmitted bit:
    // 32 bits per RGBW LED.
    rmtBufferSize =
        static_cast<size_t>(configuredLedCount) * 32;

    rmtBuffer = new rmt_data_t[rmtBufferSize];

    if (rmtBuffer == nullptr)
    {
        Serial.println("RGBW RMT: buffer allocation failed");
        return false;
    }

    /*
     * Classic ESP32: use all eight RMT memory blocks.
     *
     * With only one block we observed visible transmission
     * glitches at higher frame rates. Eight blocks greatly
     * reduce the number of buffer refills required during a
     * 32-LED RGBW frame.
     */
    initialized = rmtInit(
        outputPin,
        RMT_TX_MODE,
        RMT_MEM_NUM_BLOCKS_8,
        RMT_FREQUENCY
    );

    if (!initialized)
    {
        delete[] rmtBuffer;
        rmtBuffer = nullptr;
        rmtBufferSize = 0;

        Serial.println("RGBW RMT: initialization failed");
        return false;
    }

    // Keep the data line LOW after transmission.
    rmtSetEOT(outputPin, LOW);

    Serial.print("RGBW RMT initialized for ");
    Serial.print(configuredLedCount);
    Serial.print(" LEDs, ");
    Serial.print(rmtBufferSize);
    Serial.println(" RMT symbols");

    return true;
}


bool rgbwOutputShow(const CRGBW *leds, uint16_t count)
{
    if (!initialized || rmtBuffer == nullptr || leds == nullptr)
    {
        return false;
    }

    if (count == 0 || count > configuredLedCount)
    {
        return false;
    }

    size_t symbolIndex = 0;

    for (uint16_t i = 0; i < count; ++i)
    {
        /*
         * SK6812 RGBW wire order used by our LEDs:
         *
         *   G R B W
         */

        encodeByte(
            &rmtBuffer[symbolIndex],
            leds[i].g
        );
        symbolIndex += 8;

        encodeByte(
            &rmtBuffer[symbolIndex],
            leds[i].r
        );
        symbolIndex += 8;

        encodeByte(
            &rmtBuffer[symbolIndex],
            leds[i].b
        );
        symbolIndex += 8;

        encodeByte(
            &rmtBuffer[symbolIndex],
            leds[i].w
        );
        symbolIndex += 8;
    }

    const bool success = rmtWrite(
        outputPin,
        rmtBuffer,
        symbolIndex,
        RMT_WAIT_FOR_EVER
    );

    // SK6812 reset/latch interval.
    delayMicroseconds(LATCH_US);

    return success;
}