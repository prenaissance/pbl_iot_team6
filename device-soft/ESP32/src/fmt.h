std::string uint8_tToHexString(const uint8_t *data, size_t len)
{
    std::stringstream ss;
    ss << std::hex;

    for (size_t i = 0; i < len; ++i)
    {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }

    return ss.str();
}

std::string uuid4Format(const std::string &uuid_str)
{
    std::string formatted_uuid =
        uuid_str.substr(0, 8) + "-" +
        uuid_str.substr(8, 4) + "-" +
        uuid_str.substr(12, 4) + "-" +
        uuid_str.substr(16, 4) + "-" +
        uuid_str.substr(20);

    return formatted_uuid;
}

std::string getTimeFmt(bool f1, bool f2)
{
    std::string fmt;

    // bool f1 = rtc.getHour(true) < 10;
    // bool f2 = rtc.getMinute() < 10;

    if (f1 && f2)
    {
        fmt = "     0%d:0%d      ";
    }
    else if (f1)
    {
        fmt = "     0%d:%d      ";
    }
    else if (f2)
    {
        fmt = "     %d:0%d      ";
    }
    else
    {
        fmt = "     %d:%d      ";
    }

    return fmt;
}

std::string getDateFmt(bool f1, bool f2)
{
    std::string fmt;

    // bool f1 = rtc.getDay() < 10;
    // bool f2 = rtc.getMonth() + 1 < 10;

    if (f1 && f2)
    {
        fmt = "   0%d/0%d/%d   ";
    }
    else if (f1)
    {
        fmt = "   0%d/%d/%d   ";
    }
    else if (f2)
    {
        fmt = "   %d/0%d/%d   ";
    }
    else
    {
        fmt = "   %d/%d/%d   ";
    }

    return fmt;
}