#pragma once
/*
MIT License

Copyright (c) 2021 abhilashraju

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "glad/glad.h"

#include <iostream>
#include <string>
#include <array>
#include <optional>
#include <assert.h>
#include <sstream>
#include "gerror.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "lighting.hpp"

namespace gl {
    // template vtable for generic unitoform function
    template<typename T>struct glUniform;
    template<>
    struct glUniform<GLfloat> {
        static const auto args1() { return glUniform1f; }
        static const auto args2() { return glUniform2f; }
        static const auto args3() { return glUniform3f; }
        static const auto args4() { return glUniform4f; }
        static const auto args4v() { return glUniform4fv; }
        static const auto args3v() { return glUniform3fv; }
        static const auto args2v() { return glUniform2fv; }
        static const auto Matrix4v() { return glUniformMatrix4fv; }
        static const auto Matrix3v() { return glUniformMatrix3fv; }
        static const auto Matrix2v() { return glUniformMatrix2fv; }
        
    };
    template<>
    struct glUniform<GLint> {
        static const auto args1() { return glUniform1i; }
        static const auto args2() { return glUniform2i; }
        static const auto args3() { return glUniform3i; }
        static const auto args4() { return glUniform4i; }
        static const auto args4v() { return glUniform4iv; }
        static const auto args3v() { return glUniform3iv; }
        static const auto args2v() { return glUniform2iv; }
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
            std::string datastr = replace_imports(data);
            const char* rdata = datastr.c_str();
            glShaderSource(*shader, 1, &rdata, NULL);
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
            std::string datastr = replace_imports(str.str().c_str());
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
        template<typename... Args>
        bool check_all(const Args&... args) {
            return  ( *args &&...);
        }
        template<typename... Args>
        GProgramme(const Args&... args) {
            if (check_all(args...)) {
                pgm = glCreateProgram();
                (..., glAttachShader(*pgm, *args));
                //glAttachShader(*pgm, *f);
                glLinkProgram(*pgm);
                int success{};

                glGetProgramiv(*pgm, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(*pgm, log.size(), NULL, log.data());
                    pgm = std::nullopt;
                }
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
        template<typename Type>
        void setUniform4v(const std::string& name,GLint count, const Type* v)
        {
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::args4v()(vertexColorLocation, count, v);
        }
        template<typename Type>
        void setUniform3v(const std::string& name, GLint count, const Type* v)
        {
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::args3v()(vertexColorLocation, count, v);
        }
        template<typename Type>
        void setUniform2v(const std::string& name, GLint count, const Type* v)
        {
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::args2v()(vertexColorLocation, count, v);
        }
        void setUniformv(const std::string& name, const glm::vec3& v) {
            setUniform3v(name, 1, glm::value_ptr(v));
        }
        void setUniformv(const std::string& name, const glm::vec2& v) {
            setUniform3v(name, 1, glm::value_ptr(v));
        }
        template<typename Type>
        void setUniformMatrix4(const std::string& name,GLuint count , GLboolean trasnpose, const Type* m){
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::Matrix4v()(vertexColorLocation,count,trasnpose,m);
        }
        template<typename Type>
        void setUniformMatrix3(const std::string& name, GLuint count, GLboolean trasnpose, const Type* m) {
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::Matrix3v()(vertexColorLocation, count, trasnpose, m);
        }
        template<typename Type>
        void setUniformMatrix2(const std::string& name, GLuint count, GLboolean trasnpose, const Type* m) {
            int vertexColorLocation = glGetUniformLocation(*pgm, name.c_str());
            glUniform< Type>::Matrix2v()(vertexColorLocation, count, trasnpose, m);
        }
        void setUnformMatrix(const std::string& name, const glm::mat2& m) {
            setUniformMatrix2(name, 1, GL_FALSE,glm::value_ptr(m));
        }
        void setUnformMatrix(const std::string& name, const glm::mat3& m) {
            setUniformMatrix3(name, 1, GL_FALSE, glm::value_ptr(m));
        }
        void setUnformMatrix(const std::string& name, const glm::mat4& m) {
            setUniformMatrix4(name, 1, GL_FALSE, glm::value_ptr(m));
        }
        void setUniformBlockBinding(const std::string& name, GLuint pos) {

            unsigned int index = ::glGetUniformBlockIndex(*pgm, name.c_str());
            ::glUniformBlockBinding(*pgm, index, pos);
        }
    };

    auto make_shader(const char* data, GLuint type) {
        return [=](auto on_failure) {
            GShader shader(data, type);
            if (!shader) {
                on_failure(std::error_code{ GerrorCode::ShaderError },shader.log.data());
            }
            return std::move(shader);
        };
    }
    auto make_shader(const std::istream& stream, GLuint type) {
        return [&stream,type=type](auto on_failure) {
            GShader shader(stream, type);
            if (!shader) {
                on_failure(std::error_code{ GerrorCode::ShaderError },shader.log.data());
            }
            return std::move(shader);
        };
    }
    template<typename... Args>
    auto make_programme(Args&&... args) {
        return[args...](auto on_failure) {
           
            GProgramme pgm;
            
            pgm = std::move(GProgramme(args(on_failure)...));
            
            if (!pgm) {
                on_failure(std::error_code{ GerrorCode::ProgrammeError }, pgm.log.data());
            }
            return pgm;
        };
    }
    template<template<size_t> typename BO>
    struct SingularBO
    {
        
        BO<1> object;
        SingularBO() {}
        template<typename... Args>
        SingularBO(Args&&... args):object(std::forward<Args>(args)...){}
        SingularBO(const SingularBO&) = delete;
        SingularBO(SingularBO&&) = default;
        SingularBO& operator=(const SingularBO&) = delete;
        [[nodiscard]] auto bind() {
           return object.bind(0);
        }
        auto& operator*() {
            return object[0];
        }
    };
    template<size_t SIZE>
    struct VBOS {
        struct Bounded {
            using BUFFER_TYPE = float;
            GLuint index{ std::numeric_limits<GLuint>::max() };
            GLenum target;
            mutable GLsizeiptr size{ 0 };
            explicit Bounded(unsigned i, GLenum t) :index(i), target(t) {}
            ~Bounded() {
                if (index != std::numeric_limits<GLuint>::max() ) {
                    glBindBuffer(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = std::numeric_limits<GLuint>::max() ;
            }
            const Bounded& glBufferData(GLsizeiptr s, const void* data, GLenum usage)const {
                ::glBufferData(target, s, data, usage);
                size = s;
                return *this;;

            }
            template <typename Buffer>
            const Bounded& glBufferData(Buffer buff, GLenum usage)const {

                ::glBufferData(target, buff.size() * sizeof(Buffer::value_type), buff.data(), usage);
                return *this;

            }
            const Bounded& glBufferSubData(GLuint offset,GLuint size, const void* data)const {
                ::glBufferSubData(target, offset,size, data);
                return *this;;

            }
            template <typename Buffer>
            const Bounded& glBufferSubData(GLuint offset,Buffer buff)const {

                ::glBufferSubData(target, offset,buff.size() * sizeof(Buffer::value_type), buff.data());
                return *this;

            }
            class BufferView {
                friend class VBOS<SIZE>;
                BufferView(GLenum target, BUFFER_TYPE* base, GLuint size):target(target),base(base),size(size)
                {

                }
            public:
                using value_type = BUFFER_TYPE;
                ~BufferView() {
                    if (base)
                        ::glUnmapBuffer(target);
                }
                BufferView(const BufferView&) = delete;
                BufferView& operator = (const BufferView&) = delete;
                BufferView(BufferView&& o) {
                    std::swap(target ,o.target);
                    std::swap(size,o.size);
                    std::swap(base,o.base);
                }
                GLenum target{ GL_ARRAY_BUFFER };
                BUFFER_TYPE* base{ nullptr };
                size_t size{ 0 };
                size_t current{ 0 };
                using iterator = BUFFER_TYPE*;
                using const_iteraor=const BUFFER_TYPE*;
                iterator begin() { assert(base);  return base; }
                iterator end() { return base+size; }
                const_iteraor begin() const{ assert(base);  return base; }
                const_iteraor end() const { return base + size; }
                void advance(size_t index) {  current = (index > size) ? size : index; }
                void push_back(BUFFER_TYPE v) {
                    *(base + current) = v; current++;
                }
            };
            template<typename Stream>
            void loadFromStream(Stream& s){
                int count;
                s>>count;
                glBufferData(count * sizeof(float),nullptr,GL_STATIC_DRAW);
                auto view = mapWriteBuffer();  
                std::copy(std::istream_iterator<float>(s), std::istream_iterator<float>(), std::back_inserter(view));
            }
            BUFFER_TYPE* glMapBuffer(GLenum mode)const{
                return static_cast<BUFFER_TYPE*>(::glMapBuffer(target, mode));
            }
            auto glUnmapBuffer()const{
                return ::glUnmapBuffer(target);
            }
            const BufferView mapReadBuffer()const {
                if (size <= 0) {
                    return BufferView(target, nullptr, 0);
                }
                return BufferView(target, glMapBuffer(GL_READ_ONLY), size);
            }
            BufferView mapWriteBuffer()const {
                if (size <= 0) {
                    return BufferView(target, nullptr, 0);
                }
                return BufferView(target, glMapBuffer(GL_WRITE_ONLY), size);
            }
            auto glBindBufferBase(GLuint pos)const {
                return ::glBindBufferBase(target, pos, index);
            }
            auto glBindBufferRange(GLuint pos, GLuint offset, GLuint size)const {
                return ::glBindBufferRange(target, pos, index, offset, size);
            }
            template<typename Func>
            void execute(Func func)const {
                func(*this);
            }
            template<typename Func>
            void operator()(Func func)const {
                execute(std::move(func));
            }
        };
        std::array<GLuint, SIZE> vbos;
        GLenum target;
        bool cleared{ false };
        VBOS(GLenum t) :target(t) {
            glGenBuffers(vbos.size(), vbos.data());
        }
        ~VBOS() {
            if (!cleared) {
                glDeleteBuffers(vbos.size(), vbos.data());
            }
            
        }
        VBOS(VBOS&& o) {
            vbos = std::move(o.vbos);
            target = o.target;
            o.cleared = true;
        }
        VBOS(const VBOS&) = delete;
        VBOS& operator=(const VBOS&) = delete;
        GLuint& operator[](int index) { assert(index >= 0 && index < vbos.size()); return vbos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vbos.size());
            glBindBuffer(target, vbos[index]);
            return Bounded{ vbos[index] ,target };
        }

    };
    using VBO=SingularBO<VBOS>;
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

            const Bounded& glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, int  stridec, int offset)const
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
                glEnableVertexAttribArray(index);
                ::glVertexAttribPointer(index, size, type, normalized, get_stride_offset.first, get_stride_offset.second);
                
                return *this;
            }
            template<typename Func>
            void execute(Func func)const {
                func(*this);
            }
            template<typename Func>
            void operator()(Func func)const {
                execute(std::move(func));
            }

        };
        std::array<GLuint, SIZE> vaos;
        bool cleared{ false };
        VAOS() {
            glGenVertexArrays(vaos.size(), vaos.data());
        }
      
        ~VAOS() {
            if (!cleared) {
                glDeleteVertexArrays(vaos.size(), vaos.data());
            }
        }
        VAOS(VAOS&& o) {
            vaos = std::move(o.vaos);
            o.cleared = true;
        }
        VAOS(const VAOS&) = delete;
        VAOS& operator=(const VAOS&) = delete;
        GLuint& operator[](int index) { assert(index >= 0 && index < vaos.size()); return vaos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vaos.size());
            glBindVertexArray(vaos[index]);
            return Bounded{ index };
        }


    };
    using VAO=SingularBO<VAOS>;
    template<size_t SIZE>
    struct VTOS {
        struct Bounded {
            unsigned index{ -1 };
            GLenum target;
            mutable GLenum textureenum;
            explicit Bounded(unsigned i, GLenum t, GLenum te) :index(i), target(t), textureenum(te){
                glTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            ~Bounded() {
                if (index >= 0) {
                    glBindTexture(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                target = o.target;
                textureenum = o.textureenum;
                o.index = -1;
            }
            void next()const{
                textureenum +=1;
            }
            template<typename Func>
            void execute(Func func)const {
                func(*this);
            }
            template<typename Func>
            void operator()(Func func)const {
                execute(std::move(func));
            }
            const Bounded& glTexImage2D(GLint level,GLenum type,const std::string& imagePath)const {
                int width=0,height=0,nrChannels = 0;
               
                if(!imagePath.empty()){
                    
                    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
                    if (data) {
                        GLenum format;
                        if (nrChannels == 1)
                            format = GL_RED;
                        else if (nrChannels == 3)
                            format = GL_RGB;
                        else if (nrChannels == 4)
                            format = GL_RGBA;
                        glTexImage2D(level, format, width, height, 0, format, type, data);
                        stbi_image_free(data);  
                    }
                }
                return *this;
            }
            const Bounded& glTexImage2D(GLint level,GLint internalformat,GLenum format,GLenum type,const char* imagePath)const {
                int width=0,height=0,nrChannels = 0;
               
                if(imagePath){
                    
                    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
                    if (data) {
                        glTexImage2D( level, internalformat, width, height, 0, format, type, data);
                        stbi_image_free(data);  
                        return *this;
                    }
                }
                ::glTexImage2D(target, level, internalformat, width, height, 0, format, type, nullptr);
                return *this;
            }
            const Bounded& glTexImage2D(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)const {
                ::glTexImage2D(textureenum, level, internalformat, width, height, border, format, type, pixels);
                glGenerateMipmap(target);
                return *this;
            }
            const Bounded& glTexParameteri(GLenum p1, GLenum p2)const {
                ::glTexParameteri(target, p1, p2);
                return *this;
            }
            
            
        };

        std::vector<GLuint> vtos;
        
        GLenum target;
        bool cleared{ false };
        VTOS(GLenum t) :target(t) {
            vtos.resize(SIZE);
            glGenTextures(vtos.size(), vtos.data());
        }
        ~VTOS() {
            if(!cleared)
                glDeleteTextures(vtos.size(), vtos.data());
        }
        VTOS(VTOS&& o) {
            vtos = std::move(o.vtos);
            target = o.target;
            o.cleared = true;
        }
        VTOS(const VTOS&) = delete;
        VTOS& operator=(const VTOS&) = delete;
        void push_back(GLuint i = GLuint{}) {
            vtos.push_back(i);
            glGenTextures(1, vtos.data()+vtos.size()-1);
        }
        GLuint& operator[](int index) { assert(index >= 0 && index < vtos.size()); return vtos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vtos.size());
            glBindTexture(target, vtos[index]);
            return Bounded{ index ,target,(target==GL_TEXTURE_CUBE_MAP)?GL_TEXTURE_CUBE_MAP_POSITIVE_X:target};
        }
        [[nodiscard]]  Bounded glActiveTexture(unsigned index){
            
            ::glActiveTexture(GL_TEXTURE0+index);
           return  bind(index);
        }

    };
    struct VTO:SingularBO<VTOS>{
        [[nodiscard]]  auto glActiveTexture(unsigned index){
           return object.glActiveTexture(index);
        }
        static VTO fromCubemap(std::vector<std::string> faces)
        {
            VTO vto(GL_TEXTURE_CUBE_MAP);
            vto.bind().execute([&](auto& to) {
                for (unsigned int i = 0; i < faces.size(); i++)
                {
                    to.glTexImage2D(0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, faces[i].c_str());
                    to.next();
                }
                to.glTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                to.glTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                to.glTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                to.glTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                to.glTexParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                });
            return vto;
        }
        VTO(GLenum t) :SingularBO<VTOS>(t) {}
    };
    
    template<size_t SIZE>
    struct RBOS {
        struct Bounded {
            unsigned index{ -1 };
            GLenum target{ GL_FRAMEBUFFER };
            explicit Bounded(unsigned i, GLenum t) :index(i), target(t) {}
            ~Bounded() {
                if (index >= 0) {
                    glBindRenderbuffer(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = -1;
            }
            const Bounded& glRenderbufferStorage(GLenum internalformat, GLsizei width, GLsizei height)const {
                ::glRenderbufferStorage(target, internalformat, width, height);
                return *this;;
            }

            const Bounded& glFramebufferRenderbuffer(GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)const {
                ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
                return *this;
            }
            template<typename Func>
            void execute(Func func)const {
                func(*this);
            }
            template<typename Func>
            void operator()(Func func)const {
                execute(std::move(func));
            }
        };
        std::array<GLuint, SIZE> rbos;

        bool cleared{ false };
        RBOS() {
            glGenRenderbuffers(rbos.size(), rbos.data());
        }
        ~RBOS() {
            if (!cleared) {
                glDeleteBuffers(rbos.size(), rbos.data());
            }

        }
        RBOS(RBOS&& o) {
            rbos = std::move(o.rbos);
            o.cleared = true;
        }
        RBOS(const RBOS&) = delete;
        RBOS& operator=(const RBOS&) = delete;
        GLuint& operator[](int index) { assert(index >= 0 && index < rbos.size()); return rbos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < rbos.size());
            glBindRenderbuffer(GL_RENDERBUFFER, rbos[index]);
            return Bounded{ index ,GL_RENDERBUFFER };
        }

    };
    using RBO=SingularBO<RBOS>;
    template<size_t SIZE>
    struct FBOS {
        struct Bounded {
            unsigned index{ -1 };
            GLenum target{ GL_FRAMEBUFFER };
            explicit Bounded(unsigned i, GLenum t) :index(i), target(t) {}
            ~Bounded() {
                if (index >= 0) {
                    glBindFramebuffer(target, 0);
                }
            }
            Bounded(const Bounded&) = delete;
            Bounded(Bounded&& o) {
                index = o.index;
                o.index = -1;
            }
            const Bounded& glFramebufferTexture2D(GLenum attachment, GLenum textarget, GLuint texture, GLint level)const {
                ::glFramebufferTexture2D(target, attachment, textarget, texture, level);
                return *this;;
            }

            const Bounded& glFramebufferRenderbuffer(GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)const {
                ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
                return *this;
            }
            bool glCheckFramebufferStatus()const {
                return ::glCheckFramebufferStatus(target) == GL_FRAMEBUFFER_COMPLETE;
            }
            template<typename Func>
            void execute(Func func)const {
                func(*this);
            }
            template<typename Func>
            void operator()(Func func)const {
                execute(std::move(func));
            }
        };
        std::array<GLuint, SIZE> vfos;
        VTOS<SIZE> vtos{ GL_TEXTURE_2D };
        RBOS<SIZE> rbos;

        bool cleared{ false };
        FBOS() {
            glGenFramebuffers(vfos.size(), vfos.data());
        }
        ~FBOS() {
            if (!cleared) {
                glDeleteBuffers(vfos.size(), vfos.data());
            }

        }
        FBOS(FBOS&& o) {
            vfos = std::move(o.vfos);
            o.cleared = true;
        }
        FBOS(const FBOS&) = delete;
        FBOS& operator=(const FBOS&) = delete;
        GLuint& operator[](int index) { assert(index >= 0 && index < vfos.size()); return vfos[index]; }
        [[nodiscard]] Bounded bind(unsigned index) {
            assert(index >= 0 && index < vfos.size());
            glBindFramebuffer(GL_FRAMEBUFFER, vfos[index]);
            return Bounded{ index ,GL_FRAMEBUFFER };
        }

        std::error_code prepare(int index, GLuint wid, GLuint ht) {
            std::error_code ec= GerrorCode::Success;
               bind(0).execute([&](auto& fb) {
                auto tb = vtos.bind(0);
                tb.glTexImage2D(0, GL_RGB, wid, ht, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                fb.glFramebufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vtos[0], 0);
                auto rb = rbos.bind(0);
                rb.glRenderbufferStorage(GL_DEPTH24_STENCIL8, wid, ht);
                fb.glFramebufferRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbos[0]);
                if(!fb.glCheckFramebufferStatus())
                    ec=GerrorCode::FboCompletionError;
               
            });
            return ec;
        }
        GLuint texture(GLuint index) {
            return vtos[index];
        }

    };
    
    struct FBO :SingularBO<FBOS>
    {
        GLuint texture() {
            return object.texture(0);
        }
        auto prepare(GLuint wid, GLuint ht) {
            return object.prepare(0, wid, ht);
        }

    };
}
