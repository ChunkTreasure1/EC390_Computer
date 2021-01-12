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
    Settings,
    Messages,
    None
};

class Timestep
{
public:
  Timestep(float time = 0.0f)
    : m_Time(time)
  { }

  operator float() const { return m_Time; }

  float GetSeconds() const { return m_Time; }
  float GetMilliseconds() const { return m_Time * 1000.f; }
private:
  float m_Time;
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

  vec2& operator+(const vec2& other)
  {
    x += other.x;
    y += other.y;

    return *this;
  }

  vec2& operator-(const vec2& other)
  {
    x -= other.x;
    y -= other.y;

    return *this;
  }
};

#endif //_CLASSES