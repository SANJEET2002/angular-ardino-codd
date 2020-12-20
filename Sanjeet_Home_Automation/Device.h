#ifndef DEVICE_H__
#define DEVICE_H__
  #define ON     1
  #define OFF    0
  class Device {
    int id;
    static int total;
    bool currentState;
    struct Schedule {
      bool state;
      long wait;
      long start;
      bool scheduled;
    } schedule;
    public:
      Device() {
        this->id = total;
        total++;
      }
      ~Device() {
        total--;
      }
      bool getState() {
        return currentState;
      }
      static int length() {
        return total;
      }

      void createSchedule(long wait, bool state) {
        this->reset();
        this->schedule.wait = wait;
        this->schedule.state = state;
        this->schedule.scheduled = true;
        this->schedule.start = millis();
        Serial.println(String("Device will be ") + (state ? "ON ": "OFF ") + "after " + wait + "seconds");
      }

      void stop() {
        this->reset();
      }

      void setState(bool currentState) {
        stop();
        this->currentState = currentState;
        Print(currentState);
      }

      void update() {
        if (this->schedule.scheduled) {
          uint32_t timeEllapsed = millis() - this->schedule.start;
          if (timeEllapsed < this->schedule.wait) {
            Serial.println(String("Device will be updated after ") + ((this->schedule.wait - timeEllapsed)/1000.0) + "seconds");
          }
          if (timeEllapsed > this->schedule.wait) {
            Serial.println(String("\n\n device set to ") + (this->schedule.state ? "ON " : "OFF "));
            this->schedule.scheduled = false;
          }
        }
      }
      
      void reset() {
        this->schedule.wait = 0;
        this->schedule.state = false;
        this->schedule.scheduled = false;       
      }
  } device[8];
  int Device::total = 0;
#endif
