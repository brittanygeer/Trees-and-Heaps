#pragma once
#include "cstring"
#include "sstream"
#include "string"
#include "exception"
#include "vector"
#include "queue"

namespace cs251 {
	typedef int handle;

	class invalid_handle : public std::runtime_error {
		public: invalid_handle() : std::runtime_error("Invalid handle!") {} };
	class recycled_node : public std::runtime_error {
		public: recycled_node() : std::runtime_error("Node is recycled!") {} };

	template<typename tree_node_data>
	class tree_node {
		//Friend class grant private access to another class.
		template<typename tnd>
		friend class tree;

		/**
		 * The handle of current node, should be the index of current node within the vector array in tree.
		 */
		handle m_handle = -1;
		/**
		 * Whether the node is recycled.
		 */
		bool m_recycled = true;
		/**
		 * The content of the node.
		 */
		tree_node_data m_data = {};
		/**
		 * The handle of the parent node.
		 */
		handle m_parentHandle = -1;
		/**
		 * List of handles to all children.
		 */
		std::vector<handle> m_childrenHandles{};

	public:
		/**
		 * \brief Retrieve the data for this node.
		 * \return The modifiable reference to the node's data.
		 */
		tree_node_data& ref_data();
		/**
		 * \brief Check if the node is recycled.
		 * \return Whether this node is recycled or not.
		 */
		bool is_recycled() const;
		/**
		 * \brief Get the handle of this node.
		 * \return The handle of this node.
		 */
		handle get_handle() const;
		/**
		 * \brief Get the handle of this node's parent.
		 * \return The handle of this node's parent.
		 */
		handle get_parent_handle() const;
		/**
		 * \brief Get the list of handles of this node's children.
		 * \return The list of handles of this node's children.
		 */
		const std::vector<handle>& peek_children_handles() const;
	};

	template<typename tree_node_data>
	class tree {
	public:
		/**
		 * \brief The constructor of the tree class. You should allocate the root node here.
		 */
		tree();
		/**
		 * \brief Allocate a new node as root from pool or creating a new one.
		 * \return The handle of the new node.
		 */
		handle allocate(handle parentHandle);
		/**
		 * \brief Remove (recycle) a node. Remove all descendent nodes.
		 * \param handle The handle of the target node to be removed.
		 */
		void remove(handle handle);
		/**
		 * \brief Attach a node to another node as its child.
		 * \param targetHandle The handle of the target node as child.
		 * \param parentHandle The handle of the parent node.
		 */
		void set_parent(handle targetHandle, handle parentHandle);
		/**
		 * \brief Return the constant reference to the list of nodes.
		 * \return Constant reference to the list of nodes.
		 */
		const std::vector<tree_node<tree_node_data>>& peek_nodes() const;
		/**
		 * \brief Retrieve the node with its handle.
		 * \param handle The handle of the target node.
		 * \return The reference to the node.
		 */
		tree_node<tree_node_data>& ref_node(handle handle);
	private:

		/**
		 * The storage for all nodes.
		 */
		std::vector<tree_node<tree_node_data>> m_nodes {};
		/**
		 * The pool that keep track of the recycled nodes.
		 */
		std::queue<handle> m_node_pool {};
	};

	template <typename tree_node_data>
	tree_node_data& tree_node<tree_node_data>::ref_data() {
		if (!m_recycled) {
            return m_data;
        } else {
            throw recycled_node();
        }
	}

	template <typename tree_node_data>
	bool tree_node<tree_node_data>::is_recycled() const {
        return m_recycled;
	}

	template <typename tree_node_data>
	handle tree_node<tree_node_data>::get_handle() const {
        return m_handle;
	}

	template <typename tree_node_data>
	handle tree_node<tree_node_data>::get_parent_handle() const {
		if (!m_recycled) {
            return m_parentHandle;
        } else {
            throw recycled_node();
        }
	}

	template <typename tree_node_data>
	const std::vector<handle>& tree_node<tree_node_data>::peek_children_handles() const {
		if (!m_recycled) {
            return m_childrenHandles;
        } else {
            throw recycled_node();
        }
	}

	template <typename tree_node_data>
	tree<tree_node_data>::tree() {
        m_nodes.emplace_back();
        m_nodes[0].m_handle = 0;
        m_nodes[0].m_recycled = false;
        m_nodes[0].m_parentHandle = -1;
        m_nodes[0].m_data = {};
        m_nodes[0].m_childrenHandles = {};
	}

	template <typename tree_node_data>
	handle tree<tree_node_data>::allocate(handle parentHandle) {
        if ((parentHandle < 0) || (parentHandle >= static_cast<handle>(m_nodes.size()))) {
            throw invalid_handle();
        }
        if (m_nodes[parentHandle].m_recycled) {
            throw recycled_node();
        }
        handle childHandle;
		if (m_node_pool.empty()) {
            childHandle = m_nodes.size();
            m_nodes.push_back(tree_node<tree_node_data>());
        } else {
            childHandle = m_node_pool.front();
            m_node_pool.pop();
        }
        m_nodes[childHandle].m_handle = childHandle;
        m_nodes[childHandle].m_recycled = false;
        m_nodes[childHandle].m_parentHandle = parentHandle;
        m_nodes[parentHandle].m_childrenHandles.push_back(childHandle);
        return childHandle;
	}

	template <typename tree_node_data>
	void tree<tree_node_data>::remove(const handle h) {
		if ((h <= 0) || (h >= static_cast<handle>(m_nodes.size()))) {
            throw invalid_handle();
        }
        if (m_nodes[h].m_recycled) {
            throw recycled_node();
        }
        if (!m_nodes[h].m_childrenHandles.empty()) {
            std::vector<handle> children = m_nodes[h].m_childrenHandles;
            for (handle childHandle : children) {
                if ((childHandle > 0) && (h < static_cast<handle>(m_nodes.size()))) {
                    if (!m_nodes[childHandle].m_recycled) {
                        remove(childHandle);
                    }
                }
            }
        }
        handle parentHandle = m_nodes[h].m_parentHandle;
        if (parentHandle != -1) {
            std::vector<handle>& children = m_nodes[parentHandle].m_childrenHandles;
            std::vector<handle>::iterator it = children.begin();
            while (it != children.end()) {
                if (*it == h) {
                    it = children.erase(it);
                } else {
                    ++it;
                }
            }
        }
        m_nodes[h].m_childrenHandles.clear();
        m_nodes[h].m_recycled = true;
        m_nodes[h].m_parentHandle = -1;
        m_node_pool.push(h);   
	}

	template <typename tree_node_data>
	void tree<tree_node_data>::set_parent(const handle targetHandle, const handle parentHandle) {
		if ((targetHandle <= 0) || (targetHandle >= static_cast<handle>(m_nodes.size())) || (parentHandle < 0) || (parentHandle >= static_cast<handle>(m_nodes.size()))) {
            throw invalid_handle();
        }
        if (m_nodes[targetHandle].m_recycled || m_nodes[parentHandle].m_recycled) {
            throw recycled_node();
        }
        if (m_nodes[parentHandle].m_parentHandle == targetHandle) {
            throw invalid_handle();
        }
        handle oldParent = m_nodes[targetHandle].m_parentHandle;
        if ((oldParent >= 0) && (oldParent < m_nodes.size()) && (!m_nodes[oldParent].m_recycled)) {
            std::vector<handle>& oldParentsChildren = m_nodes[oldParent].m_childrenHandles;
            std::vector<handle>::iterator it = oldParentsChildren.begin();
            while (it != oldParentsChildren.end()) {
                if (*it == targetHandle) {
                    it = oldParentsChildren.erase(it);
                } else {
                    ++it;
                }
            }
        }
        m_nodes[targetHandle].m_parentHandle = parentHandle;
        m_nodes[parentHandle].m_childrenHandles.push_back(targetHandle);
    }

	template <typename tree_node_data>
	const std::vector<tree_node<tree_node_data>>& tree<tree_node_data>::peek_nodes() const {
		return m_nodes;
	}

	template <typename tree_node_data>
	tree_node<tree_node_data>& tree<tree_node_data>::ref_node(handle h) {
		if ((h < 0) || (h >= static_cast<handle>(m_nodes.size()))) {
            throw invalid_handle();
        }
        return m_nodes[h];
    }    
}