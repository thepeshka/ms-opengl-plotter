#include <iostream>
#include <complex>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

template <typename T>
struct Size{
    T width;
    T height;
};


template <typename T>
struct Point{
    T x;
    T y;
};

int get_ms_iteration(std::complex<double> c, double thresh, int max_iterations) {
    std::complex<double> z = c;
    for (int i = 0; i < max_iterations; i++) {
        if ((z * std::conj(z)).real() > thresh) return i;
        z = z * z + c;
    }
    return max_iterations;
}

float* draw_mbs(Size<int> wp, Size<double> span) {
    auto t0 = std::chrono::system_clock::now();

    auto* buffer = new float[wp.width * wp.height * 3];

    double span_center_x = -0.5;
    double span_center_y = 0;
    double pixel_height = span.height / wp.height;
    double pixel_width = span.width / wp.width;
    double span_x = span_center_x - span.width / 2;
    for (int pixel_x = 0; pixel_x < wp.width; pixel_x++) {
        double span_y = span_center_y + span.height / 2;
        for (int pixel_y = 0; pixel_y < wp.height; pixel_y++) {
            unsigned char iteration = get_ms_iteration(std::complex<double>(span_x, span_y), 4, 100);
            if (iteration == 100) {
                buffer[(pixel_y * wp.width + pixel_x) * 3] = 0;
                buffer[(pixel_y * wp.width + pixel_x) * 3 + 1] = 0;
                buffer[(pixel_y * wp.width + pixel_x) * 3 + 2] = 0;
            } else {
                buffer[(pixel_y * wp.width + pixel_x) * 3] = 0;
                buffer[(pixel_y * wp.width + pixel_x) * 3 + 1] = ((float) iteration / 100) * 0.5;
                buffer[(pixel_y * wp.width + pixel_x) * 3 + 2] = ((float) iteration / 100) * 1.5;
            }
            span_y -= pixel_height;
        }
        span_x += pixel_width;
    }

    auto t1 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = t1-t0;

    std::cout << elapsed_seconds.count() << std::endl;
    return buffer;
}

float* color_ramp(Size<int> wp) {
    auto* buffer = new float[wp.width * wp.height * 3];
    for (int y = 0; y < wp.height; y++) {
        for (int x = 0; x < wp.width; x++) {
            buffer[(y * wp.width + x) * 3] = (float) x / wp.width;
            buffer[(y * wp.width + x) * 3 + 1] = (float) y / wp.height;
            buffer[(y * wp.width + x) * 3 + 2] = 0;
        }
    }
    return buffer;
}

int main()
{
    Size<int> wp = {1920, 1080};
    Size<double> span = {7.11, 4};
    GLFWwindow* window;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int monitorsCount;
    auto monitors = glfwGetMonitors(&monitorsCount);
    int targetMon = 0;
    for (int i = 0; i < monitorsCount; i++) {
        if (std::string("HDMI-0") == glfwGetMonitorName(monitors[i])) {
            targetMon = i;
            break;
        }
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(wp.width, wp.height, "Hello World", nullptr, nullptr);
//    window = glfwCreateWindow(wp.width, wp.height, "Hello World", monitors[targetMon], nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    float* buff = draw_mbs(wp, span);
//    float* buff = color_ramp(wp);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels((GLsizei) wp.width, (GLsizei) wp.height, GL_RGB, GL_FLOAT, (GLvoid*)buff);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
