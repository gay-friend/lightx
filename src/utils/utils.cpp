#include "utils/utils.h"
namespace fs = std::filesystem;

func_create_node *load_lib(const char *file, const char *func_name, void **handle, NodeInfo *info)
{
#if __WIN32__
    handle * = LoadLibrary(file);
#else

    // 加载动态库
    *handle = dlopen(file, RTLD_LAZY);
    // 获取函数指针
    auto func_p = new func_create_node;
    auto func_get_name_p = new func_get_lib_name;
    *func_get_name_p = (func_get_lib_name)dlsym(*handle, "get_node_info");
    *info = (*func_get_name_p)();
    delete func_get_name_p;

    *func_p = (func_create_node)dlsym(*handle, func_name);
    return func_p;
#endif
}

LibManager::LibManager(const std::string &dir, const std::string func_name) : m_dir(dir), m_func_name(func_name)
{
    load();
}
LibManager::~LibManager()
{
}
void LibManager::load()
{
    fs::path path(m_dir);
    if (!fs::exists(path))
    {
        return;
    }
    fs::directory_entry entry(path);
    if (entry.status().type() != fs::file_type::directory)
    {
        return;
    }
    fs::directory_iterator files(m_dir);
    for (auto it : files)
    {
        auto file = it.path().c_str();
        void **handle = new (void *);
        std::cout << "Loading " << file << std::endl;
        auto info = new NodeInfo;
        auto func = load_lib(file, "create_node", handle, info);
        names.push_back(info->name);
        std::cout << "Load " << file << " successed. name as: " << info->name << std::endl;

        m_handle_map[info->name] = handle;
        m_func_map[info->name] = func;
    }
}
void LibManager::unload()
{
    m_func_map.clear();
    m_handle_map.clear();
    names.clear();
}
Node *LibManager::create_node(const std::string &node_name, QPointF pos)
{
    if (m_func_map.count(node_name) == 0)
    {
        return nullptr;
    }
    auto func = m_func_map[node_name];
    if (func == nullptr)
    {
        return nullptr;
    }
    return (*func)(pos);
}
