#pragma once
# include <system_error>
namespace gl {
    enum class GerrorCode
    {
        // no 0
        Success = 0,
        ShaderError = 10,
        ProgrammeError,
        BufferBindError,
        FboCompletionError,
    };
    enum class GerrorSource
    {
        ShaderCompilation = 10,
        VertexBuffer,
        FrameBuffer,
    };
}
namespace std
{
    template <>
    struct is_error_code_enum<gl::GerrorCode> : true_type {};

    template <>
    struct is_error_condition_enum<gl::GerrorSource> : true_type {};

}
namespace gl {
    struct g_error_category :std::error_category
    {

        const char* name()const noexcept override { return "Gl Error"; }
        std::string message(int ev)const override {
            switch (static_cast<GerrorCode>(ev))
            {
            case GerrorCode::ShaderError: {
                return "error while compiling vertex shader";
            }
                                        break;
            case GerrorCode::ProgrammeError: {
                return "error while compiling programme linking";
            }
            case GerrorCode::FboCompletionError: {
                return "Framebuffer is not complete!";
            }
                                           
            default:
                break;
            }
            return "Ubknown error";
        }

    };




    struct g_error_source_category :std::error_category
    {

        const char* name()const noexcept override { return "Gl Error Source"; }
        std::string message(int ev)const override {
            switch (static_cast<GerrorSource>(ev))
            {
            case GerrorSource::ShaderCompilation: {
                return "error in compilation and linking of shaders";
            }
                                                break;
            case GerrorSource::VertexBuffer: {
                return "error in buffer allocation and attachment";
            }

            default:
                break;
            }
            return "Unknown error source";
        }
        bool equivalent(const std::error_code& code, int condition) const noexcept override
        {
            switch (static_cast<GerrorSource>(condition))
            {
            case GerrorSource::ShaderCompilation:
                return code == GerrorCode::ShaderError || code == GerrorCode::ProgrammeError;
                break;
            case GerrorSource::VertexBuffer:
                return code == GerrorCode::BufferBindError;
                break;
            case GerrorSource::FrameBuffer:
                return code == GerrorCode::BufferBindError;
                break;
            default:
                break;
            }
            return false;
        }

    };

    inline std::error_condition make_error_condition(gl::GerrorSource e)
    {
        static const gl::g_error_source_category theErrorCondition{};
        return { static_cast<int>(e), theErrorCondition };
    }
    inline std::error_code make_error_code(gl::GerrorCode e)
    {
        static const gl::g_error_category theError{};
        return { static_cast<int>(e), theError };
    }
}

