#define MANAGER_CAPACITY 2

class StateLedDriver {
    private:
        int R, G;

        void lightRed() {
            analogWrite(R, 255);
        }

        void lightGreen() {
            analogWrite(G, 255);
        }

        void lightYellow() {
            lightRed();
            lightGreen();
        }

    public:
        StateLedDriver() : R(-1), G(-1) {}

        StateLedDriver(int pinR, int pinG) :
            R(pinR), G(pinG) {
            resetColors();
        }

        void resetColors() {
            analogWrite(R, 0);
            analogWrite(G, 0);
        }

        void update(int stock) {
            resetColors();

            if (stock < 2) {
                lightRed();
            } else if (stock < 5) {
                lightYellow();
            } else {
                lightGreen();
            }
        }

        bool initCheck() {
            return R != -1;
        }
};

class StateLedDriverManager {
    private:
        StateLedDriver instances[MANAGER_CAPACITY];
    
    public:
        void init(StateLedDriver driverInstance, int id) {
            instances[id] = driverInstance;
        }

        void update(int stock, int id) {
            if (!instances[id].initCheck()) {
                Serial.println("ERR: Led called is not initialized properly!");
                return;
            }

            instances[id].update(stock);
        }
};