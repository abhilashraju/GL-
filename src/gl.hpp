#pragma once
#include "glad/glad.h"

#include <iostream>
#include <string>
#include <array>
#include <optional>
#include <assert.h>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace gl {
    // template vtable for generic unitoform function
    template<typename T>struct glUniform;
    template<>
    struct glUniform<float> {
        static const auto args1() { return glUniform1f; }
        static const auto args2() { return glUniform2f; }
        static const auto args3() { return glUniform3f; }
        static const auto args4() { return glUniform4f; }
    };
    template<>
    struct glUniform<GLint> {
        static const auto args1() { return glUniform1i; }
        static const auto args2() { return glUniform2i; }
        static const auto args3() { return glUniform3i; }
        static const auto args4() { return glUniform4i; }
    };
    template<>
    struct glUniform<GLuint> {
        static const auto args1() { return glUniform1ui; }
        static const auto args2() { return glUniform2ui; }
        static const auto args3() { return glUniform3ui; }
        static const auto args4() { return glUniform4ui; }
    };
    struct GShader
    {
        using Shader = std::optional<GLuint>;
        using value_type = typename Shader::value_type;
        Shader shader;
        std::array<char, 512> log;
        GShader(const char* data, GLuint type) {
            shader = glCreateShader(type);
            glShaderSource(*shader, 1, &data, NULL);
            glCompileShader(*shader);
            int  success;
            glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(*shader, log.size(), NULL, log.data());
                shader = std::nullopt;
            }

        }
        
        GShader(const std::istream& stream, GLuint type) {
            std::stringstream str;
            str << stream.rdbuf();
            std::string datastr = str.str().c_str();
            const char* data = datastr.c_str();
            shader = glCreateShader(type);
            glShaderSource(*shader, 1, &data, NULL);
            glCompileShader(*shader);
            int  success;
            glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(*shader, log.size(), NULL, log.data());
                shader = std::nullopt;
            }

        }

        ~GShader() {
            if (shader) {
                glDeleteShader(*shader);
            }

        }
        GShader(const GShader&) = delete;
        GShader& operator=(const GShader&) = delete;
        GShader(GShader&& o) {
            if (o) {
                shader = std::move(o.shader);
                o.shader = std::nullopt;
                return;
            }
            std::memcmp(log.data(), o.log.data(), log.size());
        }
        GShader& operator=(GShader&& o) {
            if (o) {
                shader = std::move(o.shader);
                o.shader = std::nullopt;
                return *this;
            }
            std::memcmp(log.data(), o.log.data(), log.size());
            return *this;
        }
        operator Shader()const { return shader; }
        operator bool()const { return shader != std::nullopt; }
        value_type operator* ()const { return *shader; }

    };

    struct GProgramme {
        using Programme = std::optional<GLuint>;
        using value_type = typename Programme::value_type;
        Programme pgm;
        std::array<char, 512> log{ 0 };
        GProgramme() {}
        GProgramme(const GShader& v, const GShader& f) {
            pgm = glCreateProgram();
            glAttachShader(*pgm, *v);
            glAttachShader(*pgm, *f);
            glLinkProgram(*pgm);
            int success{};

            glGetProgramiv(*pgm, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(*pgm, log.size(), NULL, log.data());
                pgm = std::nullopt;
            }

        }
        ~GProgramme() {
            if (pgm) {
                glDeleteProgram(*pgm);
            }
        }
        GProgramme(const GProgramme&) = delete;
        GProgramme& operator=(const GProgramme&) = delete;
        GProgramme(GProgramme&& o) noexcept {
            if (o) {
                pgm = std::move(o.pgm);
                o.pgm = std::nullopt;
                return;
            }
            std::memcpy(log.data(), o.log.data(), log.size());
        }
        GProgramme& operator=(GProgramme&& o) {
            if (o) {
                pgm = std::move(o.pgm);
                o.pgm = std::nullopt;
                return *this;
            }
            std::memcpy(log.data(), o.log.data(), log.size());
            return *this;
        }
        operator Programme()const { return pgm; }
        operator bool()const { return pgm != std::nullopt; }
        value_type operator* ()const { return *pgm; }
        void use() {
            glUseProgram(*pgm);
        }
        template<int I>
        struct IntoType {
            enum { value = I };
        };
        
        template<typename Type,typename... T>
        void setUniformImpl(int vertexColorLocation, const std::tuple<T...>& t, IntoType<4>) {
            glUniform< Type>::args4()(vertexColorLocation, std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));
        }
        template<typename Type, typename... T>
        void setUniformImpl(int vertexColorLocation, const std::tuple<T...>& t, IntoType<3>) {
            glUniform< Type>::args3()(vertexColorLocation, std::get<0>(t), std::get<1>(t), std::get<2>(t));
        }
        template<typename Type, typename... T>
        void setUniformImpl(int vertexColorLocation, const std::tuple<T...>& t, IntoType<2>) {
            glUniform< Type>::args2()(vertexColorLocation, std::get<0>(t), std::get<1>(t));
        }
        template<typename Type, typename... T>
        void setUniformImpl(int vertexColorLocation, const std::tuple<T...>& t, IntoType<1>) {
            glUniform< Type>::args1()(vertexColorLocation, std::get<0>(t));
        }
        template<typename... ARGS>
        void setUniform(const std::string& name, ARGS... args)
        {
            auto t = std::make_tuple(args...);
            constexpr size_t length = std::tuple_size<decltype(t)>::value;
            using argtype = std::remove_reference_t<decltype(std::get<0>(t))>;
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            setUniformImpl<argtype>(vertexColorLocation, t, IntoType<length>{});
        }
    };

    auto make_shader(const char* data, GLuint type) {
        return [=](auto on_failure) {
            GShader shader(data, type);
            if (!shader) {
                on_failure(shader.log.data());
            }
            return std::move(shader);
        };
    }
    auto make_shader(const std::istream& stream, GLuint type) {
        return [&stream,type=type](auto on_failure) {
            GShader shader(stream, type);
            if (!shader) {
                on_failure(shader.log.data());
            }
            return std::move(shader);
        };
    }
    template<typename V, typename F>
    auto make_programme(V v, F f) {
        return[v = std::move(v), f = std::move(f)](auto on_failure) {
            auto vshader = v(on_failure);
            auto fshader = f(on_failure);
            GProgramme pgm;
            if (vshader && fshader) {
                pgm = std::move(GProgramme(vshader, fshader));
            }
            if (!pgm) {
                on_failure(pgm.log.data());
            }
            return pgm;
        };
    }
    template<size_t SIZE>
    struct VBOS {
        struct Bounded {
            unsigned index{ -1 };
            GLenum target;
            explicit Bounded(unsigned i, GLenum t) :index(i), target(t) {}
            ~Bounded() {
                if (index >= 0) {
                    glBindBuffer(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = -1;
            }
            Bounded& glBufferData(GLsizeiptr size, const void* data, GLenum usage) {
                ::glBufferData(target, size, data, usage);
                return *this;;

            }
            template <typename Buffer>
            Bounded& glBufferData(Buffer buff, GLenum usage) {

                ::glBufferData(target, buff.size() * sizeof(Buffer::value_type), buff.data(), usage);
                return *this;

            }
            template<typename Func>
            void execute(Func func)const {
                func();
            }
            template<typename Func>
            void operator()(Func func) {
                execute(std::move(func));
            }
        };
        std::array<GLuint, SIZE> vbos;
        GLenum target;
        VBOS(GLenum t) :target(t) {
            glGenBuffers(vbos.size(), vbos.data());
        }
        ~VBOS() {
            glDeleteBuffers(vbos.size(), vbos.data());
        }
        GLuint& operator[](int index) { assert(index >= 0 && index < vbos.size()); return vbos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vbos.size());
            glBindBuffer(target, vbos[index]);
            return Bounded{ index ,target };
        }

    };
    template<size_t SIZE>
    struct VAOS {
        struct Bounded {
            unsigned index{ -1 };
            explicit Bounded(unsigned i) :index(i) {}
            ~Bounded() {
                if (index >= 0) {
                    glBindVertexArray(0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = -1;
            }

            Bounded& glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, int  stridec, int offset)
            {
                auto get_stride_offset = [&]()->std::pair<int, void*> {

                    switch (type) {
                    case GL_BYTE:
                        return std::pair<int, void*>(stridec * sizeof(bool), (void*)(offset * sizeof(bool)));
                        break;
                    case GL_SHORT:
                        return std::pair<int, void*>{ stridec * sizeof(short), (void*)(offset * sizeof(short)) };
                        break;
                    case GL_UNSIGNED_SHORT:
                        return std::pair<int, void*>{ stridec * sizeof(unsigned short), (void*)(offset * sizeof(unsigned short)) };
                        break;
                    case GL_INT:
                        return std::pair<int, void*>{ stridec * sizeof(int), (void*)(offset * sizeof(int)) };
                        break;
                    case GL_FLOAT:
                        return std::pair<int, void*>{ stridec * sizeof(float), (void*)(offset * sizeof(float)) };
                        break;
                    }
                    return std::pair<int, void*>(stridec * sizeof(bool), (void*)(offset * sizeof(bool)));
                }();

                ::glVertexAttribPointer(index, size, type, normalized, get_stride_offset.first, get_stride_offset.second);
                glEnableVertexAttribArray(index);
                return *this;
            }
            template<typename Func>
            void execute(Func func)const {
                func();
            }
            template<typename Func>
            void operator()(Func func) {
                execute(std::move(func));
            }

        };
        std::array<GLuint, SIZE> vaos;
        VAOS() {
            glGenVertexArrays(vaos.size(), vaos.data());
        }
        ~VAOS() {
            glDeleteVertexArrays(vaos.size(), vaos.data());
        }
        GLuint& operator[](int index) { assert(index >= 0 && index < vaos.size()); return vaos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vaos.size());
            glBindVertexArray(vaos[index]);
            return Bounded{ index };
        }


    };

    template<size_t SIZE>
    struct VTOS {
        struct Bounded {
            unsigned index{ -1 };
            GLenum target;
            explicit Bounded(unsigned i, GLenum t) :index(i), target(t) {}
            ~Bounded() {
                if (index >= 0) {
                    glBindTexture(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = -1;
            }
            bool glTexImage2D(GLint level,const char* imagePath) {
                int width=0,height=0,nrChannels = 0;
                stbi_set_flip_vertically_on_load(true);
                unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
                if (data) {
                    ::glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                    return true;
                }
                return false;
            }
            bool glTexImage2D(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) {
                ::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
                glGenerateMipmap(target);
                return true;
            }
            void glTexParameteri(GLenum p1, GLenum p2) {
                ::glTexParameteri(target, p1, p2);
            }
            
        };
        std::array<GLuint, SIZE> vtos;
        GLenum target;
        VTOS(GLenum t) :target(t) {
            glGenTextures(vtos.size(), vtos.data());
        }
        ~VTOS() {
            glDeleteTextures(vtos.size(), vtos.data());
        }
        GLuint& operator[](int index) { assert(index >= 0 && index < vtos.size()); return vtos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vtos.size());
            glBindTexture(target, vtos[index]);
            return Bounded{ index ,target };
        }

    };
   

}
