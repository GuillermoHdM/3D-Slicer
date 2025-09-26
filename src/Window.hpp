#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Math.hpp"
#include "OpenGl.hpp"

class Window {
  private:
    GLFWwindow* m_window = nullptr;
    ivec2       m_size   = {};

  public:
    Window(ivec2 size, bool visible = true);/*WARNING this was explicit, possible breaking point*/
    ~Window();
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    void        update();
    bool        should_exit();
    ivec2       size() const { return m_size; }
    GLFWwindow* handle() const { return m_window; }

    // Common to all windows
    static void initialize_system();
    static void destroy_system();
};


#endif // WINDOW_HPP
