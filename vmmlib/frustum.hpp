/*
 * Copyright (c) 2006-2016, Visualization and Multimedia Lab,
 *                          University of Zurich <http://vmml.ifi.uzh.ch>,
 *                          Eyescale Software GmbH,
 *                          Blue Brain Project, EPFL
 *
 * This file is part of VMMLib <https://github.com/VMML/vmmlib/>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.  Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution.  Neither the name of the Visualization and Multimedia
 * Lab, University of Zurich nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VMML__FRUSTUM__HPP__
#define __VMML__FRUSTUM__HPP__

#include <cstring>           // memcmp
#include <vmmlib/matrix.hpp> // used inline

#if defined(_MSC_VER)
 //can otherwise cause problems with, e.g., M_PI under Windows.
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#include <corecrt_math_defines.h>
#endif
#else
#ifndef M_PI
#define M_PI 3.1415926535897932
#endif
#endif

namespace vmml
{
/** Represents a frustum, following OpenGL conventions. */
template <typename T>
class Frustum
{
public:
    /** Construct a default frustum (-1, 1, -1, 1, 0.1, 100). */
    Frustum();

    /** Construct a frustum with default values */
    Frustum(T left, T right, T bottom, T top, T nearPlane, T farPlane);

    /** Construct a frustum using gluPerspective semantics */
    Frustum(T field_of_view_y, T aspect_ratio, T nearPlane_, T farPlane);

    /** Construct a frustum from a projection matrix */
    Frustum(const Matrix<4, 4, T>& projection);

    /** Destruct this frustum */
    ~Frustum() {}
    /** @return true if this and the other frustum are identical */
    bool operator==(const Frustum<T>& other) const;

    /** @return true if this and the other frustum are not identical */
    bool operator!=(const Frustum<T>& other) const;

    /** @return true if thw two frusta are identical withing the tolerance */
    bool equals(const Frustum<T>& other,
                T tolerance = std::numeric_limits<T>::epsilon()) const;

    /** @return the perspective matrix for the given frustum. */
    Matrix<4, 4, T> computePerspectiveMatrix() const;

    /** @return the orthographic matrix for the given frustum. */
    Matrix<4, 4, T> computeOrthoMatrix() const;

    /** Move the frustum near plane by the given offset "sideways" */
    void jitter(const vector<2, T>& jitter_);

    /**
     * Move the frustum near plane.
     *
     * Changes the position of the nearPlane, adjusting the other parameters in
     * a way that the shape of the perspective pyramid stays the same.
     */
    void adjustNearPlane(const T nearPlane);

    /** @name Access to frustum corners */
    //@{
    T& left() { return _array[0]; }
    T left() const { return _array[0]; }
    T& right() { return _array[1]; }
    T right() const { return _array[1]; }
    T& bottom() { return _array[2]; }
    T bottom() const { return _array[2]; }
    T& top() { return _array[3]; }
    T top() const { return _array[3]; }
    T& nearPlane() { return _array[4]; }
    T nearPlane() const { return _array[4]; }
    T& farPlane() { return _array[5]; }
    T farPlane() const { return _array[5]; }
    //@}

    /** @return the width of this frustum at the near plane */
    T getWidth() const;

    /** @return the height of this frustum at the near plane */
    T getHeight() const;

    friend std::ostream& operator<<(std::ostream& os, const Frustum& f)
    {
        const std::ios::fmtflags flags = os.flags();
        const int prec = os.precision();

        os.setf(std::ios::right, std::ios::adjustfield);
        os.precision(5);
        os << "[" << std::setw(10) << f.left() << " " << std::setw(10)
           << f.right() << " " << std::setw(10) << f.bottom() << " "
           << std::setw(10) << f.top() << " " << std::setw(10) << f.nearPlane()
           << " " << std::setw(10) << f.farPlane() << "]";
        os.precision(prec);
        os.setf(flags);
        return os;
    };

private:
    T _array[6]; //!< left, right, bottom, top, near, far storage
};
} // namespace vmml

// - implementation - //

namespace vmml
{
template <typename T>
Frustum<T>::Frustum()
{
    _array[0] = -1;
    _array[1] = 1;
    _array[2] = -1;
    _array[3] = 1;
    _array[4] = 0.1f;
    _array[5] = 100;
}

template <typename T>
Frustum<T>::Frustum(const T _left, const T _right, const T _bottom,
                    const T _top, const T _near, const T _far)
{
    _array[0] = _left;
    _array[1] = _right;
    _array[2] = _bottom;
    _array[3] = _top;
    _array[4] = _near;
    _array[5] = _far;
}

template <typename T>
Frustum<T>::Frustum(const T fov_y, const T aspect_ratio, const T nearPlane_,
                    const T farPlane_)
{
    _array[2] = std::tan(0.5 * fov_y * M_PI / 180.0) * nearPlane_;
    _array[3] = -_array[2];
    // depend on _array[2,3]:
    _array[0] = bottom() * aspect_ratio;
    _array[1] = top() * aspect_ratio;
    _array[4] = nearPlane_;
    _array[5] = farPlane_;
}

template <typename T>
Frustum<T>::Frustum(const Matrix<4, 4, T>& projection)
{
    _array[4] = projection(2, 3) / (projection(2, 2) - 1.0);
    _array[5] = projection(2, 3) / (projection(2, 2) + 1.0);
    _array[2] = nearPlane() * (projection(1, 2) - 1.0) / projection(1, 1);
    _array[3] = nearPlane() * (projection(1, 2) + 1.0) / projection(1, 1);
    _array[0] = nearPlane() * (projection(0, 2) - 1.0) / projection(0, 0);
    _array[1] = nearPlane() * (projection(0, 2) + 1.0) / projection(0, 0);
}

template <typename T>
bool Frustum<T>::operator==(const Frustum<T>& other) const
{
    return ::memcmp(_array, other._array, sizeof(_array)) == 0;
}

template <typename T>
bool Frustum<T>::operator!=(const Frustum<T>& other) const
{
    return ::memcmp(_array, other._array, sizeof(_array)) != 0;
}

template <typename T>
bool Frustum<T>::equals(const Frustum<T>& other, const T tolerance) const
{
    return std::abs(_array[0] - other._array[0]) <= tolerance &&
           std::abs(_array[1] - other._array[1]) <= tolerance &&
           std::abs(_array[2] - other._array[2]) <= tolerance &&
           std::abs(_array[3] - other._array[3]) <= tolerance &&
           std::abs(_array[4] - other._array[4]) <= tolerance &&
           std::abs(_array[5] - other._array[5]) <= tolerance;
}

template <typename T>
Matrix<4, 4, T> Frustum<T>::computePerspectiveMatrix() const
{
    Matrix<4, 4, T> M;

    M(0, 0) = 2.0 * nearPlane() / (right() - left());
    M(0, 1) = 0.0;
    M(0, 2) = (right() + left()) / (right() - left());
    M(0, 3) = 0.0;

    M(1, 0) = 0.0;
    M(1, 1) = 2.0 * nearPlane() / (top() - bottom());
    M(1, 2) = (top() + bottom()) / (top() - bottom());
    M(1, 3) = 0.0;

    M(2, 0) = 0.0;
    M(2, 1) = 0.0;
    // NOTE: Some glfrustum man pages say wrongly '(far + near) / (far - near)'
    M(2, 2) = -(farPlane() + nearPlane()) / (farPlane() - nearPlane());
    M(2, 3) = -2.0 * farPlane() * nearPlane() / (farPlane() - nearPlane());

    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = -1.0;
    M(3, 3) = 0.0;

    return M;
}

template <typename T>
Matrix<4, 4, T> Frustum<T>::computeOrthoMatrix() const
{
    Matrix<4, 4, T> M;

    M(0, 0) = 2.0 / (right() - left());
    M(0, 1) = 0.0;
    M(0, 2) = 0.0;
    M(0, 3) = -(right() + left()) / (right() - left());

    M(1, 0) = 0.0;
    M(1, 1) = 2.0 / (top() - bottom());
    M(1, 2) = 0.0f;
    M(1, 3) = -(top() + bottom()) / (top() - bottom());

    M(2, 0) = 0.0;
    M(2, 1) = 0.0;
    M(2, 2) = -2.0 / (farPlane() - nearPlane());
    M(2, 3) = -(farPlane() + nearPlane()) / (farPlane() - nearPlane());

    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0f;

    return M;
}

template <typename T>
void Frustum<T>::jitter(const vector<2, T>& jitter_)
{
    left() = left() + jitter_.x();
    right() = right() + jitter_.x();
    bottom() = bottom() + jitter_.y();
    top() = top() + jitter_.y();
}

template <typename T>
void Frustum<T>::adjustNearPlane(const T new_near)
{
    if (new_near == nearPlane())
        return;

    const T ratio = new_near / nearPlane();
    right() *= ratio;
    left() *= ratio;
    top() *= ratio;
    bottom() *= ratio;
    nearPlane() = new_near;
}

template <typename T>
inline T Frustum<T>::getWidth() const
{
    return std::abs(right() - left());
}

template <typename T>
inline T Frustum<T>::getHeight() const
{
    return std::abs(top() - bottom());
}

} // namespace vmml

#endif
