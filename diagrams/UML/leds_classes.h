classDiagram

LedManager *-- LedDriver

class LedDriver {
    -int R
    -int G

    -void lightRed()
    -void lightGreen()
    -void lightYellow()

    +void resetColors()
    +void update(int)
    +bool initCheck()
}

class LedManager {
    -LedDriver instances[2]

    +void update(int, int)
}
