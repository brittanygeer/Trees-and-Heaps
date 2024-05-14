#include "filesystem.hpp"

#include <iostream>

using namespace cs251;

filesystem::filesystem(const size_t sizeLimit) {
    m_sizeLimit = sizeLimit;
    m_currentSize = 0;
    m_fileSystemNodes;
    m_maxHeap;
}

bool filesystem::exist(const handle targetHandle) {
    for (const tree_node<filesystem_node_data>& node : m_fileSystemNodes.peek_nodes()) {
        if ((node.get_handle() == targetHandle) && (!node.is_recycled())) {
            return true;
        }
    }
    return false;
}

handle filesystem::create_file(const size_t fileSize, const std::string& fileName) {
    handle parentHandle = 0;
    if (!exist(parentHandle)) {
        throw invalid_handle();
    }
    if (m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : fileName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    if (fileSize > get_available_size()) {
        throw exceeds_size();   
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(parentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == fileName) {
            throw file_exists();
        }
    }
	filesystem_node_data file;
    file.m_type = node_type::File;
    file.m_name = fileName;
    file.m_fileSize = fileSize;
    m_currentSize += fileSize;
    handle fileHandle = m_fileSystemNodes.allocate(parentHandle);
    m_fileSystemNodes.ref_node(fileHandle).ref_data() = file;
    m_maxHeap.push(fileSize, fileHandle);
    return fileHandle;
}

handle filesystem::create_directory(const std::string& directoryName) {
    handle parentHandle = 0;
    if (!exist(parentHandle)) {
        throw invalid_handle();
    }
    if (m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : directoryName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(parentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == directoryName) {
            throw directory_exists();
        }
    }
	filesystem_node_data directory;
    directory.m_type = node_type::Directory;
    directory.m_name = directoryName;
    handle directoryHandle = m_fileSystemNodes.allocate(parentHandle);
    m_fileSystemNodes.ref_node(directoryHandle).ref_data() = directory;
    return directoryHandle;
}

handle filesystem::create_link(const handle targetHandle, const std::string& linkName) {
    handle parentHandle = 0;
    if ((!exist(parentHandle)) || (!exist(targetHandle))) {
        throw invalid_handle();
    }
    if (m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : linkName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(parentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == linkName) {
            throw link_exists();
        }
    }
	filesystem_node_data link;
    link.m_type = node_type::Link;
    link.m_linkedHandle = targetHandle;
    link.m_name = linkName;
    handle linkHandle = m_fileSystemNodes.allocate(parentHandle);
    m_fileSystemNodes.ref_node(linkHandle).ref_data() = link;
    return linkHandle;
}

handle filesystem::create_file(const size_t fileSize, const std::string& fileName, const handle parentHandle) {
    if (!exist(parentHandle)) {
        throw invalid_handle();
    }
    node_type parentType = m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type;
    handle newParentHandle = parentHandle;
    if (parentType == node_type::Link) {
        newParentHandle = follow(parentHandle);    
    }
    if (m_fileSystemNodes.ref_node(newParentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : fileName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    if (fileSize > get_available_size()) {
        throw exceeds_size();   
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(newParentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == fileName) {
            throw file_exists();
        }
    }
	filesystem_node_data file;
    file.m_type = node_type::File;
    file.m_name = fileName;
    file.m_fileSize = fileSize;
    m_currentSize += fileSize;
    handle fileHandle = m_fileSystemNodes.allocate(newParentHandle);
    m_fileSystemNodes.ref_node(fileHandle).ref_data() = file;
    m_maxHeap.push(fileSize, fileHandle);
    return fileHandle;
}

handle filesystem::create_directory(const std::string& directoryName, const handle parentHandle) {
    if (!exist(parentHandle)) {
        throw invalid_handle();
    }
    node_type parentType = m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type;
    handle newParentHandle = parentHandle;
    if (parentType == node_type::Link) {
        newParentHandle = follow(parentHandle);    
    }
    if (m_fileSystemNodes.ref_node(newParentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : directoryName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(newParentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == directoryName) {
            throw directory_exists();
        }
    }
	filesystem_node_data directory;
    directory.m_type = node_type::Directory;
    directory.m_name = directoryName;
    handle directoryHandle = m_fileSystemNodes.allocate(newParentHandle);
    m_fileSystemNodes.ref_node(directoryHandle).ref_data() = directory;
    return directoryHandle;
}

handle filesystem::create_link(const handle targetHandle, const std::string& linkName, const handle parentHandle) {
    if ((!exist(parentHandle)) || (!exist(targetHandle))) {
        throw invalid_handle();
    }
    node_type parentType = m_fileSystemNodes.ref_node(parentHandle).ref_data().m_type;
    handle newParentHandle = parentHandle;
    if (parentType == node_type::Link) {
        newParentHandle = follow(parentHandle);    
    }
    if (m_fileSystemNodes.ref_node(newParentHandle).ref_data().m_type != node_type::Directory) {
        throw invalid_handle();   
    }
    for (char c : linkName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(newParentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == linkName) {
            throw link_exists();
        }
    }
    filesystem_node_data link;
    link.m_type = node_type::Link;
    link.m_linkedHandle = targetHandle;
    link.m_name = linkName;
    handle linkHandle = m_fileSystemNodes.allocate(newParentHandle);
    m_fileSystemNodes.ref_node(linkHandle).ref_data() = link;
    return linkHandle;
}

bool filesystem::remove(const handle targetHandle) {
	if (!exist(targetHandle) || targetHandle == 0) {
        throw invalid_handle();    
    }
    node_type type = m_fileSystemNodes.ref_node(targetHandle).ref_data().m_type;
    if (type == node_type::Directory) {
        if (m_fileSystemNodes.ref_node(targetHandle).peek_children_handles().empty()) {
            m_fileSystemNodes.remove(targetHandle);
            return true;
        }
        return false;  
    }
    if (type == node_type::File) {
        m_currentSize -= m_fileSystemNodes.ref_node(targetHandle).ref_data().m_fileSize;
        m_maxHeap.remove(targetHandle);
    }
    m_fileSystemNodes.remove(targetHandle);
    return true;    
}

void filesystem::rename(const handle targetHandle, const std::string& newName) {
	if (!exist(targetHandle) || targetHandle == 0) {
        throw invalid_handle();    
    }
    for (char c : newName) {
        if (c == '/') {
            throw invalid_name();   
        }
    }
    handle parentHandle = m_fileSystemNodes.ref_node(targetHandle).get_parent_handle();
    std::vector<handle> children = m_fileSystemNodes.ref_node(parentHandle).peek_children_handles();
    for (handle h : children) {
        if (get_name(h) == newName) {
            throw name_exists();
        }
    }
    m_fileSystemNodes.ref_node(targetHandle).ref_data().m_name = newName;
}

std::string filesystem::get_absolute_path(const handle targetHandle) {
    if (!exist(targetHandle)) {
        throw invalid_handle();    
    }
    std::string absolutePath = "";
    handle currentHandle = targetHandle;
    while (currentHandle != 0) {
        if (!absolutePath.empty()) {
            absolutePath = "/" + absolutePath;
        }
        std::string currentName = get_name(currentHandle);
        absolutePath = currentName + absolutePath;
        currentHandle = m_fileSystemNodes.ref_node(currentHandle).get_parent_handle();
    }
    absolutePath = "/" + absolutePath;
    return absolutePath;
}

std::string filesystem::get_name(const handle targetHandle) {
	if (!exist(targetHandle)) {
        throw invalid_handle();    
    }
    return m_fileSystemNodes.ref_node(targetHandle).ref_data().m_name;
}

handle filesystem::get_handle(const std::string& absolutePath) {
    if (absolutePath == "/") {
        return 0;
    }
    std::string path = absolutePath;
    path.erase(0, 1);
    handle currentHandle = 0;
    std::string name = "";
    for (char c : path) {
        if (c != '/') {
            name += c;
        } else {
            std::vector<handle> children = m_fileSystemNodes.ref_node(currentHandle).peek_children_handles();
            bool exist = false;
            handle childHandle;
            for (handle h : children) {
                if (get_name(h) == name) {
                    exist = true;
                    childHandle = h;
                    break;
                }
            }
            if (!exist) {
                throw invalid_path();
            }
            node_type type = m_fileSystemNodes.ref_node(childHandle).ref_data().m_type;
            if (type == node_type::Link) {
                currentHandle = follow(childHandle);
            } else if (type == node_type::Directory) {
                currentHandle = childHandle;
            } else {
                throw invalid_path();
            }
            name.clear();
        }
    }
    std::vector<handle> children = m_fileSystemNodes.ref_node(currentHandle).peek_children_handles();
    bool exist = false;
    handle childHandle;
    for (handle h : children) {
        if (get_name(h) == name) {
            childHandle = h;
            exist = true;
            break;
        }
    }
    if (!exist) {
        throw invalid_path();
    }
    return childHandle;
}

handle filesystem::follow(const handle targetHandle) {
    if (!exist(targetHandle)) {
        throw invalid_handle();    
    }
    node_type type = m_fileSystemNodes.ref_node(targetHandle).ref_data().m_type;
    if (type != node_type::Link) {
        return targetHandle;    
    }
    handle linkedHandle = m_fileSystemNodes.ref_node(targetHandle).ref_data().m_linkedHandle;
    return follow(linkedHandle);    
}

size_t filesystem::get_available_size() const {
	return m_sizeLimit - m_currentSize;
}

size_t filesystem::get_file_size(const handle targetHandle) {
	if (!exist(targetHandle)) {
        throw invalid_handle();    
    }
    node_type type = m_fileSystemNodes.ref_node(targetHandle).ref_data().m_type;
    if (type == node_type::File) {
        return m_fileSystemNodes.ref_node(targetHandle).ref_data().m_fileSize;    
    }
    if (type == node_type::Directory) {
        throw invalid_handle();
    }
    if (type == node_type::Link) {
        get_file_size(m_fileSystemNodes.ref_node(targetHandle).ref_data().m_linkedHandle);
    }
}

size_t filesystem::get_file_size(const std::string& absolutePath) {
    return get_file_size(get_handle(absolutePath));
}

std::string filesystem::print_layout() {
	std::stringstream ss{};
	const auto& node = m_fileSystemNodes.ref_node(0);
	for (const auto& childHandle : node.peek_children_handles()) {
		print_traverse(0, ss, childHandle);
	}
	return ss.str();
}

void filesystem::print_traverse(const size_t level, std::stringstream& ss, const handle targetHandle) {
	auto& node = m_fileSystemNodes.ref_node(targetHandle);
	std::stringstream indentation{};
	for (auto i = level; i > 0; i--)
	{
		indentation << "\t";
	}
	std::string type{};
	switch (node.ref_data().m_type)
	{
	case node_type::Directory: type = "[D]"; break;
	case node_type::Link: type = "[L]"; break;
	case node_type::File: type = "[F]"; break;
	}
	ss << indentation.str() << type << node.ref_data().m_name;
	if (node.ref_data().m_type == node_type::Link)
	{
		try {
			const auto path = get_absolute_path(follow(node.get_handle()));
			ss << " [->" << path << "]";
		}
		catch (const std::exception& e)
		{
			ss << " [invalid]";
		}
	}
	else if (node.ref_data().m_type == node_type::File)
	{
		ss << " (size = " << std::to_string(node.ref_data().m_fileSize) << ")";
	}
	ss << std::endl;
	for (const auto& childHandle : node.peek_children_handles())
	{
		print_traverse(level + 1, ss, childHandle);
	}
}

handle filesystem::get_largest_file_handle() const {
    return m_maxHeap.top();
}