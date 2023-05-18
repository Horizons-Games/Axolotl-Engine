template<typename T>
struct VectorField
{
public:
    std::string name;
    std::function<std::vector<T>(void)> getter;
    std::function<void(const std::vector<T>&)> setter;
    FieldType type;

    VectorField(const std::string& name,
        const std::function<std::vector<T>(void)>& getter,
        const std::function<void(const std::vector<T>&)>& setter) :
        name(name),
        getter(getter),
        setter(setter)
    {
        type = GetInnerType();
        if (!name.empty())
        {
            this->name.front() = std::toupper(this->name.front());
        }
    }

    ~VectorField() = default;

private:
    FieldType GetInnerType()
    {
        try
        {
            std::vector<T> test;
            return TypeToEnum<T>::value;
        }
        catch (const std::bad_any_cast&)
        {
            return FieldType::UNKNOWN;
        }
    }
};
