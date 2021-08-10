#pragma once
#include "GLFW/glfw3.h"
#include <functional>
#include <map>
namespace gl {
    struct GWindow
    {
        GLFWwindow* window_{ nullptr };
        GWindow(int width, int height) {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
            window_ = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
            if (window_ == NULL)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();

            }
            glfwMakeContextCurrent(window_);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "Failed to initialize GLAD" << std::endl;
            }
            getWindowMap()[window_] = this;
        }
        ~GWindow() {
            getWindowMap()[window_] = nullptr;
            glfwDestroyWindow(window_);
        }

        GLFWwindow* window() {
            return window_;
        }
        void exec() {
            while (!glfwWindowShouldClose(window_))
            {
                // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
                // -------------------------------------------------------------------------------
                renderCallBack();
                glfwSwapBuffers(window_);
                glfwWaitEvents();
            }

        }
        template<typename Callback>
        void setWindowPosCallback(Callback callback) {
            posCallBack = std::move(callback);
            glfwSetWindowPosCallback(window_, [](GLFWwindow* window, int x, int y) {
                auto w = getWindow(window);
                if (w && w->posCallBack)
                    w->posCallBack(x, y);
                });
        }
        template<typename Callback>
        void setRenderCallback(Callback callback) {
            renderCallBack = std::move(callback);
            glfwSetWindowRefreshCallback(window_, [](GLFWwindow* window) {
                auto w = getWindow(window);
                if (w && w->renderCallBack)
                    w->renderCallBack();
                });

        }
        template<typename Callback>
        void setResizeCallback(Callback callback) {
            resizeCallBack = std::move(callback);
            glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int x, int y) {
                auto w = getWindow(window);
                if (w && w->resizeCallBack)
                    w->resizeCallBack(x, y);
                });
        }
        template<typename Callback>
        void setKeyCallback(Callback callback) {
            keycallback = std::move(callback);
            glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                auto w = getWindow(window);
                if (w && w->keycallback)
                    w->keycallback(key, scancode, action, mods);
                });
        }
        std::function<void()> posCallBack;
        std::function<void()> renderCallBack;
        std::function<void(int, int)> resizeCallBack;
        std::function<void(int key, int scancode, int action, int mods)> keycallback;


        static std::map< GLFWwindow*, GWindow*>& getWindowMap() {
            static std::map< GLFWwindow*, GWindow*>  gmap;
            return gmap;
        }
        static GWindow* getWindow(GLFWwindow* win) {
            return getWindowMap()[win];
        }

    };
}

