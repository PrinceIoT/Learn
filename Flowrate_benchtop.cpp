#include "mbed.h"

// Define the pin where the flow sensor is connected
#define FLOW_SENSOR_PIN PA_0  // Pin PA0 on Nucleo F439ZI

// Define variables
volatile int pulseCount = 0;  // Number of pulses counted
unsigned long oldTime = 0;    // Time at the last calculation

// Instantiate serial communication
mbed::UnbufferedSerial pc(USBTX, USBRX, 9600);  // tx, rx, baudrate

// Interrupt handler for the flow sensor pulse
void pulseCounter() {
    pulseCount++;
}

// Function to print data via serial
void printToSerial(const char *message) {
    pc.write(message, strlen(message));
}

// Main function
int main() {
    // Configure the pin as input with pull-up
    DigitalIn flowSensor(FLOW_SENSOR_PIN, PullUp);
    InterruptIn flowSensorInterrupt(FLOW_SENSOR_PIN);
    flowSensorInterrupt.rise(&pulseCounter);

    printToSerial("Flow sensor setup completed. Waiting for pulses...\n");

    while (true) {
        unsigned long currentTime = HAL_GetTick();
        unsigned long elapsedTime = currentTime - oldTime;

        if (elapsedTime >= 1000) { // One second has passed
            flowSensorInterrupt.disable_irq();

            if (pulseCount > 0) {
                // Calculate the flow rate in liters per minute
                float flowRate = (pulseCount / 5880.0) * 60.0;

                char buffer[50];
                sprintf(buffer, "Flow rate: %.2f L/min\n", flowRate);
                printToSerial(buffer);
            } else {
                printToSerial("No pulses detected.\n");
            }

            pulseCount = 0;
            oldTime = currentTime;

            flowSensorInterrupt.enable_irq();
        }
    }
}
