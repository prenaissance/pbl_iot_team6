class DispenserDriver {
    private:
        Servo* pMotor;
        int initAngle = 70;
        int rotUnit = 36;

    public:
        DispenserDriver(Servo* pm) : pMotor(pm) {
            pMotor->write(initAngle);
        }

        void dispence(int cNum) {
            pMotor->write(initAngle + cNum * rotUnit);
            delay(500);
            pMotor->write(initAngle);
        }
};