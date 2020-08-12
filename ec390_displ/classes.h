#ifndef _CLASSES
#define _CLASSES

enum class MessageLevel
{
    Info = 0,
    Warning,
    Error,
    Critical
};

enum class Menu
{
    Main = 0,
    Engine,
    Hydraulics,
    Settings
};

class Message
{
public:
    Message(MessageLevel level, char* message)
        : m_MessageLevel(level), m_Message(message), m_Accepted(false)
    {}

    ~Message()
    {
        delete m_Message;
    }

    void Print()
    {
        Serial.println(m_Message);
    }

private:
    MessageLevel m_MessageLevel;
    char* m_Message;
    bool m_Accepted;
};

#endif //_CLASSES
