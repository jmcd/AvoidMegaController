/*************************************************************************
 *
 * REALM CONFIDENTIAL
 * __________________
 *
 *  [2011] - [2012] Realm Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Realm Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Realm Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Realm Incorporated.
 *
 **************************************************************************/

#ifndef REALM_BPTREE_HPP
#define REALM_BPTREE_HPP

#include <memory> // std::unique_ptr
#include <realm/array.hpp>
#include <realm/impl/destroy_guard.hpp>
#include <realm/impl/output_stream.hpp>

namespace realm {

/// Specialize BpTree to implement column types.
// FIXME: Instead of the Nullable parameter, templates could specialize on an
// "Optional<T>" type instead. It would also remove the need for separate
// is_null/set_null methods.
template<class T, bool Nullable>
class BpTree;

class ArrayInteger;
class ArrayIntNull;

class BpTreeBase {
public:
    struct unattached_tag {};

    // Accessor concept:
    Allocator& get_alloc() const noexcept;
    void destroy() noexcept;
    void detach();
    bool is_attached() const noexcept;
    void set_parent(ArrayParent* parent, size_t ndx_in_parent) noexcept;
    size_t get_ndx_in_parent() const noexcept;
    void set_ndx_in_parent(size_t ndx) noexcept;
    void update_from_parent(size_t old_baseline) noexcept;
    MemRef clone_deep(Allocator& alloc) const;

    // BpTree interface:
    const Array& root() const noexcept;
    Array& root() noexcept;
    bool root_is_leaf() const noexcept;
    void introduce_new_root(ref_type new_sibling_ref, Array::TreeInsertBase& state, bool is_append);
    void replace_root(std::unique_ptr<Array> leaf);

protected:
    explicit BpTreeBase(std::unique_ptr<Array> root);
    explicit BpTreeBase(BpTreeBase&&) = default;
    BpTreeBase& operator=(BpTreeBase&&) = default;
    std::unique_ptr<Array> m_root;

    struct SliceHandler {
        virtual MemRef slice_leaf(MemRef leaf_mem, size_t offset, size_t size,
                                  Allocator& target_alloc) = 0;
        ~SliceHandler() noexcept {}
    };
    static ref_type write_subtree(const Array& root, size_t slice_offset,
                                  size_t slice_size, size_t table_size,
                                  SliceHandler&, _impl::OutputStream&);
    friend class ColumnBase;
    friend class ColumnBaseSimple;
private:
    struct WriteSliceHandler;

    // FIXME: Move B+Tree functionality from Array to this class.
};

template<class T, bool Nullable>
struct GetLeafType;

// Default implementation of BpTree. This should work for all types that have monomorphic
// leaves (i.e. all leaves are of the same type).
template<class T, bool Nullable>
class BpTree : public BpTreeBase {
public:
    using value_type = T;
    using LeafType = typename GetLeafType<T, Nullable>::type;

    /// LeafInfo is used by get_leaf() to provide access to a leaf
    /// without instantiating unnecessary nodes along the way.
    /// Upon return, out_leaf with hold a pointer to the leaf containing
    /// the index given to get_leaf(). If the index happens to be
    /// in the root node (i.e., the root is a leaf), it will point
    /// to the root node.
    /// If the index isn't in the root node, fallback will be initialized
    /// to represent the leaf holding the node, and out_leaf will be set
    /// to point to fallback.
    struct LeafInfo {
        const LeafType** out_leaf;
        LeafType* fallback;
    };

    BpTree();
    explicit BpTree(BpTreeBase::unattached_tag);
    explicit BpTree(Allocator& alloc);
    explicit BpTree(std::unique_ptr<Array> root) : BpTreeBase(std::move(root)) {}
    BpTree(BpTree<T, Nullable>&&) = default;
    BpTree<T, Nullable>& operator=(BpTree<T, Nullable>&&) = default;
    void init_from_ref(Allocator& alloc, ref_type ref);
    void init_from_mem(Allocator& alloc, MemRef mem);
    void init_from_parent();

    size_t size() const noexcept;
    bool is_empty() const noexcept { return size() == 0; }

    T get(size_t ndx) const noexcept;
    bool is_null(size_t ndx) const noexcept;
    void set(size_t, T value);
    void set(size_t, null);
    void set_null(size_t);
    void insert(size_t ndx, T value, size_t num_rows = 1);
    void insert(size_t ndx, null, size_t num_rows = 1);
    void erase(size_t ndx, bool is_last = false);
    void move_last_over(size_t ndx, size_t last_row_ndx);
    void clear();
    T front() const noexcept;
    T back() const noexcept;

    size_t find_first(T value, size_t begin = 0, size_t end = npos) const;
    void find_all(IntegerColumn& out_indices, T value,
                  size_t begin = 0, size_t end = npos) const;

    static MemRef create_leaf(Array::Type, size_t size, T value, Allocator&);

    /// See LeafInfo for information about what to put in the inout_leaf
    /// parameter.
    ///
    /// This function cannot be used for modifying operations as it
    /// does not ensure the presence of an unbroken chain of parent
    /// accessors. For this reason, the identified leaf should always
    /// be accessed through the returned const-qualified reference,
    /// and never directly through the specfied fallback accessor.
    void get_leaf(size_t ndx, size_t& out_ndx_in_leaf,
                  LeafInfo& inout_leaf) const noexcept;

    void update_each(Array::UpdateHandler&);
    void update_elem(size_t, Array::UpdateHandler&);

    void adjust(size_t ndx, T diff);
    void adjust(T diff);
    void adjust_ge(T limit, T diff);

    ref_type write(size_t slice_offset, size_t slice_size,
                   size_t table_size, _impl::OutputStream& out) const;

#if defined(REALM_DEBUG)
    void verify() const;
    static size_t verify_leaf(MemRef mem, Allocator& alloc);
#endif
    static void leaf_to_dot(MemRef mem, ArrayParent* parent, size_t ndx_in_parent,
                         std::ostream& out, Allocator& alloc);
private:
    LeafType& root_as_leaf();
    const LeafType& root_as_leaf() const;

    std::unique_ptr<Array> create_root_from_ref(Allocator& alloc, ref_type ref);
    std::unique_ptr<Array> create_root_from_mem(Allocator& alloc, MemRef mem);

    struct EraseHandler;
    struct UpdateHandler;
    struct SetNullHandler;
    struct SliceHandler;
    struct AdjustHandler;
    struct AdjustGEHandler;

    struct LeafValueInserter;
    struct LeafNullInserter;

    template<class TreeTraits>
    void bptree_insert(size_t row_ndx, Array::TreeInsert<TreeTraits>& state, size_t num_rows);
};


/// Implementation:

inline BpTreeBase::BpTreeBase(std::unique_ptr<Array> root) : m_root(std::move(root))
{
}

inline
Allocator& BpTreeBase::get_alloc() const noexcept
{
    return m_root->get_alloc();
}

inline
void BpTreeBase::destroy() noexcept
{
    if (m_root)
        m_root->destroy_deep();
}

inline
void BpTreeBase::detach()
{
    m_root->detach();
}

inline
bool BpTreeBase::is_attached() const noexcept
{
    return m_root->is_attached();
}

inline
bool BpTreeBase::root_is_leaf() const noexcept
{
    return !m_root->is_inner_bptree_node();
}

inline
void BpTreeBase::set_parent(ArrayParent* parent, size_t ndx_in_parent) noexcept
{
    m_root->set_parent(parent, ndx_in_parent);
}

inline
size_t BpTreeBase::get_ndx_in_parent() const noexcept
{
    return m_root->get_ndx_in_parent();
}

inline
void BpTreeBase::set_ndx_in_parent(size_t ndx) noexcept
{
    m_root->set_ndx_in_parent(ndx);
}

inline
void BpTreeBase::update_from_parent(size_t old_baseline) noexcept
{
    m_root->update_from_parent(old_baseline);
}

inline
MemRef BpTreeBase::clone_deep(Allocator& alloc) const
{
    return m_root->clone_deep(alloc);
}

inline
const Array& BpTreeBase::root() const noexcept
{
    return *m_root;
}

inline
Array& BpTreeBase::root() noexcept
{
    return *m_root;
}

template<class T, bool N>
BpTree<T,N>::BpTree() : BpTree(Allocator::get_default())
{
}

template<class T, bool N>
BpTree<T,N>::BpTree(Allocator& alloc) : BpTreeBase(std::unique_ptr<Array>(new LeafType(alloc)))
{
}

template<class T, bool N>
BpTree<T,N>::BpTree(BpTreeBase::unattached_tag) : BpTreeBase(nullptr)
{
}

template<class T, bool N>
std::unique_ptr<Array> BpTree<T,N>::create_root_from_mem(Allocator& alloc, MemRef mem)
{
    const char* header = mem.m_addr;
    std::unique_ptr<Array> new_root;
    if (Array::get_is_inner_bptree_node_from_header(header)) {
        new_root.reset(new Array{alloc});
        new_root->init_from_mem(mem);
    }
    else {
        std::unique_ptr<LeafType> leaf { new LeafType{alloc} };
        leaf->init_from_mem(mem);
        new_root = std::move(leaf);
    }
    return new_root;
}

template<class T, bool N>
std::unique_ptr<Array> BpTree<T,N>::create_root_from_ref(Allocator& alloc, ref_type ref)
{
    const char* header = alloc.translate(ref);
    std::unique_ptr<Array> new_root;
    if (Array::get_is_inner_bptree_node_from_header(header)) {
        new_root.reset(new Array{alloc});
        new_root->init_from_ref(ref);
    }
    else {
        std::unique_ptr<LeafType> leaf { new LeafType{alloc} };
        leaf->init_from_ref(ref);
        new_root = std::move(leaf);
    }
    return new_root;
}

template<class T, bool N>
void BpTree<T,N>::init_from_ref(Allocator& alloc, ref_type ref)
{
    auto new_root = create_root_from_ref(alloc, ref);
    replace_root(std::move(new_root));
}

template<class T, bool N>
void BpTree<T,N>::init_from_mem(Allocator& alloc, MemRef mem)
{
    auto new_root = create_root_from_mem(alloc, mem);
    replace_root(std::move(new_root));
}

template<class T, bool N>
void BpTree<T,N>::init_from_parent()
{
    ref_type ref = root().get_ref_from_parent();
    auto new_root = create_root_from_ref(get_alloc(), ref);
    new_root->set_parent(m_root->get_parent(), m_root->get_ndx_in_parent());
    m_root = std::move(new_root);
}

template<class T, bool N>
typename BpTree<T,N>::LeafType&
BpTree<T,N>::root_as_leaf()
{
    REALM_ASSERT_DEBUG(root_is_leaf());
    REALM_ASSERT_DEBUG(dynamic_cast<LeafType*>(m_root.get()) != nullptr);
    return static_cast<LeafType&>(root());
}

template<class T, bool N>
const typename BpTree<T,N>::LeafType&
BpTree<T,N>::root_as_leaf() const
{
    REALM_ASSERT_DEBUG(root_is_leaf());
    REALM_ASSERT_DEBUG(dynamic_cast<const LeafType*>(m_root.get()) != nullptr);
    return static_cast<const LeafType&>(root());
}

template<class T, bool N>
size_t BpTree<T, N>::size() const noexcept
{
    if (root_is_leaf()) {
        return root_as_leaf().size();
    }
    return root().get_bptree_size();
}

template<class T, bool N>
T BpTree<T, N>::back() const noexcept
{
    // FIXME: slow
    return get(size()-1);
}

namespace _impl {

// NullableOrNothing encapsulates the behavior of nullable and
// non-nullable leaf types, so that non-nullable leaf types
// don't have to implement is_null/set_null but BpTree can still
// support the interface (and return false / assert when null
// is not supported).
template<class Leaf, bool Nullable>
struct NullableOrNothing;
template<class Leaf>
struct NullableOrNothing<Leaf, true> {
    static bool is_null(const Leaf& leaf, size_t ndx)
    {
        return leaf.is_null(ndx);
    }
    static void set_null(Leaf& leaf, size_t ndx)
    {
        leaf.set_null(ndx);
    }
};
template<class Leaf>
struct NullableOrNothing<Leaf, false> {
    static bool is_null(const Leaf&, size_t)
    {
        return false;
    }
    static void set_null(Leaf&, size_t)
    {
        REALM_ASSERT_RELEASE(false);
    }
};

}

template<class T, bool N>
bool BpTree<T, N>::is_null(size_t ndx) const noexcept
{
    if (!N) {
        // Don't bother traversing the tree if we know it can't be null.
        return false;
    }

    if (root_is_leaf()) {
        return _impl::NullableOrNothing<LeafType, N>::is_null(root_as_leaf(), ndx);
    }
    LeafType fallback(get_alloc());
    const LeafType* leaf;
    LeafInfo leaf_info { &leaf, &fallback };
    size_t ndx_in_leaf;
    get_leaf(ndx, ndx_in_leaf, leaf_info);
    return _impl::NullableOrNothing<LeafType, N>::is_null(*leaf, ndx_in_leaf);
}

template<class T, bool N>
T BpTree<T, N>::get(size_t ndx) const noexcept
{
    REALM_ASSERT_DEBUG(ndx < size());
    if (root_is_leaf()) {
        return root_as_leaf().get(ndx);
    }

    // Use direct getter to avoid initializing leaf array:
    std::pair<MemRef, size_t> p = root().get_bptree_leaf(ndx);
    const char* leaf_header = p.first.m_addr;
    size_t ndx_in_leaf = p.second;
    return LeafType::get(leaf_header, ndx_in_leaf);
}

template<class T, bool N>
template<class TreeTraits>
void BpTree<T,N>::bptree_insert(size_t row_ndx, Array::TreeInsert<TreeTraits>& state, size_t num_rows)
{
    ref_type new_sibling_ref;
    for (size_t i = 0; i < num_rows; ++i) {
        size_t row_ndx_2 = row_ndx == realm::npos ? realm::npos : row_ndx + i;
        if (root_is_leaf()) {
            REALM_ASSERT_DEBUG(row_ndx_2 == realm::npos || row_ndx_2 < REALM_MAX_BPNODE_SIZE);
            new_sibling_ref = root_as_leaf().bptree_leaf_insert(row_ndx_2, state.m_value, state);
        }
        else {
            if (row_ndx_2 == realm::npos) {
                new_sibling_ref = root().bptree_append(state); // Throws
            }
            else {
                new_sibling_ref = root().bptree_insert(row_ndx_2, state); // Throws
            }
        }

        if (REALM_UNLIKELY(new_sibling_ref)) {
            bool is_append = row_ndx_2 == realm::npos;
            introduce_new_root(new_sibling_ref, state, is_append);
        }
    }
}

template<class T, bool N>
struct BpTree<T,N>::LeafValueInserter {
    using value_type = T;
    T m_value;
    LeafValueInserter(T value) : m_value(std::move(value)) {}

    // TreeTraits concept:
    static ref_type
    leaf_insert(MemRef leaf_mem, ArrayParent& parent, size_t ndx_in_parent,
                Allocator& alloc, size_t ndx_in_leaf, Array::TreeInsert<LeafValueInserter>& state)
    {
        LeafType leaf { alloc };
        leaf.init_from_mem(leaf_mem);
        leaf.set_parent(&parent, ndx_in_parent);
        // Should not move out of m_value, because the same inserter may be used to perform
        // multiple insertions (for example, if num_rows > 1).
        return leaf.bptree_leaf_insert(ndx_in_leaf, state.m_value, state);
    }
};

template<class T, bool N>
struct BpTree<T,N>::LeafNullInserter {
    using value_type = null;
    // TreeTraits concept:
    static ref_type
    leaf_insert(MemRef leaf_mem, ArrayParent& parent, size_t ndx_in_parent,
                Allocator& alloc, size_t ndx_in_leaf, Array::TreeInsert<LeafNullInserter>& state)
    {
        LeafType leaf { alloc };
        leaf.init_from_mem(leaf_mem);
        leaf.set_parent(&parent, ndx_in_parent);
        return leaf.bptree_leaf_insert(ndx_in_leaf, null{}, state);
    }
};

template<class T, bool N>
void BpTree<T, N>::insert(size_t row_ndx, T value, size_t num_rows)
{
    REALM_ASSERT_DEBUG(row_ndx == npos || row_ndx < size());
    Array::TreeInsert<LeafValueInserter> inserter;
    inserter.m_value = std::move(value);
    inserter.m_nullable = N;
    bptree_insert(row_ndx, inserter, num_rows); // Throws
}

template<class T, bool N>
void BpTree<T, N>::insert(size_t row_ndx, null, size_t num_rows)
{
    static_assert(N, "BpTree is not nullable.");
    Array::TreeInsert<LeafNullInserter> inserter;
    inserter.m_nullable = true;
    bptree_insert(row_ndx, inserter, num_rows); // Throws
}

template<class T, bool N>
struct BpTree<T,N>::UpdateHandler : Array::UpdateHandler
{
    LeafType m_leaf;
    const T m_value;
    UpdateHandler(BpTreeBase& tree, T value) noexcept:
        m_leaf(tree.get_alloc()), m_value(std::move(value)) {}
    void update(MemRef mem, ArrayParent* parent, size_t ndx_in_parent,
                size_t elem_ndx_in_leaf) override
    {
        m_leaf.init_from_mem(mem);
        m_leaf.set_parent(parent, ndx_in_parent);
        m_leaf.set(elem_ndx_in_leaf, m_value); // Throws
    }
};

template<class T, bool N>
struct BpTree<T,N>::SetNullHandler : Array::UpdateHandler
{
    LeafType m_leaf;
    SetNullHandler(BpTreeBase& tree) noexcept: m_leaf(tree.get_alloc()) {}
    void update(MemRef mem, ArrayParent* parent, size_t ndx_in_parent,
                size_t elem_ndx_in_leaf) override
    {
        m_leaf.init_from_mem(mem);
        m_leaf.set_parent(parent, ndx_in_parent);
        _impl::NullableOrNothing<LeafType,N>::set_null(m_leaf, elem_ndx_in_leaf); // Throws
    }
};

template<class T, bool N>
void BpTree<T, N>::set(size_t ndx, T value)
{
    if (root_is_leaf()) {
        root_as_leaf().set(ndx, std::move(value));
    }
    else {
        UpdateHandler set_leaf_elem(*this, std::move(value));
        m_root->update_bptree_elem(ndx, set_leaf_elem); // Throws
    }
}

template<class T, bool N>
void BpTree<T, N>::set(size_t ndx, null)
{
    set_null(ndx);
}

template<class T, bool N>
void BpTree<T,N>::set_null(size_t ndx)
{
    if (!N) {
        // Don't bother traversing the tree if we know it can't be null.
        return;
    }
    if (root_is_leaf()) {
        _impl::NullableOrNothing<LeafType,N>::set_null(root_as_leaf(), ndx);
    }
    else {
        SetNullHandler set_leaf_elem(*this);
        m_root->update_bptree_elem(ndx, set_leaf_elem); // Throws;
    }
}

template<class T, bool N>
struct BpTree<T,N>::EraseHandler : Array::EraseHandler {
    BpTreeBase& m_tree;
    LeafType m_leaf;
    bool m_leaves_have_refs; // FIXME: Should be able to eliminate this.
    EraseHandler(BpTreeBase& tree) noexcept:
        m_tree(tree),
        m_leaf(tree.get_alloc()),
        m_leaves_have_refs(false) {}
    bool erase_leaf_elem(MemRef leaf_mem, ArrayParent* parent,
                         size_t leaf_ndx_in_parent,
                         size_t elem_ndx_in_leaf) override
    {
        m_leaf.init_from_mem(leaf_mem);
        REALM_ASSERT_3(m_leaf.size(), >=, 1);
        size_t last_ndx = m_leaf.size() - 1;
        if (last_ndx == 0) {
            m_leaves_have_refs = m_leaf.has_refs();
            return true;
        }
        m_leaf.set_parent(parent, leaf_ndx_in_parent);
        size_t ndx = elem_ndx_in_leaf;
        if (ndx == npos)
            ndx = last_ndx;
        m_leaf.erase(ndx); // Throws
        return false;
    }
    void destroy_leaf(MemRef leaf_mem) noexcept override
    {
        // FIXME: Seems like this would cause file space leaks if
        // m_leaves_have_refs is true, but consider carefully how
        // m_leaves_have_refs get its value.
        m_tree.get_alloc().free_(leaf_mem);
    }
    void replace_root_by_leaf(MemRef leaf_mem) override
    {
        std::unique_ptr<LeafType> leaf{new LeafType(m_tree.get_alloc())}; // Throws
        leaf->init_from_mem(leaf_mem);
        m_tree.replace_root(std::move(leaf)); // Throws
    }
    void replace_root_by_empty_leaf() override
    {
        std::unique_ptr<LeafType> leaf{new LeafType(m_tree.get_alloc())}; // Throws
        leaf->create(m_leaves_have_refs ? Array::type_HasRefs :
                     Array::type_Normal); // Throws
        m_tree.replace_root(std::move(leaf)); // Throws
    }
};

template<class T, bool N>
void BpTree<T,N>::erase(size_t ndx, bool is_last)
{
    REALM_ASSERT_DEBUG(ndx < size());
    REALM_ASSERT_DEBUG(is_last == (ndx == size()-1));
    if (root_is_leaf()) {
        root_as_leaf().erase(ndx);
    }
    else {
        size_t ndx_2 = is_last ? npos : ndx;
        EraseHandler handler(*this);
        Array::erase_bptree_elem(m_root.get(), ndx_2, handler);
    }
}

template<class T, bool N>
void BpTree<T, N>::move_last_over(size_t row_ndx, size_t last_row_ndx)
{
    // Copy value from last row over
    if (is_null(last_row_ndx)) {
        set_null(row_ndx);
    }
    else {
        int_fast64_t value = get(last_row_ndx);
        set(row_ndx, value);
    }
    erase(last_row_ndx, true);
}

template<class T, bool N>
void BpTree<T,N>::clear()
{
    if (root_is_leaf()) {
        if (std::is_same<T, int64_t>::value && !N && root().get_type() == Array::type_HasRefs) {
            // FIXME: This is because some column types rely on integer columns
            // to contain refs.
            root().clear_and_destroy_children();
        }
        else {
            root_as_leaf().clear();
        }
    }
    else {
        root().clear_and_destroy_children();

        // Reinitialize the root's memory as a leaf.
        Allocator& alloc = get_alloc();
        std::unique_ptr<LeafType> new_root(new LeafType(alloc));
        new_root->init_from_mem(MemRef{root().get_ref(), alloc});
        new_root->set_type(Array::type_Normal);
        replace_root(std::move(new_root));
    }
}


template<class T, bool N>
struct BpTree<T,N>::AdjustHandler : Array::UpdateHandler {
    LeafType m_leaf;
    const T m_diff;
    AdjustHandler(BpTreeBase& tree, T diff) : m_leaf(tree.get_alloc()),
        m_diff(diff)
    {}

    void update(MemRef mem, ArrayParent* parent, size_t ndx_in_parent, size_t) final
    {
        m_leaf.init_from_mem(mem);
        m_leaf.set_parent(parent, ndx_in_parent);
        m_leaf.adjust(0, m_leaf.size(), m_diff);
    }
};

template<class T, bool N>
void BpTree<T, N>::adjust(T diff)
{
    if (root_is_leaf()) {
        root_as_leaf().adjust(0, m_root->size(), std::move(diff)); // Throws
    }
    else {
        AdjustHandler adjust_leaf_elem(*this, std::move(diff));
        m_root->update_bptree_leaves(adjust_leaf_elem); // Throws
    }
}

template<class T, bool N>
void BpTree<T, N>::adjust(size_t ndx, T diff)
{
    static_assert(!N, "adjust is undefined for nullable integer arrays.");
    set(ndx, get(ndx) + diff);
}

template<class T, bool N>
struct BpTree<T,N>::AdjustGEHandler : Array::UpdateHandler {
    LeafType m_leaf;
    const T m_limit, m_diff;

    AdjustGEHandler(BpTreeBase& tree, T limit, T diff) : m_leaf(tree.get_alloc()),
        m_limit(limit), m_diff(diff)
    {}

    void update(MemRef mem, ArrayParent* parent, size_t ndx_in_parent, size_t) final
    {
        m_leaf.init_from_mem(mem);
        m_leaf.set_parent(parent, ndx_in_parent);
        m_leaf.adjust_ge(m_limit, m_diff);
    }
};

template<class T, bool N>
void BpTree<T, N>::adjust_ge(T limit, T diff)
{
    if (root_is_leaf()) {
        root_as_leaf().adjust_ge(std::move(limit), std::move(diff)); // Throws
    }
    else {
        AdjustGEHandler adjust_leaf_elem(*this, std::move(limit), std::move(diff));
        m_root->update_bptree_leaves(adjust_leaf_elem); // Throws
    }
}

template<class T, bool N>
struct BpTree<T,N>::SliceHandler : public BpTreeBase::SliceHandler {
public:
    SliceHandler(Allocator& alloc): m_leaf(alloc) {}
    MemRef slice_leaf(MemRef leaf_mem, size_t offset, size_t size,
                      Allocator& target_alloc) override
    {
        m_leaf.init_from_mem(leaf_mem);
        return m_leaf.slice_and_clone_children(offset, size, target_alloc); // Throws
    }
private:
    LeafType m_leaf;
};

template<class T, bool N>
ref_type BpTree<T,N>::write(size_t slice_offset, size_t slice_size,
                            size_t table_size, _impl::OutputStream& out) const
{
    ref_type ref;
    if (root_is_leaf()) {
        Allocator& alloc = Allocator::get_default();
        MemRef mem = root_as_leaf().slice_and_clone_children(slice_offset, slice_size, alloc); // Throws
        Array slice(alloc);
        _impl::DeepArrayDestroyGuard dg(&slice);
        slice.init_from_mem(mem);
        bool deep = true;
        bool only_when_modified = false;
        ref = slice.write(out, deep, only_when_modified); // Throws
    }
    else {
        SliceHandler handler(get_alloc());
        ref = write_subtree(root(), slice_offset, slice_size,
                                table_size, handler, out); // Throws
    }
    return ref;
}

template<class T, bool N>
MemRef BpTree<T, N>::create_leaf(Array::Type leaf_type, size_t size, T value, Allocator& alloc)
{
    bool context_flag = false;
    return LeafType::create_array(leaf_type, context_flag, size, std::move(value), alloc);
}

template<class T, bool N>
void BpTree<T, N>::get_leaf(size_t ndx, size_t& ndx_in_leaf,
                                  LeafInfo& inout_leaf_info) const noexcept
{
    if (root_is_leaf()) {
        ndx_in_leaf = ndx;
        *inout_leaf_info.out_leaf = &root_as_leaf();
        return;
    }
    std::pair<MemRef, size_t> p = root().get_bptree_leaf(ndx);
    inout_leaf_info.fallback->init_from_mem(p.first);
    ndx_in_leaf = p.second;
    *inout_leaf_info.out_leaf = inout_leaf_info.fallback;
}

template<class T, bool N>
size_t BpTree<T, N>::find_first(T value, size_t begin, size_t end) const
{
    if (root_is_leaf()) {
        return root_as_leaf().find_first(value, begin, end);
    }

    // FIXME: It would be better to always require that 'end' is
    // specified explicitly, since Table has the size readily
    // available, and Array::get_bptree_size() is deprecated.
    if (end == npos)
        end = size();

    LeafType leaf_cache(get_alloc());
    size_t ndx_in_tree = begin;
    while (ndx_in_tree < end) {
        const LeafType* leaf;
        LeafInfo leaf_info { &leaf, &leaf_cache };
        size_t ndx_in_leaf;
        get_leaf(ndx_in_tree, ndx_in_leaf, leaf_info);
        size_t leaf_offset = ndx_in_tree - ndx_in_leaf;
        size_t end_in_leaf = std::min(leaf->size(), end - leaf_offset);
        size_t ndx = leaf->find_first(value, ndx_in_leaf, end_in_leaf); // Throws (maybe)
        if (ndx != not_found)
            return leaf_offset + ndx;
        ndx_in_tree = leaf_offset + end_in_leaf;
    }

    return not_found;
}

template<class T, bool N>
void BpTree<T,N>::find_all(IntegerColumn& result, T value, size_t begin, size_t end) const
{
    if (root_is_leaf()) {
        root_as_leaf().find_all(&result, value, 0, begin, end); // Throws
        return;
    }

    // FIXME: It would be better to always require that 'end' is
    // specified explicitely, since Table has the size readily
    // available, and Array::get_bptree_size() is deprecated.
    if (end == npos)
        end = size();

    LeafType leaf_cache(get_alloc());
    size_t ndx_in_tree = begin;
    while (ndx_in_tree < end) {
        const LeafType* leaf;
        LeafInfo leaf_info { &leaf, &leaf_cache };
        size_t ndx_in_leaf;
        get_leaf(ndx_in_tree, ndx_in_leaf, leaf_info);
        size_t leaf_offset = ndx_in_tree - ndx_in_leaf;
        size_t end_in_leaf = std::min(leaf->size(), end - leaf_offset);
        leaf->find_all(&result, value, leaf_offset, ndx_in_leaf, end_in_leaf); // Throws
        ndx_in_tree = leaf_offset + end_in_leaf;
    }
}

#if defined(REALM_DEBUG)
template<class T, bool N>
size_t BpTree<T, N>::verify_leaf(MemRef mem, Allocator& alloc)
{
    LeafType leaf(alloc);
    leaf.init_from_mem(mem);
    leaf.verify();
    return leaf.size();
}

template<class T, bool N>
void BpTree<T, N>::verify() const
{
    if (root_is_leaf()) {
        root_as_leaf().verify();
    }
    else {
        root().verify_bptree(&verify_leaf);
    }
}
#endif // REALM_DEBUG

template<class T, bool N>
void BpTree<T,N>::leaf_to_dot(MemRef leaf_mem, ArrayParent* parent, size_t ndx_in_parent,
                                    std::ostream& out, Allocator& alloc)
{
    LeafType leaf(alloc);
    leaf.init_from_mem(leaf_mem);
    leaf.set_parent(parent, ndx_in_parent);
    leaf.to_dot(out);
}

} // namespace realm

#endif // REALM_BPTREE_HPP
