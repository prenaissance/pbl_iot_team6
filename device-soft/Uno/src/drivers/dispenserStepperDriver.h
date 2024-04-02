#define MANAGER_CAPACITY 2
#define CONTROL_PINS_NUM 4

#define STEPS_IN_REV 2550

class DispenserStepperDriver {
    private:
        int pins[CONTROL_PINS_NUM];
        int stepNum;
        bool dir;
        int revCnt;

    public:
        DispenserStepperDriver() : stepNum(-1), revCnt(-1), dir(false) {
            for (int i = 0; i < CONTROL_PINS_NUM; i++) { pins[i] = -1; }
        }

        DispenserStepperDriver(int p1, int p2, int p3, int p4) : stepNum(0), revCnt(0), dir(false) {
            pins[0] = p1; pins[1] = p2; pins[2] = p3; pins[3] = p4;

            for (int i = 0; i < CONTROL_PINS_NUM; i++) {
                digitalWrite(pins[i], LOW);
            }
        }

        void OneStep() {
            for (int i = 0; i < CONTROL_PINS_NUM; i++) {
                digitalWrite(pins[i], LOW);
            }

            if (dir) {
                digitalWrite(pins[stepNum], HIGH);
            } else {
                digitalWrite(pins[CONTROL_PINS_NUM-1-stepNum], HIGH);
            }

            if (stepNum++ > CONTROL_PINS_NUM - 1) {
                stepNum = 0;
            }

            if (revCnt++ > STEPS_IN_REV) {
                revCnt = 0;
            }
        }

        void finishRevolution() {
            if (revCnt > 0) {
                this->OneStep();
            }
        }
        
        bool initCheck() {
            return revCnt != -1;
        }

        bool revCheck() {
            return revCnt > 0;
        }
};

class DispenserStepperDriverManager {
    private:
        DispenserStepperDriver instances[MANAGER_CAPACITY];
    
    public:
        void init(DispenserStepperDriver driverInstance, int id) {
            instances[id] = driverInstance;
        }

        void initRevolution(int id) {
            if (!instances[id].initCheck()) {
                Serial.println("ERR: Motor called is not initialized properly!");
                return;
            } else if (instances[id].revCheck()) {
                Serial.println("ERR: Motor called is already preforming revolution!");
                return;
            }

            instances[id].OneStep();
        }

        void manageRevolutions() {
            for (int i = 0; i < MANAGER_CAPACITY; i++) {
                if (instances[i].initCheck()) {
                    instances[i].finishRevolution();
                }
            }
        }
};