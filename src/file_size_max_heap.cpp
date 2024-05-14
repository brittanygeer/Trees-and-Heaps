#include "file_size_max_heap.hpp"
using namespace cs251;

void file_size_max_heap::push(const size_t fileSize, const handle handle) {
    file_size_max_heap_node node;
    node.m_value = fileSize;
    node.m_handle = handle;
    int i = m_nodeSize;
    m_nodes.push_back(node);
    int p = (i - 1) / 2;
    while ((i > 0) && (m_nodes[i].m_value > m_nodes[p].m_value)) {
        file_size_max_heap_node temp = m_nodes[i];
        m_nodes[i] = m_nodes[p];
        m_nodes[p] = temp;
        i = p;
        p = (i - 1) / 2;
    }          
    m_nodeSize += 1;
}

handle file_size_max_heap::top() const {
    if (m_nodes.empty()) {
        throw heap_empty();    
    }
    return(m_nodes[0].m_handle);
}

void file_size_max_heap::remove(const handle handle) {
    int index = -1;
    for (int i = 0; i < m_nodeSize; i ++) {
        if (m_nodes[i].m_handle == handle) {
            index = i;
            break;
        }
    }
    m_nodes[index] = m_nodes[m_nodeSize - 1];
    m_nodes.pop_back();
    m_nodeSize -= 1;
    int maxChild;
    while (index < m_nodeSize / 2) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        if (rightChild < m_nodeSize && m_nodes[rightChild].m_value > m_nodes[leftChild].m_value) {
            maxChild =  rightChild;   
        } else {
            maxChild = leftChild;    
        }
        if (m_nodes[maxChild].m_value > m_nodes[index].m_value) {
            file_size_max_heap_node temp = m_nodes[maxChild];
            m_nodes[maxChild] = m_nodes[index];
            m_nodes[index] = temp;
            index = maxChild;
        } else {
            break;    
        }
    }
}