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

    void Print();

private:
    MessageLevel m_MessageLevel;
    char* m_Message;
    bool m_Accepted;
};

struct vec2
{
  vec2(float x = 0, float y = 0)
    : x(x), y(y)
  {}

  float x;
  float y;
};

#endif //_CLASSES
