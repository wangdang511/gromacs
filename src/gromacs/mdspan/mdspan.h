/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright (c) 2018, by the GROMACS development team, led by
 * Mark Abraham, David van der Spoel, Berk Hess, and Erik Lindahl,
 * and including many others, as listed in the AUTHORS file in the
 * top-level source directory and at http://www.gromacs.org.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * http://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at http://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out http://www.gromacs.org.
 */
/*
 * This file is a modified version of original work of Sandia Corporation.
 * In the spirit of the original code, this particular file can be distributed
 * on the terms of Sandia Corporation.
 */
/*
 *                          Kokkos v. 2.0
 *               Copyright (2014) Sandia Corporation
 *
 * Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 * the U.S. Government retains certain rights in this software.
 *
 * Kokkos is licensed under 3-clause BSD terms of use:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Corporation nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Questions? Contact Christian R. Trott (crtrott@sandia.gov)
 */
/*! \libinternal \file
 * \brief Declares gmx::mdspan
 *
 * \author Christian Trott <crtrott@sandia.gov>
 * \author Ronan Keryell <ronan.keryell@xilinx.com>
 * \author Carter Edwards <hedwards@nvidia.com>
 * \author David Hollman <dshollm@sandia.gov>
 * \author Christian Blau <cblau@gwdg.de>
 * \ingroup mdspan
 */
#ifndef MDSPAN_MDSPAN_H
#define MDSPAN_MDSPAN_H

#include <array>
#include <type_traits>

#include "accessor_policy.h"
#include "extents.h"
#include "layouts.h"

namespace gmx
{

/*! \libinternal \brief Multidimensional array indexing and memory access with flexible mapping and access model.
 *
 * \tparam ElementType Type of elemnt to be viewed
 * \tparam Extents The dimensions of the multidimenisonal array to view.
 * \tparam LayoutPolicy Describes is the memory layout of the multidimensional array; right by default.
 * \tparam AccessorPolicy Describes memory access model.
 */
template<class ElementType, class Extents, class LayoutPolicy = layout_right, class AccessorPolicy = accessor_basic<ElementType> >
class basic_mdspan
{
    public:
        //! Expose type used to define the extents of the data.
        using extents_type     = Extents;
        //! Expose type used to define the layout of the data.
        using layout_type      = LayoutPolicy;
        //! Expose type used to define the memory access model of the data.
        using accessor_type    = AccessorPolicy;
        //! Expose type used to map multidimensional indices to one-dimensioal indices.
        using mapping_type     = typename layout_type::template mapping<extents_type>;
        //! Exposes the type of stored element.
        using element_type     = typename accessor_type::element_type;
        //! Expose the underlying type of the stored elements.
        using value_type       = typename std::remove_cv<element_type>::type;
        //! Expose the type used for indexing.
        using index_type       = ptrdiff_t;
        //! Expose type for index differences.
        using difference_type  = ptrdiff_t;
        //! Expose underlying pointer to data type.
        using pointer          = typename accessor_type::pointer;
        //! Expose reference to data type.
        using reference        = typename accessor_type::reference;

        //! Trivial constructor
        constexpr basic_mdspan() noexcept : acc_(), map_(), ptr_() {}
        //! Move constructor
        constexpr basic_mdspan(basic_mdspan &&other) noexcept = default;
        //! copy constructor
        constexpr basic_mdspan(const basic_mdspan &other) noexcept = default;
        //! Copy assignment
        basic_mdspan &operator=(const basic_mdspan &other) noexcept = default;
        //! Move assignment
        basic_mdspan &operator=(basic_mdspan &&other) noexcept = default;

        //! Copy constructor
        template<class OtherElementType,
                 class OtherExtents,
                 class OtherLayoutPolicy,
                 class OtherAccessor>
        constexpr basic_mdspan(
            const basic_mdspan<OtherElementType,
                               OtherExtents,
                               OtherLayoutPolicy,
                               OtherAccessor> &rhs ) noexcept
            : acc_( rhs.acc_ ),
              map_( rhs.map_ ),
              ptr_( rhs.ptr_ )
        {}
        //! Copy assignment constructor
        template<class OtherElementType,
                 class OtherExtents,
                 class OtherLayoutPolicy,
                 class OtherAccessor>
        basic_mdspan &operator= (
            const basic_mdspan<OtherElementType,
                               OtherExtents,
                               OtherLayoutPolicy,
                               OtherAccessor> &rhs ) noexcept
        { acc_ = rhs.acc_; map_ = rhs.map_; ptr_ = rhs.ptr_; return *this; }

        /*!\brief Construct mdspan by setting the dynamic extents and pointer to data.
         * \param[in] ptr Pointer to data to be accessed by this span
         * \param[in] DynamicExtents
         * \tparam IndexType index type to describe dynamic extents
         */
        template<class ... IndexType >
        explicit constexpr basic_mdspan
            ( pointer ptr, IndexType ... DynamicExtents ) noexcept
            : acc_(accessor_type()), map_( extents_type(DynamicExtents ...) ), ptr_(ptr) {}
        /*! \brief Construct from array describing dynamic extents.
         * \param[in] ptr Pointer to data to be accessed by this span
         * \param[in] dynamic_extents Array the size of dynamic extents.
         */
        constexpr basic_mdspan( pointer ptr, const std::array<ptrdiff_t, extents_type::rank_dynamic()> &dynamic_extents)
            : acc_(accessor_type()), map_( extents_type(dynamic_extents)), ptr_(ptr) {}
        /*! \brief Construct from pointer and mapping.
         * \param[in] ptr Pointer to data to be accessed by this span
         * \param[in] m Mapping from multidimenisonal indices to one-dimensional offset.
         */
        constexpr basic_mdspan( pointer ptr, const mapping_type &m ) noexcept
            : acc_(accessor_type()), map_( m ), ptr_(ptr) {}
        /*! \brief Construct with pointer, mapping and accessor.
         * \param[in] ptr Pointer to data to be accessed by this span
         * \param[in] m Mapping from multidimenisonal indices to one-dimensional offset.
         * \param[in] a Accessor implementing memory access model.
         */
        constexpr basic_mdspan( pointer ptr, const mapping_type &m, const accessor_type &a ) noexcept
            : acc_(a), map_( m ), ptr_(ptr) {}

        /*! \brief Brace operator to access multidimenisonal array element.
         * \param[in] indices The multidimensional indices of the object.
         * Requires rank() == sizeof...(IndexType). Slicing is implemented via sub_span.
         * \returns reference to element at indices.
         */
        template<class... IndexType >
        constexpr typename std::enable_if<sizeof ... (IndexType) == extents_type::rank(), reference>::type
        operator()( IndexType... indices) const noexcept
        { return acc_.access( ptr_, map_( indices ... ) ); }
        /*! \brief Canonical bracket operator for one-dimensional arrays.
         * Allows mdspan to act like array in one-dimension.
         * Enabled only when rank==1.
         * \param[in] i one-dimensional index
         * \returns reference to element stored at position i
         */
        template<class IndexType>
        constexpr typename std::enable_if<std::is_integral<IndexType>::value && 1 == extents_type::rank(), reference>::type
        operator[]( const IndexType &i ) const noexcept
        { return acc_.access( ptr_, map_(i) ); }
        //! Report the rank.
        static constexpr int rank() noexcept
        { return extents_type::rank(); }
        //! Report the dynamic rank.
        static constexpr int rank_dynamic() noexcept
        { return extents_type::rank_dynamic(); }
        /*! \brief Return the static extent.
         * \param[in] k dimension to query for static extent
         * \returns static extent along specified dimension
         */
        constexpr index_type static_extent( size_t k ) const noexcept
        { return map_.extents().static_extent( k ); }

        /*! \brief Return the extent.
         * \param[in] k dimension to query for extent
         * \returns extent along specified dimension
         */
        constexpr index_type extent( int k ) const noexcept
        { return map_.extents().extent( k ); }

        //! Return all extents
        constexpr const extents_type &extents() const noexcept
        { return map_.extents(); }
        //! Report if mappings for this basic_span is always unique.
        static constexpr bool is_always_unique()     noexcept { return mapping_type::is_always_unique(); }
        //! Report if mapping for this basic_span is always strided
        static constexpr bool is_always_strided()    noexcept { return mapping_type::is_always_strided(); }
        //! Report if mapping for this basic_span is always is_contiguous
        static constexpr bool is_always_contiguous() noexcept { return mapping_type::is_always_contiguous(); }
        //! Report if the currently applied map is unique
        constexpr bool is_unique() const noexcept  { return map_.is_unique(); }
        //! Report if the currently applied map is strided
        constexpr bool is_strided() const noexcept { return map_.is_strided(); }
        //! Report if the currently applied map is contiguous
        constexpr bool is_contiguous() const noexcept {return map_.is_contiguous(); }
        //! Report stride along a specific rank.
        constexpr index_type stride( size_t r ) const noexcept
        { return map_.stride(r); }
        //! Return the currently applied mapping.
        constexpr mapping_type mapping() const noexcept { return map_; }
        //! Return the memory access model.
        constexpr accessor_type accessor() const noexcept { return acc_; }
        //! Return pointer to underlying data
        constexpr pointer data() const noexcept { return ptr_; }
    private:
        //! The memory access model
        accessor_type acc_;
        //! The transformation from multidimenisonal index to memory offset.
        mapping_type  map_;
        //! Memory location handle
        pointer       ptr_;
};

//! basic_mdspan with wrapped indices, basic_accessor policiy and right-aligned  memory layout.
template<class T, ptrdiff_t... Indices>
using mdspan = basic_mdspan<T, extents<Indices...>, layout_right, accessor_basic<T> >;

}      // namespace gmx

#endif /* end of include guard: MDSPAN_MDSPAN_H */
