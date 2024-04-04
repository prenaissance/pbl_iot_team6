#define QUEUE_MAX_SIZE 64

class LcdMsg {
    private:
        char line1[16], line2[16];
        int priority;
        int lifetime;

    public:
        LcdMsg() {}

        LcdMsg(const char* l1, const char* l2, int pr, int lt) : priority(pr), lifetime(lt) {
            strncpy(line1, l1, sizeof(line1) - 1); line1[sizeof(line1) - 1] = '\0';
            strncpy(line2, l2, sizeof(line2) - 1); line2[sizeof(line2) - 1] = '\0';
        }

        const char* getL1() { return line1; }
        const char* getL2() { return line2; }

        int getPriority() { return priority; }

        bool checkLifetime(int dt) { 
            lifetime -= dt;
            return lifetime > 0; 
        }
};

class MsgQueue {
    private:
        LcdMsg queue[QUEUE_MAX_SIZE];
        int queueSize;

    public:
        MsgQueue() : queueSize(0) {}

        void enqueue(LcdMsg msg) {
            if (queueSize < QUEUE_MAX_SIZE) {
                queue[queueSize] = msg;
                queueSize++;
            } else {
                Serial.println("ERR: Queue overflow!");
            }
        }

        int pickNext() {
            int maxPriorityIdx = 0;
            int maxPriority = queue[maxPriorityIdx].getPriority();

            for (int i = 0; i < queueSize; i++) {
                if (queue[i].getPriority() > maxPriority) {
                    maxPriorityIdx = i;
                    maxPriority = queue[maxPriorityIdx].getPriority();
                }
            }

            return maxPriorityIdx;
        }

        int updCurrent(int currentIdx) {
            if (queueSize == 0) {
                return -1;
            } else {
                return pickNext();    
            }
        }

        void deleteExpired(int exp) {
            for (int i = exp; i < queueSize - 1; i++) {
                queue[i] = queue[i+1];
            }

            queueSize--;
        }

        LcdMsg* accessCurr(int currentIdx) {
            return &queue[currentIdx];
        }
};

class LcdDriver {
    private:
        LiquidCrystal_I2C* lcd;
        MsgQueue mq;
        int current;
        unsigned long lastUpdate;

    public:
        LcdDriver(LiquidCrystal_I2C* lcdPtr) : lcd(lcdPtr), current(-1), lastUpdate(0) {
            reset();
        }

        void reset() {
            lcd->clear(); lcd->setCursor(0, 0);
        }

        void update() {
            if (current != -1) {
                if (!mq.accessCurr(current)->checkLifetime(millis() - lastUpdate)) {
                    mq.deleteExpired(current);
                    current = -1;
                }
            }

            if (current == -1) {
                current = mq.updCurrent(current);

                if (current != -1) {
                    display(mq.accessCurr(current)->getL1(), mq.accessCurr(current)->getL2());
                } else {
                    reset();
                }
            }

            lastUpdate = millis();
        }

        void display(const char* l1, const char* l2) {
            reset();
            lcd->print(l1); lcd->setCursor(0, 1); lcd->print(l2);
        }

        MsgQueue* accessQueue() {
            return &mq;
        }
};