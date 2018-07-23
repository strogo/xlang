
namespace xlang::meta::reader
{
    template <typename T>
    template <typename Row>
    Row index_base<T>::get_row() const
    {
        WINRT_ASSERT(type() == (index_tag_v<T, Row>));
        return get_database().get_table<Row>()[index()];
    }

    inline auto typed_index<TypeDefOrRef>::TypeDef() const
    {
        return get_row<reader::TypeDef>();
    }

    inline auto typed_index<TypeDefOrRef>::TypeRef() const
    {
        return get_row<reader::TypeRef>();
    }

    inline auto typed_index<TypeDefOrRef>::TypeSpec() const
    {
        return get_row<reader::TypeSpec>();
    }

    inline auto typed_index<HasConstant>::Field() const
    {
        return get_row<reader::Field>();
    }

    inline auto typed_index<HasConstant>::Param() const
    {
        return get_row<reader::Param>();
    }

    inline auto typed_index<HasConstant>::Property() const
    {
        return get_row<reader::Property>();
    }

    inline auto typed_index<CustomAttributeType>::MemberRef() const
    {
        return get_row<reader::MemberRef>();
    }

    inline auto typed_index<CustomAttributeType>::MethodDef() const
    {
        return get_row<reader::MethodDef>();
    }

    inline auto typed_index<MemberRefParent>::TypeRef() const
    {
        return get_row<reader::TypeRef>();
    }

    inline bool TypeDef::has_attribute(std::string_view const& type_namespace, std::string_view const& type_name) const
    {
        for (auto&& attribute : CustomAttribute())
        {
            if (attribute.Type().type() == CustomAttributeType::MemberRef)
            {
                auto type = attribute.Type().MemberRef().Class().TypeRef();

                if (type_name == type.TypeName() && type_namespace == type.TypeNamespace())
                {
                    return true;
                }
            }
            else
            {
                WINRT_ASSERT(attribute.Type().type() == CustomAttributeType::MethodDef);
                // TODO: Resolve method parent (TypeDef)
            }
        }

        return false;
    }

    inline bool TypeDef::is_enum() const
    {
        auto base = Extends().TypeRef();
        return base.TypeNamespace() == "System" && base.TypeName() == "Enum";
    }

    struct EnumDefinition
    {
        explicit EnumDefinition(TypeDef const& type)
            : m_typedef(type)
        {
            WINRT_ASSERT(type.is_enum());
            for (auto field : type.FieldList())
            {
                if (!field.is_literal() && !field.is_static())
                {
                    WINRT_ASSERT(m_underlying_type == ElementType::End);
                    m_underlying_type = std::get<ElementType>(field.Signature().Type().Type());
                    WINRT_ASSERT(ElementType::Boolean <= m_underlying_type && m_underlying_type <= ElementType::U8);
                }
            }
        }
        TypeDef m_typedef;
        ElementType m_underlying_type{};
    };

    inline auto TypeDef::get_enum_definition() const
    {
        return EnumDefinition{ *this };
    }
}
