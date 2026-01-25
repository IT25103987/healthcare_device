#ifndef FireBaseSndOBJ_h
#define FireBaseSndOBJ_h

#include <Arduino.h>
class FireBaseSndOBJ{
      private:
    int  steps;
    bool fallDetec;
    bool state;
    int avgBpm;
    float temp;
    float pressure;
    float Altitude;
    int FloorsClimbed;
    String Forecast;


    public:
    FireBaseSndOBJ() {}

    // Getters
    int getSteps() { return steps; }
    bool getFallDetec() { return fallDetec; }
    bool getState() { return state; }
    int getAvgBpm() { return avgBpm; }
    float getTemp() { return temp; }
    float getPressure() { return pressure; }
    float getAltitude() { return Altitude; }
    int getFloorsClimbed() { return FloorsClimbed; }
    String getForecast() { return Forecast; }

    // Setters
    void setSteps(int s) {
         steps = s; 
        }
    void setFallDetec(bool f) { 
        fallDetec = f; 
    }
    void setState(bool st) { 
        state = st; }
    void setAvgBpm(int bpm) { 
        avgBpm = bpm;
     }
    void setTemp(float t) {
         temp = t;
         }
    void setPressure(float p) {
         pressure = p; 
        }
    void setAltitude(float a) {
         Altitude = a; 
        }
    void setFloorsClimbed(int fc) { 
        FloorsClimbed = fc; 
    }
    void setForecast(String f) {
         Forecast = f; 
        
        }

};
#endif
