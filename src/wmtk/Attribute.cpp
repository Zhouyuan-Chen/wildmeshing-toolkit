#include "Attribute.hpp"
#include <wmtk/io/MeshWriter.hpp>
#include <wmtk/utils/Rational.hpp>

namespace wmtk {

template <typename T>
void Attribute<T>::serialize(const std::string& name, const int dim, MeshWriter& writer) const
{
    writer.write(name, dim, stride(), m_data);
}

template <typename T>
Attribute<T>::Attribute(long stride, long size)
    : m_stride(stride)
{
    if (size > 0) {
        m_data = std::vector<T>(size * stride, T(0));
    }
}


template <typename T>
bool Attribute<T>::operator==(const Attribute<T>& o) const
{
    return m_stride == o.m_stride && m_data == o.m_data;
}

template <typename T>
void Attribute<T>::reserve(const long size)
{
    if (size > m_data.size()) {
        m_data.resize(m_stride * size, T(0));
    }
}
template <typename T>
long Attribute<T>::size() const
{
    return m_data.size() / m_stride;
}
template <typename T>
long Attribute<T>::stride() const
{
    return m_stride;
}

template <typename T>
void Attribute<T>::set(std::vector<T> val)
{
    assert(val.size() % m_stride == 0);
    m_data = std::move(val);
}
template <typename T>
auto Attribute<T>::const_vector_attribute(const long index) const -> ConstMapResult
{
    const long start = index * m_stride;
    return ConstMapResult(m_data.data() + start, m_stride);
}


template <typename T>
typename Attribute<T>::MapResult Attribute<T>::vector_attribute(const long index)
{
    const long start = index * m_stride;
    auto buffer = MapResult(m_data.data() + start, m_stride);
    return buffer;
}

template <typename T>
T Attribute<T>::const_scalar_attribute(const long index) const
{
    assert(m_stride == 1);
    return m_data[index];
}

template <typename T>
T& Attribute<T>::scalar_attribute(const long index)
{
    assert(m_stride == 1);
    return m_data[index];
}

template class Attribute<char>;
template class Attribute<long>;
template class Attribute<double>;
template class Attribute<Rational>;
} // namespace wmtk
